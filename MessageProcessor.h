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

#include <windows.h>
#include <zmouse.h> // for WM_MOUSEWHEEL
#include "IMessageProcessor.h"

namespace Gui
{
	class Command
	{
	public:
		enum Event
		{
			BUTTON = BN_CLICKED,
			EDIT   = EN_UPDATE
		};
	public:
		int  event;
		int  id;
		HWND handle;
	public:
		Command(WPARAM wParam, LPARAM lParam)
		{
			event  = HIWORD(wParam);
			id     = LOWORD(wParam);
			handle = (HWND)lParam;
		};
	};

    class IAsyncUpdateListener
    {
    public:
        virtual bool onAsyncUpdate() = 0;
    };
	/**
	 *	Interface definition for command listeners
	 */
	class ICommandListener
	{
	public:
		virtual bool onCommand(const Command & command) = 0;
	};

	/**
	 *	Interface definition for mouse listeners
	 */
	class IMouseListener
	{
	public:
		virtual bool onLButtonDown       (WPARAM wParam, LPARAM lParam) = 0;
		virtual bool onLButtonUp         (WPARAM wParam, LPARAM lParam) = 0;
		virtual bool onRButtonDown       (WPARAM wParam, LPARAM lParam) = 0;
		virtual bool onRButtonUp         (WPARAM wParam, LPARAM lParam) = 0;
		virtual bool onMouseWheel        (WPARAM wParam, LPARAM lParam) = 0;
		virtual bool onLButtonDoubleClick(WPARAM wParam, LPARAM lParam) = 0;
		// onLButtonClick
		// onRButtonClick
		// 
		// onRButtonDoubleClick
	};

	class IMouseMoveListener
	{
	public:
		virtual bool onMouseMove(int x, int y, WPARAM wParam) = 0;
	};
	
	class IFocusListener
	{
	public:
		virtual bool onSetFocus (void) = 0;
		virtual bool onKillFocus(void) = 0;
	};

	class ICursorListener
	{
	public:
		virtual bool onSetCursor(void) = 0;
	};

	class IMessageListener
	{
	public:
		virtual bool onMessage(UINT message, WPARAM wParam, LPARAM lParam) = 0;
	};

	class IKeyboardListener
	{
	public:
		virtual bool onKeyDown(WPARAM wParam, LPARAM lParam) = 0;
		virtual bool onKeyUp  (WPARAM wParam, LPARAM lParam) = 0;
		virtual bool onChar   (WPARAM wParam, LPARAM lParam) = 0;
	};

    class IScrollListener
    {
    public:
		virtual bool onVScroll(WPARAM wParam, LPARAM lParam) = 0;
		virtual bool onHScroll(WPARAM wParam, LPARAM lParam) = 0;
    };

    class INotifyListener
    {
    public:
        virtual bool onNotify(WPARAM wParam, LPARAM lParam) = 0;
    };
	/**
	 *	Base template class for mmessage processors
	 */
	class MessageProcessor : public IMessageProcessor
	{
	protected:
		ICommandListener     * _commandListener;
		IFocusListener       * _focusListener;
		IMouseListener       * _mouseListener;
		IMouseMoveListener   * _mouseMoveListener;
		ICursorListener      * _cursorListener;
		IMessageListener     * _messageListener;
		IKeyboardListener    * _keyboardListener;
        IAsyncUpdateListener * _asyncUpdateListener;
        IScrollListener      * _scrollListener;
        INotifyListener      * _notifyListener;
        LONG                   _messageResult;
	public:
		MessageProcessor(void)
			:_commandListener(0),
			 _focusListener(0),
			 _mouseListener(0),
			 _mouseMoveListener(0),
			 _cursorListener(0),
			 _messageListener(0),
			 _keyboardListener(0),
             _asyncUpdateListener(0),
             _scrollListener(0),
             _notifyListener(0),
             _messageResult(0)
		{
		}

        virtual void setAsyncUpdateListener(IAsyncUpdateListener * updateListener)
        {
            _asyncUpdateListener = updateListener;
        }

		virtual void setCommandListener(ICommandListener * commandListener)
		{
			_commandListener = commandListener;
		}

		virtual void setFocusListener(IFocusListener * focusListener)
		{
			_focusListener = focusListener;
		}

		virtual void setMouseListener(IMouseListener * mouseListener)
		{
			_mouseListener = mouseListener;
		}

		virtual void setMouseMoveListener(IMouseMoveListener * mouseMoveListener)
		{
			_mouseMoveListener = mouseMoveListener;
		}

		virtual void setMessageListener(IMessageListener * messageListener)
		{
			_messageListener = messageListener;
		}

		virtual void setKeyboardListener(IKeyboardListener * keyboardListener)
		{
			_keyboardListener = keyboardListener;
		}

