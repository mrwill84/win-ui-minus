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

#include "IView.h"
#include "Graphics.h"
#include "Controller.h"
#include <windowsx.h>
#include <vector>
#include "WinUtils.h"

namespace Gui
{
    Point getMouseOffset(void) 
    {
        POINT point;
        ::GetCursorPos(&point);
        return point;
    }

	///////////////////////////////////////////////////////////////////////////////////////
	class ControlBase : public IView
    {
    protected:
        HWND            _hWnd;
		Point           _position;     
		Dimension       _size;
		Dimension       _maxSize;
		Dimension       _minSize;
        Dimension       _defaultSize;
		DWORD           _exStyle;
		IMessageProcessor 
                      * _controller;
		WNDPROC         _oldProc;
		std::wstring    _text;
		DWORD           _style;

    public:
		virtual ~ControlBase(){}

		virtual HWND getHandle() const { return _hWnd; }

        virtual Point getPosition(void) const
        {
            return _position;
        }

		virtual Dimension getSize(void) const
		{
			return _size;
		}

		virtual void setPosition(const Point & point, const Dimension & size = Dimension())
		{
			bool isPositionChanged = false;
            bool isSizeChanged     = false;
            RECT oldRect = getInternalRect();

			if(_position != point)
			{
				isPositionChanged = true;
			}
            if((size != Dimension()) && (_size != size))
            {
                isSizeChanged = true;
                _size = size;
            }
			
            _position = point;

            if(_hWnd && (isPositionChanged || isSizeChanged))
            {
                //HDWP hdwp = ::BeginDeferWindowPos(1);
                //::DeferWindowPos(hdwp, _hWnd, 0, _position.x, _position.y, static_cast<int>(_size.width), static_cast<int>(_size.height), SWP_NOZORDER);
                //::EndDeferWindowPos(hdwp);
			    ::SetWindowPos(
				    _hWnd, 0, 
				    _position.x, _position.y, 
				    static_cast<int>(_size.width), static_cast<int>(_size.height), 
				    SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);

			    if(isPositionChanged)
                {
				    refresh();
                }

			    if(isSizeChanged)
                {
				    RECT newRect = getInternalRect();
                    redrawChangedParts(oldRect, newRect);
                }
            }
		}

		virtual bool canSize(const size_t width, const size_t height)
		{
			if(_minSize.width > width)
			{
				return false;
			}
			if(_minSize.height > height)
			{
				return false;
			}
			return true;
		}

		virtual void setSize(const Dimension & size)
		{
            setPosition(_position, size);
		}

        virtual void setDefaultSize(const Dimension & dimension){_defaultSize = dimension;}

		virtual Dimension getMaxSize() const { return _maxSize; }
		virtual Dimension getMinSize() const { return _minSize; }
		virtual Dimension getDefaultSize() const { return _defaultSize; }

		virtual void setMaxSize(int width, int height)
		{
			_maxSize.width  = width;
			_maxSize.height = height;
		}

		virtual void setMinSize(int width, int height)
		{
			_minSize.width  = width;
			_minSize.height = height;
		}

		virtual void setText(const std::wstring & text)
		{
			_text = text;
            if(_hWnd)
            {
			    ::SetWindowText(_hWnd, _text.c_str());
            }
		}
        
        virtual std::wstring getText(void) const { return _text; }

        virtual void enable(bool isEnabled = true){::EnableWindow(_hWnd, isEnabled);}

        virtual void refresh(const Rect & rect = Rect::Null())
		{
			RECT rect_;
            if(rect == Rect::Null())
            {
			    rect_ = getClientRect();
            }
            else
            {
                rect_ = rect;
            }
            if(_hWnd)
            {
  			    ::RedrawWindow(_hWnd, &rect_, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASENOW | RDW_ERASE | RDW_ALLCHILDREN);
            }
		}

		virtual void destroy(void) 
        {
            if(_hWnd)
            {
                ::DestroyWindow(_hWnd);
            }
            _hWnd = 0;
        }

		virtual void show(bool isShow)
		{
            if(_hWnd)
            {
			    ::ShowWindow(_hWnd, isShow?SW_SHOW:SW_HIDE);
            }
		}

        void setStyle(const DWORD style) { _style = style;}
        
        DWORD getStyle(void) const { return _style; }

        void setExStyle(const DWORD exStyle) { _exStyle = exStyle; }

		void setController(IMessageProcessor * controller)
		{
			_controller = controller;
            if(_controller && _hWnd)
            {
			    _oldProc = subclass(_hWnd);
            }
		}

        RECT getClientRect(void) const
        {
            RECT rect = {0};
            if(_hWnd)
            {
                ::GetClientRect(_hWnd, &rect);
            }
            return rect;
        }

        RECT getInternalRect(void) const
        {
            RECT rect = {0};
            rect.left   = 0;
            rect.top    = 0;
            rect.right  = static_cast<int>(_size.width);
            rect.bottom = static_cast<int>(_size.height);
            return rect;
        }
        
