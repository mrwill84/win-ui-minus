/*
 * Copyright (c) 2006-2007, Andry Korolyuk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   * Neither the name of the Andry Korolyuk nor the names of its contributors 
 *     may be used to endorse or promote products derived from this software 
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "ILayout.h"
#include "ControlBase.h"
#include "IEventHandler.h"

namespace Gui
{
class SplitBar : public ControlBase,
		         public Controller<SplitBar>

{
public:
    class Event
    {
    public:
        std::wstring id;
        size_t       index;
        Event(const std::wstring & id_, const size_t & index_)
            :id(id_), index(index_)
        {
        }
    };

protected:
	HCURSOR                _sizeCursor;
    Point                  _lastMousePos;
    Gui::IHandler<Event> * _eventHandler;
    size_t                 _index;

public:
	SplitBar(bool isVertical, const size_t & index = 0)
        :_index(index)
	{
		Gui::registerClass(L"Gui::SplitBar");
		_sizeCursor = ::LoadCursor(NULL, IDC_SIZENS);
		if(isVertical)
		{
			_sizeCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		}
	}

    void setIndex(const size_t index)
    {
        _index = index;
    }

	virtual void create(const HWND parentWindow)
	{
		createBase(L"Gui::SplitBar", parentWindow);
		setView(this);
    }
	virtual bool onPaint(WPARAM wParam, LPARAM lParam)
    { 
		Graphics graphics(_hWnd);
        graphics.setBkColor(Colors::DIALOG);
		graphics.fillRect(getClientRect());
        return true;
    }
	
    virtual bool onMouseMove(int x, int y, WPARAM wParam)
	{
		if((wParam & MK_LBUTTON) && (::GetCapture() == _hWnd))
		{
            Point mousePos(x, y);
            if(mousePos != _lastMousePos)
            {
                _eventHandler->handle(Event(L"SplitBarMove", _index));
                _lastMousePos = mousePos;
            }
		}
		else
		{
			::SetCursor(_sizeCursor);
		}
		return true;
	}
	virtual bool onLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		::SetCapture(_hWnd);
		::SetCursor(_sizeCursor);
        RECT windowRect;
		GetWindowRect(::GetParent(_hWnd), &windowRect);

        ::ClipCursor(&windowRect);
		return true;
	}
		
	virtual bool onLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		::ReleaseCapture();
        ::ClipCursor(0);
		return true;
	}

    void setEventHandler(Gui::IHandler<Event> * handler)
    {
        _eventHandler = handler;
    }
};

class Splitter
{
public:
	enum Type
	{
		HORIZONTAL,
		VERTICAL
	};

protected:
    SplitBar * _bar;
	Type       _type;
    bool       _isVisible;

public:
    Splitter(const size_t index, Type type)
        :_bar(0), _type(type), _isVisible(false)
    {
        _bar = new SplitBar(type == VERTICAL, index);
    }

    virtual void setPosition(size_t position)
    {
        Point point;
        if(_type == VERTICAL)
        {
            point.x = static_cast<int>(position) - 2;;
            point.y = 0;
        }
        else
        {
            point.x = 0;
            point.y = static_cast<int>(position) - 2;;
        }
        _bar->setPosition(point);
    }

    virtual size_t getPosition() const
    {
        if(_type == VERTICAL)
        {
            return _bar->getPosition().x + 2;
        }
        return _bar->getPosition().y + 2;
    }

    SplitBar * getBar()
    {
        return _bar;
    }

    void setVisible(bool isVisible)
    {
        _isVisible = isVisible;
        _bar->show(isVisible);
    }

    bool isVisible(void) const
    {
        return _isVisible;
    }
};

class SplitLayout : public ILayout,
                    public Gui::IHandler<SplitBar::Event>
{
protected:
    Splitter::Type         _type;
    std::vector<IView*>    _views;
    std::vector<Splitter>  _splitters;
    HWND                   _parentWnd;
    size_t                 _width;
    size_t                 _height;

public:
	static const int MIN_VIEW_SIZE = 7;

    SplitLayout(Splitter::Type type = Splitter::VERTICAL)
        :_type(type),
         _parentWnd(0),
         _width(0),
         _height(0)
    {
    }

    Splitter getSplitter(const size_t & index) const
    {
        return _splitters[index];
    }

    Splitter::Type getType(void) const
    {
        return _type;
    }

    size_t getHeight() const
    {
        return _height;
    }

	size_t getWidth() const
    {
        return _width;
    }

    /* ILayout implementation */
	virtual void add(IView * view)
    {
        addView(view);
        addSplitter();
        update();
		refreshAll();
    }

	virtual void insert(size_t index, IView * view)
	{
		insertView(index, view);
		insertSplitter(index);
		update();
		refreshAll();
	}

	virtual void setResize(const size_t width, const size_t height)
    {
        bool isInitial = false;
        if(_width == 0 && _height == 0)
        {
            isInitial = true;
        }
        _width  = width;
        _height = height;

        if(isInitial)
        {
            recalculateSplittersPositions();
        }

		if(_splitters.size())
		{
			_splitters[_splitters.size() - 1].setPosition((_type == Splitter::VERTICAL)?_width:_height);
		}

        update();
    }

	virtual void createControls(const HWND parentWnd)
    {
        _parentWnd = parentWnd;
		for(size_t index = 0; index < _views.size(); ++index)
		{
            _views[index]->create(parentWnd);
            _splitters[index].getBar()->create(parentWnd);
		}
        update();
    }

	virtual void clean(void)
    {
    }

    /* IEventHandler implementation */
    virtual void handle(const SplitBar::Event & event)
    {
        if(event.id == L"SplitBarMove")
        {
            onSplitBarMove(event.index);
        }
    }

    bool isEmpty(void) const
    {
        return (_views.size() == 0);
    }

    void removeView(size_t index)
    {
        removeSplitter(index);
        _views.erase(_views.begin() + index);
		update();
		refreshAll();
    }

	void setSplitterPosition(size_t index, size_t position)
	{
		size_t correctedPosition = position;
		if(correctedPosition < (getPreviousSplitterPosition(index) + MIN_VIEW_SIZE))
		{
			correctedPosition = getPreviousSplitterPosition(index) + MIN_VIEW_SIZE;
		}
		if(correctedPosition > (getNextSplitterPosition(index) - MIN_VIEW_SIZE))
		{
			correctedPosition = getNextSplitterPosition(index) - MIN_VIEW_SIZE;
		}
        _splitters[index].setPosition(correctedPosition);
        update();
	}

	size_t getViewsCount(void) const
	{
		return _views.size();
	}

	IView * getView(size_t index) const
	{
		return _views[index];
	}
