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

#include "IModel.h"

#include "../ControlBase.h"
#include "../Modellable.h"
#include "../Splitbar/View.h"
#include "../DockableWindow.h"
#include "../TransparentWindow.h"

namespace Gui { namespace DockContainer
{
class INotificationHandler
{
public:
    virtual void onDockableWindowClosed(size_t windowId) = 0;
};

class CenterPane : public ControlBase,
	               public Controller<CenterPane>
{
protected:
	IView * _pane;
public:
	CenterPane()
        :_pane(0)
	{
		setView(this);
		Gui::registerClass(L"Gui::DockContainer::CenterPane");
	}

	void setPane(IView * view)
	{
		_pane = view;
	}

	virtual void create(const HWND parentWnd)
	{
		ControlBase::createBase(L"Gui::DockContainer::CenterPane", parentWnd);
		if(_pane)
		{
			_pane->create(_hWnd);
		}
	}

	virtual bool onSize(unsigned long x, unsigned long y)
    {
        _pane->setPosition(
			Point(1, 1),
			Dimension(_size.width - 2, _size.height - 2));
        return false;
    }

	virtual bool onMessage(UINT message, WPARAM wParam, LPARAM lParam) 
    {
        if(message == WM_PARENTNOTIFY)
        {
            switch (wParam)
            {
                case WM_LBUTTONDOWN:
                case WM_MBUTTONDOWN:
                case WM_RBUTTONDOWN:
                {
					::SetFocus(_pane->getHandle());
                    break;
                }
            }
        }
        return false; 
    }

	virtual bool onPaint(WPARAM wParam, LPARAM lParam)
	{
        Graphics graphics(_hWnd);
		Pen greyPen = Pen::create(PS_SOLID, 1, Colors::GREY);
		
		RECT clientRect = getClientRect();

		graphics.setPen(greyPen);
		graphics.rectangle(clientRect);

		return true;
	}

};

class View : public ControlBase,
	         public Modellable<IModel>,
			 public IHandler<Splitbar::Event>,
             public IHandler<DockableEvent>
{
public:
    class DefaultController : public Controller<View>
    {
    public:
        virtual bool onSize(unsigned long x, unsigned long y)
        {
            _view->resize();
            return false;
        }
    };

protected:
	std::vector<IView*>          _views;
	DefaultController            _defaultController;
	std::vector<Splitbar::View*> _splitbars;
    Point                       _initialMouseOffset;
    RECT                        _dragRect;
	Dock::Position              _dragTarget;
    DockableWindow            * _focusedDock;
    TransparentWindow           _transparentWindow;
	CenterPane                  _centerPane;
    INotificationHandler      * _notificationHandler;

public:
	View()
        :_focusedDock(0), 
         _dragTarget(Dock::FREE),
         _notificationHandler(0)
	{
		_defaultController.setView(this);
		Gui::registerClass(L"Gui::DockContainer::View");
	}

	virtual void create(const HWND parentWnd)
	{
		ControlBase::createBase(L"Gui::DockContainer::View", parentWnd);
		for(size_t index = 0; index < _views.size(); ++index)
		{
			_views.at(index)->create(_hWnd);
		}
        _transparentWindow.create(_hWnd);
        _transparentWindow.show(false);
		syncWithModel();
	}

	virtual void destroy(void)
	{
		for(size_t index = 0; index < _views.size(); ++index)
		{
			_views.at(index)->destroy();
		}
	}

	virtual void add(IView * view, const Dock::Position & dockPosition)
	{
		if(dockPosition != Dock::CENTER)
		{
			DockableWindow * dockableView = new DockableWindow();
			dockableView->setFrame(view);
			dockableView->setHandler(this);
			dockableView->setId(_views.size());
			_views.push_back(dockableView);
		}
		else
		{
			_centerPane.setPane(view);
			_views.push_back(&_centerPane);
		}
		_model->addDock(dockPosition);
		syncWithModel();
	}

	void resize()
	{
		_model->resize(_size.width, _size.height);
		syncWithModel();
	}

    /* IEventHandler implementations */
    virtual void handle(const Splitbar::Event & event)
    {
        if(event.id == L"SplitBarMove")
        {
			Point point = getMouseOffset();
			POINT p = point;
			::MapWindowPoints(0, _hWnd, &p, 1);

            _model->setSplitterPos(event.index, p.x, p.y);
			syncWithModel();
        }
    }

	virtual void handle(const DockableEvent & event)
    {
        if(event.eventId == L"ButtonDown")
        {
            onButtonDown(event.windowId);
        }
        else if(event.eventId == L"DragMouseMove")
        {
            onDragMouseMove(event.windowId);
        }
        else if(event.eventId == L"DragEntered")
        {
            onDragEntered();
        }
        else if(event.eventId == L"DragExited")
        {
			onDragExited(event.windowId);
        }
        else if(event.eventId == L"HideWindow")
        {
            onHideWindow(event.windowId);
        }
    }

    IView * getView(size_t index)
    {
        return _views[index];
    }

    virtual void setNotificatioinHandler(INotificationHandler * notificationHandler)
    {
        _notificationHandler = notificationHandler;
    }
    virtual void showView(size_t index, bool isShow)
    {
        _views[index]->show(isShow);
        _model->hideDock(index, !isShow);
        syncWithModel();
    }
protected:
	void syncWithModel(void)
	{
		syncViews();
		syncSplitters();
	}

	void syncViews(void)
	{
		for(size_t index = 0; index < _model->getDocksCount(); ++index)
		{
			Dock dock = _model->getDock(index);
			if(dock.dock != Dock::FREE)
			{
				_views.at(index)->setPosition(Point(dock.x, dock.y), Dimension(dock.width, dock.height));
			}
		}
	}

	void syncSplitters(void)
	{
		if(_hWnd)
		{
            while(_splitbars.size() && (_splitbars.size() > _model->getSplittersCount()))
            {
                _splitbars.at(0)->destroy();
                delete _splitbars.at(0);
                _splitbars.erase(_splitbars.begin() + 0);
            }

            for(size_t index = 0; index < _model->getSplittersCount(); ++index)
			{
				Splitter splitter = _model->getSplitter(index);
				if(_splitbars.size() == index)
				{
					Splitbar::View * splitbar = new Splitbar::View(splitter.type == Splitter::VERTICAL);
					splitbar->setEventHandler(this);
					splitbar->create(_hWnd);
					_splitbars.push_back(splitbar);
				}
				_splitbars.at(index)->setPosition(
					Point(splitter.x, splitter.y), Dimension(splitter.width, splitter.height));
				_splitbars.at(index)->setIndex(index);
				_splitbars.at(index)->setVertical(splitter.type == Splitter::VERTICAL);
			}
		}
	}
    void onButtonDown(const size_t dockId)
    {
        if(_focusedDock != findDockableWindow(dockId))
        {
            if(_focusedDock)
            {
                _focusedDock->setFocused(false);
                _focusedDock = 0;
            }
            _focusedDock = findDockableWindow(dockId);

            if(_focusedDock)
            {
                _focusedDock->setFocused(true);
            }
        }
    }

    void onDragMouseMove(size_t windowId)
    {
        Point currentMouseOffset = getMouseOffset();
        
		Dock dock = _model->getDock(windowId);
        _dragTarget = calculatePossibleDragTarget(currentMouseOffset);
		if((dock.dock == Dock::FREE) && (_dragTarget == Dock::FREE))
		{
            _transparentWindow.show(false);
		}
		else
		{
			drawDragFrame(currentMouseOffset - _initialMouseOffset);
		}
    }

    void onDragEntered()
    {
        _initialMouseOffset = getMouseOffset();
        ::memset(&_dragRect, 0, sizeof(RECT));
    }

    void onDragExited(size_t windowId)
    {
        drawXorDragFrame();
		Dock dock = _model->getDock(windowId);

		if(_dragTarget != dock.dock)
        {
			if(_dragTarget != Dock::FREE)
			{
				if(dock.dock == Dock::FREE)
				{
					PopupWindow * popupWindow = (PopupWindow*)_views[dock.id];
					IView * view = popupWindow->getChild();

					DockableWindow * dockableView = new DockableWindow();
					dockableView->create(_hWnd);
					dockableView->setFrame(view);
					dockableView->setHandler(this);
					dockableView->setId(dock.id);
					_views[dock.id] = dockableView;

                    popupWindow->destroy();
                    delete popupWindow;
				}
				
				_model->redock(dock.id, _dragTarget);
			}
			else
			{
				if(dock.dock != Dock::FREE)
				{
					_model->undock(dock.id);
                    if(_focusedDock)
                    {
					    IView * view = _focusedDock->getChild();

					    PopupWindow * popupWindow = new PopupWindow();
					    popupWindow->setRect(_dragRect);
					    popupWindow->create(_hWnd);
					    popupWindow->setChild(view);
					    popupWindow->setHandler(this);
					    popupWindow->setId(dock.id);
					    _views[dock.id] = popupWindow;

                        _focusedDock->destroy();
                        delete _focusedDock;
                        _focusedDock = 0;
                    }
				}
			}
			syncWithModel();
        }

   		::ReleaseCapture();
        _transparentWindow.show(false);
    }

    void onHideWindow(size_t windowId)
    {
        if(_notificationHandler)
        {
            _notificationHandler->onDockableWindowClosed(windowId);
        }
    }

    void drawXorDragFrame(void)
    {
        _transparentWindow.setPosition(
            Point(_dragRect.left, _dragRect.top),
            Dimension(_dragRect.right - _dragRect.left, _dragRect.bottom - _dragRect.top));
        _transparentWindow.show(true);
    }

	Dock::Position calculatePossibleDragTarget(const Point & mousePos) const
    {
		POINT point = mousePos;
		::ScreenToClient(_hWnd, &point);
		Dock::Position dragTarget = _model->getDockPosition(point.x, point.y);
        return dragTarget;
    }

	void drawDragFrame(const Point & offset)
    {
		if(_dragTarget == Dock::FREE)
        {
            if(_focusedDock)
            {
                RECT clientRect = {0};
                ::GetClientRect(_focusedDock->getHandle(), &clientRect);
                POINT frameOffset = {0};
                ::ClientToScreen(_focusedDock->getHandle(), &frameOffset);
                _dragRect.left   = frameOffset.x  + offset.x;
                _dragRect.top    = frameOffset.y  + offset.y;
                _dragRect.right  = _dragRect.left + clientRect.right  - clientRect.left;
                _dragRect.bottom = _dragRect.top  + clientRect.bottom - clientRect.top;
            }
        }
        else
        {
            _dragRect = makeDragRect(_dragTarget);
        }

        drawXorDragFrame();
    }

	DockableWindow * findDockableWindow(size_t index)
	{
		return (DockableWindow*)_views[index];
	}

	RECT makeDragRect(const Dock::Position & dragTarget) const
    {
		RECT dragRect = _model->getDockArea(dragTarget);

		POINT point = {0};
        ::ClientToScreen(_hWnd, &point);
        dragRect.left   += point.x;
        dragRect.top    += point.y;
        dragRect.right  += point.x;
        dragRect.bottom += point.y;

		return dragRect;
    }

};
}};