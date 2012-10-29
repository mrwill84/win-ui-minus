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
#include <tchar.h>
#include <commctrl.h>
#include "ControlBase.h"
#include "MessageProcessor.h"
#include "WinUtils.h"
#include "Menu.h"

namespace Gui
{
	/**
	 *	Base class for Windows Application View
	 */
	class ApplicationWindow : public ControlBase
	{
	public:
		class DefaultController : public Controller<ApplicationWindow>
		{
		public:
			virtual bool onSize(unsigned long x, unsigned long y)
			{
				if(_view)
				{
					_view->setSize(Dimension(x, y));
				}
				return false;
			}		

			virtual bool onDestroy(WPARAM wParam, LPARAM lParam)
			{
				_view->exit(0);
				return false;
			}		
		};

	protected:
		std::wstring      _className;
		std::wstring      _title;
		IView           * _contextPane;
		IView           * _statusBar;
		IView           * _toolBar;
        DefaultController _defaultController;

	public:
		ApplicationWindow(const std::wstring & className)
			:_contextPane(0),
			 _statusBar(0),
			 _toolBar(0),
			 _className(className), 
			 _title(L"Untitled")
		{
			Gui::registerClass(className);
			_style       = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
			_exStyle     = 0;
			_position.x  = CW_USEDEFAULT;
			_position.y  = CW_USEDEFAULT;
			_size.width  = CW_USEDEFAULT;
			_size.height = CW_USEDEFAULT;
			_defaultController.setView(this);
		}
	    
		void create(const HWND parentWindow = HWND_DESKTOP)
		{
			createBase(_className, 
				       parentWindow, 
					   WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
					   _title);
		}

		void setTitle(const std::wstring & title)
		{
			setText(title);
		}

        void setMenu(Gui::Menu::View & menu)
		{
			::SetMenu(_hWnd, menu);
		}

		void setStatusBar(IView * statusBar)
		{
			_statusBar = statusBar;
			_statusBar->create(_hWnd);
		}

		void setToolBar(IView * toolBar)
		{
			_toolBar = toolBar;
			_toolBar->create(_hWnd);
		}

		void setContextPane(IView * contextPane)
		{
			_contextPane = contextPane;
			_contextPane->create(_hWnd);
		}
		
        void setIconSmall(HICON hIcon)
        {
            ::SendMessage(_hWnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        }

        void setIconBig(HICON hIcon)
        {
            ::SendMessage(_hWnd, WM_SETICON, ICON_BIG  , (LPARAM)hIcon);
        }

		int start()
		{
			::ShowWindow (_hWnd, SW_SHOW);
			::UpdateWindow(_hWnd);

			MSG message;
            BOOL returnCode = 0;
			while((returnCode = ::GetMessage(&message, 0, 0, 0)) != 0)
			{
                if(returnCode != -1)
                {
				    ::TranslateMessage(&message);
				    ::DispatchMessage(&message);
                }
			}
			return static_cast<int>(message.wParam);
		}
	    
        virtual void exit(int code)
        {
			::PostQuitMessage(code);
        }

		virtual void setSize(const Dimension & size)
		{
			size_t width  = size.width;
			size_t height = size.height;
			int posY = 0;
			if(_statusBar)
			{
				RECT rect = _statusBar->getWindowRect();
				height -= rect.bottom - rect.top;
                _statusBar->setPosition(Point(0, static_cast<int>(height)), Dimension(size.width, rect.bottom - rect.top));
			}
			if(_toolBar)
			{
				RECT rect;
				::GetWindowRect(_toolBar->getHandle(), &rect);
				posY += rect.bottom - rect.top;
				_toolBar->setPosition(Point(0, 0), Dimension(size.width, rect.bottom - rect.top));
			}
			if(_contextPane)
			{
				_contextPane->setPosition(
                    Point(0, posY),
                    Dimension(width, (static_cast<int>(height - posY) >= 0)?height - posY:0));
			}
		}
	};
}; // namespace