		bool hasPoint(const Point & point) const
        {
            RECT rect = getClientRect();
            POINT pnt = point;
            if(_hWnd)
            {
                ::MapWindowRect(_hWnd, 0, &rect);
                if((pnt.x > rect.left) && (pnt.x < rect.right) &&
                    (pnt.y > rect.top) && (pnt.y < rect.bottom))
                {
                    return true;
                }
            }
            return false;
        }

		virtual bool isWindow(void) const
		{
			return (::IsWindow(_hWnd) == TRUE);
		}

		virtual RECT getWindowRect(void) const
		{
            RECT rect = {0};
            if(_hWnd)
            {
			    ::GetWindowRect(_hWnd, &rect);
            }
			return rect;
		}

		void asyncUpdate(void)
        {
            if(_hWnd)
            {
                ::PostMessage(_hWnd, WM_ASYNC_UPDATE, 0, 0);
            }
        }

        void callDefaultProc(int msg, WPARAM wParam, LPARAM lParam)
        {
            if(_oldProc && _hWnd)
            {
                ::CallWindowProc(_oldProc, _hWnd, msg, wParam, lParam);
            }
        }

        virtual void scroll(int dx, int dy)
        {
            if(_hWnd)
            {
                ::ScrollWindowEx(_hWnd, dx, dy, 0, 0, 0, 0, SW_INVALIDATE | SW_SCROLLCHILDREN);
            }
        }

        virtual void setFocus(void)
        {
            if(_hWnd)
            {
                ::SetFocus(_hWnd);
            }
        }
#pragma warning( push )
#pragma warning( disable : 4312 4311)
		virtual WNDPROC subclass(const HWND hWnd)
		{
			_hWnd = hWnd;
			WNDPROC superProc = reinterpret_cast<WNDPROC>(::GetWindowLong(_hWnd, GWL_WNDPROC));
            if(::GetProp(_hWnd, L"OldProc"))
            {
                superProc = reinterpret_cast<WNDPROC>(::GetProp(_hWnd, L"OldProc")); //inherit and replace
            }
            else
            {
			    ::SetProp(_hWnd, L"OldProc", (HANDLE)(superProc));
            }
        	
			LRESULT (__stdcall *newProc)(HWND, UINT, WPARAM, LPARAM) = controlProc<ControlBase>;

            ::SetProp(_hWnd, L"Gui::IMessageProcessor", reinterpret_cast<HANDLE>(_controller));
			::SetWindowLong(_hWnd, GWL_WNDPROC , reinterpret_cast<long>(newProc    ));

            return superProc;
		};
#pragma warning( pop )  

	protected:
		ControlBase()
			:_hWnd(0), 
			 _controller(0), 
			 _style(0),
             _exStyle(0),
			 _oldProc(0),
			 _defaultSize(Dimension(150, 150))
		{
		}

		void createBase(const std::wstring & className, 
			            const HWND parentWnd, 
						DWORD style = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, 
			            const std::wstring & title = L"",
						DWORD exStyle = 0)
		{
            if(isWindow())
            {
                ::SetParent(_hWnd, parentWnd);
            }
            else
            {
			    (!_style)  ?_style   = style  :_style;
			    (!_exStyle)?_exStyle = exStyle:_exStyle;
                _hWnd = ::CreateWindowEx(_exStyle, 
								        className.c_str(), 
								        title.c_str(), 
								        _style,
								        _position.x ?_position.x :CW_USEDEFAULT, 
								        _position.y ?_position.y :CW_USEDEFAULT, 
								        _size.width ?static_cast<int>(_size.width ):CW_USEDEFAULT, 
								        _size.height?static_cast<int>(_size.height):CW_USEDEFAULT, 
		                                parentWnd, 
		                                0, 
								        ::GetModuleHandle(0), 
		                                this);
                if(!_hWnd)
                {
                    DWORD error = ::GetLastError();
                    throw error;
                }
			    SetWindowFont(_hWnd, GetStockFont(DEFAULT_GUI_FONT), FALSE);
                if(_controller)
                {
				    _oldProc = subclass(_hWnd);
                }
                if(!_text.empty())
                {
                    setText(_text);
                }
            }
		}
    
        void redrawChangedParts(const RECT & oldRect, const RECT & newRect)
        {
            RECT updateRect = newRect;

            if(newRect.bottom > oldRect.bottom)
            {
                updateRect.top = oldRect.bottom - 20;
                refresh(updateRect);
            }
            else if(newRect.bottom < oldRect.bottom)
            {
                updateRect.top    = newRect.bottom - 20;
                refresh(updateRect);
            }
            
            if(newRect.right > oldRect.right)
            {
                updateRect = newRect;
				updateRect.left  = oldRect.right - (newRect.right - oldRect.right + 20);
                refresh(updateRect);
            }
            else if(newRect.right < oldRect.right)
            {
                updateRect = newRect;
                updateRect.left = newRect.right - 20;
                refresh(updateRect);
            }
        }
    };
};