protected:
	size_t getPreviousSplitterPosition(size_t index)
	{
		if(index > 0)
		{
			return _splitters[index - 1].getPosition();
		}
		return 0;
	}

	size_t getNextSplitterPosition(size_t index)
	{
		return _splitters[index + 1].getPosition();
	}

    void update(void)
    {
        for(size_t index = 0; index < _views.size(); ++index)
        {
            Point     position(0, 0);
            Dimension size    (_width, _height);

            if(_type == Splitter::HORIZONTAL)
            {
                position.y  = (index == 0)?0:static_cast<int>(_splitters[index - 1].getPosition() + 2);
                if(!isLastView(index))
                {
                    size.height = _splitters[index].getPosition() - position.y - 2;
                    _splitters[index].getBar()->setSize(Dimension(_width, 4));
                }
                else if((index != 0) /*&& _height*/)
                {
                    size.height = _height - _splitters[index - 1].getPosition() - 2;
                }
            }
            else
            {
                position.x = (index == 0)?0:static_cast<int>(_splitters[index - 1].getPosition() + 2);
                if(!isLastView(index))
                {
                    size.width = _splitters[index].getPosition() - position.x - 2;
                    _splitters[index].getBar()->setSize(Dimension(4, _height));
                }
                else if((index != 0) /*&& (_width)*/)
                {
                    size.width = _width - _splitters[index - 1].getPosition() - 2;
                }
            }

            _views[index]->setPosition(position, size);
            _splitters[index].getBar()->refresh();
        }
    }

	void addView(IView * view)
    {
        _views.push_back(view);

        if(_parentWnd)
        {
            view->create(_parentWnd);
        }
    }

	void insertView(size_t index, IView * view)
	{
		_views.insert(_views.begin() + index, view);
        if(_parentWnd)
        {
            view->create(_parentWnd);
        }
	}

	void insertSplitter(size_t index)
	{
		_splitters.insert(_splitters.begin() + index, Splitter(index, _type));
		recalculateSplittersIndexes();
        _splitters[index].getBar()->setEventHandler(this);
        if(_parentWnd)
        {
            _splitters[index].getBar()->create(_parentWnd);
            _splitters[index].getBar()->show(true);
        }
        recalculateSplittersPositions();
	}

    void addSplitter()
    {
        _splitters.push_back(Splitter(_views.size() - 1, _type));
        _splitters[_splitters.size() - 1].getBar()->setEventHandler(this);
        if(_splitters.size() > 1)
        {
            _splitters[_splitters.size() - 2].setVisible(true);
        }
        if(_parentWnd)
        {
            _splitters[_splitters.size() -1].getBar()->create(_parentWnd);
            _splitters[_splitters.size() -1].getBar()->show(_splitters[_splitters.size() -1].isVisible());
        }
        recalculateSplittersPositions();
    }

    void removeSplitter(size_t index)
    {
        if(_splitters.size())
        {
            _splitters[index].getBar()->destroy();
            _splitters.erase(_splitters.begin() + index);

            recalculateSplittersIndexes();
            recalculateSplittersPositions();

            if(_splitters.size() > 0)
            {
                _splitters[_splitters.size() - 1].setVisible(false);
            }
        }
    }

    void recalculateSplittersIndexes(void)
    {
        for(size_t index = 0; index < _splitters.size(); ++index)
        {
            _splitters[index].getBar()->setIndex(index);
        }
    }

    void recalculateSplittersPositions(void)
    {
        size_t fullLenght = _height;
        if(_type == Splitter::VERTICAL)
        {
            fullLenght = _width;
        }
        for(size_t index = 0; index < _splitters.size(); ++index)
        {
            _splitters[index].setPosition((fullLenght/_splitters.size()) * (index + 1));
        }
    }

    void onSplitBarMove(const size_t barIndex/*, const Point & mousePoint*/)
    {
        Point mouseOffset = getMouseOffset();
        POINT point = mouseOffset;
        ::MapWindowPoints(0, _parentWnd, &point, 1);
        if(_type == Splitter::HORIZONTAL)
        {
            setSplitterPosition(barIndex, point.y);
        }
        else
        {
            setSplitterPosition(barIndex, point.x);
        }
       
    }

    Point getMouseOffset(void) const
    {
        POINT point;
        ::GetCursorPos(&point);
        return point;
    }

    bool isLastView(const size_t & index)
    {
        return (index == (_views.size() - 1));
    }

	void refreshAll()
	{
		for(size_t index = 0; index < _views.size(); ++index)
		{
			_views[index]->refresh();
			_splitters[index].getBar()->refresh();
		}
	}
};
};//namespace