        virtual void setScrollListener(IScrollListener * scrollListener)
        {
            _scrollListener = scrollListener;
        }

        virtual void setNotifyListener(INotifyListener * notifyListener)
        {
            _notifyListener = notifyListener;
        }
		virtual bool processMessage(UINT message, WPARAM wParam, LPARAM lParam)
		{
            _messageResult = 0;
			switch (message)
			{
                case WM_CREATE:
                {
                    return onCreate(wParam, lParam);
                    break;
                }
				case WM_DESTROY:
				{
					return onDestroy(wParam, lParam);
					break;
				}
                case WM_CLOSE:
                {
                    return onClose(wParam, lParam);
                    break;
                }
				case WM_SIZE:
				{
					return onSize(LOWORD(lParam), HIWORD(lParam));
					break;
				}
				case WM_COMMAND:
				{
					if(_commandListener)
					{
						return _commandListener->onCommand(Command(wParam, wParam));
					}
					break;
				}
				case WM_SETFOCUS:
				{
					if(_focusListener)
					{
						return _focusListener->onSetFocus();
					}
					break;
				}
				case WM_KILLFOCUS:
				{
					if(_focusListener)
					{
						return _focusListener->onKillFocus();
					}
					break;
				}
				case WM_MOUSEMOVE:
				{
					if(_mouseMoveListener)
					{
						return _mouseMoveListener->onMouseMove(LOWORD(lParam), HIWORD(lParam), wParam);
					}
					break;
				}
				case WM_SETCURSOR:
				{
					if(_cursorListener)
					{
						return _cursorListener->onSetCursor();
					}
					break;
				}
				case WM_LBUTTONDOWN:
				{
					if(_mouseListener)
					{
						return _mouseListener->onLButtonDown(wParam, lParam);
					}
					break;
				}
				case WM_LBUTTONUP:
				{
					if(_mouseListener)
					{
						return _mouseListener->onLButtonUp(wParam, lParam);
					}
					break;
				}
				case WM_RBUTTONDOWN:
				{
					if(_mouseListener)
					{
						return _mouseListener->onRButtonDown(wParam, lParam);
					}
					break;
				}
				case WM_RBUTTONUP:
				{
					if(_mouseListener)
					{
						return _mouseListener->onRButtonUp(wParam, lParam);
					}
					break;
				}
				case WM_LBUTTONDBLCLK:
				{
					if(_mouseListener)
					{
						return _mouseListener->onLButtonDoubleClick(wParam, lParam);
					}
					break;
				}
				case WM_MOUSEWHEEL:
				{
					if(_mouseListener)
					{
						return _mouseListener->onMouseWheel(wParam, lParam);
					}
					break;
				}
				case WM_KEYDOWN:
				{
					if(_keyboardListener)
					{
						return _keyboardListener->onKeyDown(wParam, lParam);
					}
					break;
				}
				case WM_KEYUP:
				{
					if(_keyboardListener)
					{
						return _keyboardListener->onKeyUp(wParam, lParam);
					}
					break;
				}
				case WM_CHAR:
				{
					if(_keyboardListener)
					{
						return _keyboardListener->onChar(wParam, lParam);
					}
					break;
				}
                case Gui::WM_ASYNC_UPDATE:
				{
					if(_asyncUpdateListener)
					{
						return _asyncUpdateListener->onAsyncUpdate();
					}
					break;
				}
                case WM_VSCROLL:
                {
                    if(_scrollListener)
                    {
                        return _scrollListener->onVScroll(wParam, lParam);
                    }
                    break;
                }
                case WM_HSCROLL:
                {
                    if(_scrollListener)
                    {
                        return _scrollListener->onHScroll(wParam, lParam);
                    }
                    break;
                }
                case WM_NOTIFY:
                    {
                        if(_notifyListener)
                        {
                            return _notifyListener->onNotify(wParam, lParam);
                        }
                        break;
                    }
                case WM_PAINT:
				{
					return onPaint(wParam, lParam);
				}
				default:
				{
					if(_messageListener)
					{
						return _messageListener->onMessage(message, wParam, lParam);
					}
					break;
				}
			}
			return false;
		}

        virtual LONG getMessageResult(void)
        {
            return _messageResult;
        }

		virtual bool onCreate (WPARAM wParam, LPARAM lParam    ){ return false; }
		virtual bool onPaint  (WPARAM wParam, LPARAM lParam    ){ return false;	}
		virtual bool onSize   (unsigned long x, unsigned long y){ return false;	}
		virtual bool onDestroy(WPARAM wParam, LPARAM lParam    ){ return false;	}
        virtual bool onClose  (WPARAM wParam, LPARAM lParam    ){ return false; }
	};
};// namespace