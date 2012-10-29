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

#include "ControlBase.h"
#include "IScrollable.h"

namespace Gui
{

class ScrollPane : public ControlBase,
                   public IScrollable
{
	class DefaultController : public Controller<ScrollPane>
    {
    public:
        virtual bool onSize(unsigned long x, unsigned long y)
        {
            _view->resize();
            return true;
        }

		virtual bool onVScroll(WPARAM wParam, LPARAM lParam)
        {
            int code = LOWORD(wParam);
            int pos  = HIWORD(wParam);
            switch(code)
            {
                case SB_TOP:
                {
                    _view->scrollTop();
                    break;
                }
                case SB_BOTTOM:
                {
                    _view->scrollBottom();
                    break;
                }
                case SB_LINEUP:
                {
                    _view->scrollDown();
                    break;
                }
                case SB_LINEDOWN:
                {
                    _view->scrollUp();
                    break;
                }
                case SB_PAGEUP:
                {
                    break;
                }
                case SB_PAGEDOWN:
                {
                    break;
                }
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                {
                    SCROLLINFO si = {sizeof(si), SIF_TRACKPOS};
                    ::GetScrollInfo(_view->getHandle(), SB_VERT, &si);
                    _view->setVerticalScrollPos(si.nTrackPos);
                    break;
                }
            }
            return true;
        }

        virtual bool onHScroll(WPARAM wParam, LPARAM lParam)
        {
            int code = LOWORD(wParam);
            int pos  = HIWORD(wParam);
            switch(code)
            {
                case SB_THUMBPOSITION:
                case SB_THUMBTRACK:
                {
                    SCROLLINFO si = {sizeof(si), SIF_TRACKPOS};
                    ::GetScrollInfo(_view->getHandle(), SB_HORZ, &si);
                    _view->setHorizontalScrollPos(si.nTrackPos);
                    break;
                }
				case SB_LINELEFT:
				{
					_view->scrollRight();
					break;
				}
				case SB_LINERIGHT: 
				{
					_view->scrollLeft();
					break;
				}
            }
            return true;
        }
        virtual bool onMessage(UINT message, WPARAM wParam, LPARAM lParam) 
        {
            if(message == WM_PARENTNOTIFY)
            {
                switch (wParam)
                {
                    case WM_MOUSEWHEEL:
                    {
                        return onMouseWheel(wParam, lParam);
                        break;
                    }
                }
            }
            return false; 
        }

        virtual bool onMouseWheel(WPARAM wParam, LPARAM lParam)
        {
            int zDelta = (int)(short)HIWORD(wParam);
			if(zDelta < 0)
			{
				_view->scrollUp();
                _view->scrollUp();
                _view->scrollUp();
			}
			else
			{
				_view->scrollDown();
                _view->scrollDown();
                _view->scrollDown();
			}
            return false;
        }
    };

	DefaultController _defaultController;
protected:
	IView * _pane;
    int     _verticalScrollPos;
    int     _horizontalScrollPos;

public:
	ScrollPane()
		:_pane(0), _verticalScrollPos(0), _horizontalScrollPos(0)
	{
		_defaultController.setView(this);
		Gui::registerClass(L"Gui::ScrollPane");
	}

	virtual void create(const HWND parentWnd)
	{
		ControlBase::createBase(L"Gui::ScrollPane", parentWnd);
		_pane->create(_hWnd);
		resize();
	}

	virtual void setPane(IView * view)
	{
		_pane = view;
	}

    virtual std::wstring getText() const
    {
        return _pane->getText();
    }

	virtual void resize()
	{
        SCROLLINFO si = {sizeof(si), SIF_TRACKPOS};
        ::GetScrollInfo(_hWnd, SB_VERT, &si);

		setVerticalScrollPos(si.nTrackPos);
        
		::GetScrollInfo(_hWnd, SB_HORZ, &si);

		setHorizontalScrollPos(si.nTrackPos);
        setPanePosition();
		setupScrollbars();
	}
    virtual void setScrollerSize(const Dimension & size)
    {
        _pane->setPosition(Point(0 -_horizontalScrollPos, 0 - _verticalScrollPos), size);

        resize();
    }

	void setPanePosition(void)
	{
        RECT rect = getClientRect();

		if(_size != Dimension())
		{
			_pane->setPosition(
                Point(0 -_horizontalScrollPos, 0 - _verticalScrollPos), 
                Dimension(rect.right - rect.left, rect.bottom - rect.top));
		}
	}

    virtual void setupScrollbars(void)
    {
        setupVerticalScrollbar();
        setupHorizontalScrollbar();
    }

    virtual void setVerticalScrollPos(int scrollPos)
    {
        if(_verticalScrollPos != scrollPos)
        {
            int dy = _verticalScrollPos - scrollPos;
			
            scroll(0, dy);
            _verticalScrollPos = scrollPos;

            handleVerticalScroll(scrollPos);
            setupVerticalScrollbar();
        }
    }

    virtual void setHorizontalScrollPos(int scrollPos)
    {
        if(_horizontalScrollPos != scrollPos)
        {
            int dx = _horizontalScrollPos - scrollPos;

            scroll(dx, 0);
            _horizontalScrollPos = scrollPos;

            handleHorizontalScroll(scrollPos);
            setupHorizontalScrollbar();
        }
    }

    void scrollUp(void)
    {
        if(getVirtualHeight() > _size.height)
        {
            if((_verticalScrollPos + 5) < (static_cast<int>(getVirtualHeight()) - static_cast<int>(_size.height)))
            {
                setVerticalScrollPos(_verticalScrollPos + 5);
            }
            else
            {
                setVerticalScrollPos(static_cast<int>(getVirtualHeight()) - static_cast<int>(_size.height));
            }
        }
    }

    void scrollDown(void)
    {
        if((_verticalScrollPos - 5) > 0)
        {
            setVerticalScrollPos(_verticalScrollPos - 5);
        }
        else
        {
            setVerticalScrollPos(0);
        }
    }

    void scrollTop(void)
    {
        setVerticalScrollPos(0);
    }

    void scrollBottom(void)
    {
        if(getVirtualHeight() > _size.height)
        {
            setVerticalScrollPos(static_cast<int>(getVirtualHeight()) - static_cast<int>(_size.height));
        }
    }

    void scrollLeft(void)
    {
        if((_horizontalScrollPos + 5) < static_cast<int>(getVirtualWidth()))
        {
            setHorizontalScrollPos(_horizontalScrollPos + 5);
        }
    }

    void scrollRight(void)
    {
        if((_horizontalScrollPos - 5) > 0)
        {
            setHorizontalScrollPos(_horizontalScrollPos - 5);
        }
    }

	virtual void setupVerticalScrollbar(void)
    {
        SCROLLINFO si = { sizeof(si) };
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        
        RECT  clientRect = getClientRect();
        
        int height = (clientRect.bottom - clientRect.top );
        si.nPage = height;
        if((si.nPage >= static_cast<unsigned int>(getVirtualHeight())) && (_verticalScrollPos != 0))
        {
            _verticalScrollPos = 0;
            ::RedrawWindow(_hWnd, 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
        si.nPos = _verticalScrollPos;
        si.nMin  =  0;
        si.nMax  = static_cast<int>(getVirtualHeight()) - 1;

        ::SetScrollInfo(_hWnd, SB_VERT, &si, TRUE);

		::MapWindowRect(_hWnd, ::GetParent(_hWnd), &clientRect);
		clientRect.left    = clientRect.right;
		clientRect.right  += 20;
		clientRect.bottom += 20;
        ::RedrawWindow(::GetParent(_hWnd), &clientRect, 0, 
                       RDW_INVALIDATE | RDW_ALLCHILDREN /*| RDW_UPDATENOW | RDW_ERASENOW | RDW_ERASE*/);    
    }

    virtual void setupHorizontalScrollbar(void)
    {
		SCROLLINFO si = { sizeof(si) };
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        
        RECT clientRect = getClientRect();
        
        int width  = (clientRect.right  - clientRect.left);
        si.nPage = width;
        if((si.nPage >= static_cast<unsigned int>(getVirtualWidth())) && (_horizontalScrollPos != 0))
        {
            _horizontalScrollPos = 0;
            ::RedrawWindow(_hWnd, 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
        si.nPos = _horizontalScrollPos;
        si.nMin  =  0;
        si.nMax  = static_cast<int>(getVirtualWidth()) - 1;

        ::SetScrollInfo(_hWnd, SB_HORZ, &si, TRUE);

		::MapWindowRect(_hWnd, ::GetParent(_hWnd), &clientRect);
		clientRect.top     = clientRect.bottom;
		clientRect.bottom += 20;
		clientRect.right  += 20;
        ::RedrawWindow(::GetParent(_hWnd), &clientRect, 0, 
                       RDW_INVALIDATE | RDW_ALLCHILDREN /*| RDW_UPDATENOW | RDW_ERASENOW | RDW_ERASE*/);    
    }
	virtual size_t getVirtualHeight(void) const 
	{
		return _pane->getMinSize().height;
	}
    virtual size_t getVirtualWidth(void) const 
	{
		return _pane->getMinSize().width;
	}
    virtual void handleVerticalScroll  (int scrollPos) {}
    virtual void handleHorizontalScroll(int scrollPos) {}

};

};//namespace