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

#include "TitledFrame.h"
#include "IEventHandler.h"

namespace Gui
{
    struct DockableEvent
    {
        std::wstring eventId;
        size_t       windowId;
        DockableEvent(const std::wstring & eventId_, size_t windowId_)
            :eventId(eventId_), windowId(windowId_){}

    };

    class DockableWindow : public TitledFrame
    {
    private:
        bool         _isFocused;
        HCURSOR      _dragCursor;
        IHandler<DockableEvent> * _handler;
		size_t       _id;
    public:
        DockableWindow():_isFocused(false){_dragCursor = ::LoadCursor(NULL, IDC_SIZEALL);}

		virtual void setFocused(bool isFocused)
        {
            _isFocused = isFocused;
            ::SetFocus(_frame->getHandle());
			refresh();
        }
        
        void setHandler(IHandler<DockableEvent> * handler)
        {
            _handler = handler;
        }

		IView * getChild(void) const 
		{
			return _frame;
		}

		void setId(size_t id)
		{
			_id = id;
		}

		size_t getId() const
		{
			return _id;
		}

        virtual bool onLButtonDown(WPARAM wParam, LPARAM lParam) 
        {
            _handler->handle(DockableEvent(L"ButtonDown", _id));

            POINT pt;
            pt.x = GET_X_LPARAM(::GetMessagePos());
            pt.y = GET_Y_LPARAM(::GetMessagePos());
            
            if(::DragDetect(_hWnd, pt))
            {
                ::SetCapture(_hWnd);
                ::SetCursor(_dragCursor);
                _handler->handle(DockableEvent(L"DragEntered", _id));
            }
            
            setFocused(true);
            
            return false; 
        }
		
        virtual bool onLButtonUp(WPARAM wParam, LPARAM lParam)
		{
            _handler->handle(DockableEvent(L"DragExited", _id));
            return false;
        }

		virtual bool onMouseMove(int x, int y, WPARAM wParam)
		{
			if((wParam & MK_LBUTTON) && ::GetCapture() == _hWnd)
			{
                _handler->handle(DockableEvent(L"DragMouseMove", _id));
			}
			return true;
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
                        _handler->handle(DockableEvent(L"ButtonDown", _id));
                        setFocused(true);

                        break;
                    }
                }
            }
            return false; 
        }

    protected:
        
        virtual void paintTitle(Graphics & graphics) 
        {
            Pen greyPen = Pen::create(PS_SOLID, 1, Colors::WINDOW);
			
			RECT clientRect = getClientRect();
			RECT titleRect = {0, 0, clientRect.right, 20};

			graphics.setPen(greyPen);
			graphics.rectangle(titleRect);
			
			::InflateRect(&titleRect, -1, -1);

            if(_isFocused)
            {
			    TRIVERTEX vertexes[2];
			    GRADIENT_RECT gradientRects = {0};

			    vertexes[0].x = titleRect.left;
			    vertexes[0].y = titleRect.top;
			    vertexes[0].Red   = 0xA500;
			    vertexes[0].Green = 0xDD00;
			    vertexes[0].Blue  = 0xE600;

			    vertexes[1].x = titleRect.right  - titleRect.left + 1;
			    vertexes[1].y = titleRect.bottom - titleRect.top  + 1;
			    vertexes[1].Red   = 0x9A00;
			    vertexes[1].Green = 0xAD00;
			    vertexes[1].Blue  = 0xC220;

			    gradientRects.UpperLeft  = 1;
			    gradientRects.LowerRight = 0;

			    ::GradientFill(graphics, vertexes, 2, &gradientRects, 1, GRADIENT_FILL_RECT_V);
            }
            else
            {
                graphics.setBkColor((Colors::Color)COLOR_SCROLLBAR);
                graphics.fillRect(titleRect);
            }
			::SetBkMode(graphics, TRANSPARENT);
			graphics.setFont(Gui::Fonts::DEFAULT());
            if(_isFocused)
            {
                graphics.setTextColor(Colors::WINDOW);
            }
 
            titleRect.top  += 3;
            titleRect.left += 3;
            ::DrawText(graphics, _title.c_str(), static_cast<int>(_title.size()), &titleRect , DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
        }

    };

    class PopupWindow : public ControlBase
    {
    public:
        class DefaultController : public Controller<PopupWindow>
        {
		protected:
			IHandler<DockableEvent> * _handler;
        public:
            virtual bool onSize(unsigned long x, unsigned long y)
            {
                if(_view)
                {
                    _view->setSize(Dimension(x, y));
                }
                return false;
            }

            virtual bool onMessage(UINT message, WPARAM wParam, LPARAM lParam)
            {
                if(message == WM_MOVE)
                {
                    _handler->handle(DockableEvent(L"DragMouseMove", _view->getId()));
					return true;
                }
				else if(message == WM_EXITSIZEMOVE)
				{
					_handler->handle(DockableEvent(L"DragExited", _view->getId()));
				}
                else if(message == WM_PARENTNOTIFY)
                {
                    switch (wParam)
                    {
                        case WM_LBUTTONDOWN:
                        case WM_MBUTTONDOWN:
                        case WM_RBUTTONDOWN:
                        {
                            ::SetFocus(_view->getHandle());
                            break;
                        }
                    }
                }
                else if(message == WM_CLOSE)
                {
                    _view->show(false);
                    _handler->handle(DockableEvent(L"HideWindow", _view->getId()));
                    return true;
                }

                return false;
			}

			virtual void setHandler(IHandler<DockableEvent> * handler)
			{
				_handler = handler;
			}

        };
        
    protected:
        IView * _child;
        DefaultController _defaultController;
		size_t  _id;

    public:
        PopupWindow()
            :_child(0)
        {
            _defaultController.setView(this);
            _controller = &_defaultController;
            Gui::registerClass(L"Gui::PopupWindow");
        }

        virtual void create(const HWND parentWnd)
        {
            createBase(
                L"Gui::PopupWindow", 
                parentWnd, 
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_POPUP|WS_SYSMENU|WS_SIZEBOX|WS_CAPTION,
                L"",
                WS_EX_TOOLWINDOW|WS_EX_CONTROLPARENT);
        }

		virtual void setId(size_t id)
		{
			_id = id;
		}

		size_t getId(void)
		{
			return _id;
		}

        virtual void setHandler(IHandler<DockableEvent> * handler)
        {
            _defaultController.setHandler(handler);
        }

        DefaultController * getDefaultController()
        {
            return &_defaultController;
        }

        void setRect(const RECT & rect)
        {
            _position = Point(rect.left, rect.top);
            _size     = Dimension(rect.right - rect.left, rect.bottom - rect.top);
        }

        void setChild(IView * view)
        {
            _child = view;
            if(_child->getHandle() == 0)
            {
                _child->create(_hWnd);
            }
            else
            {
                ::SetParent(_child->getHandle(), _hWnd);
            }
            _child->setPosition(Point(0, 0), _size);
            setText(_child->getText());
        }

        virtual void setSize(const Dimension & size)
        {
            if(_child)
            {
                _child->setSize(size);
                _child->refresh();
            }
        }

		virtual IView * getChild(void) const
		{
			return _child;
		}
    };

};