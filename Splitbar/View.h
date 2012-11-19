/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include "../ControlBase.h"
#include "../IEventHandler.h"

namespace Gui { namespace Splitbar
{
class Event
{
public:
    std::wstring id;
    size_t       index;
    Event(const std::wstring & id_, const size_t & index_)
        :id(id_), index(index_){}
};

class View : public ControlBase,
		     public Controller<View>

{
protected:
	HCURSOR                _sizeCursor;
	HCURSOR                _verticalCursor;
	HCURSOR                _horizontalCursor;
    Point                  _lastMousePos;
    Gui::IHandler<Event> * _eventHandler;
    size_t                 _index;

public:
	View(bool isVertical, const size_t & index = 0)
        :_index(index)
	{
		Gui::registerClass(L"Gui::Splitbar::View");
		_horizontalCursor = ::LoadCursor(NULL, IDC_SIZENS);
		_verticalCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		setVertical(isVertical);
	}

    void setIndex(const size_t index)
    {
        _index = index;
    }

	virtual void setVertical(bool isVertical)
	{
		if(isVertical)
		{
			_sizeCursor = _verticalCursor;
		}
		else
		{
			_sizeCursor = _horizontalCursor;
		}
	}

	virtual void create(IView * parentView)
	{
		createBase(L"Gui::Splitbar::View", parentView);
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
}};//namespace