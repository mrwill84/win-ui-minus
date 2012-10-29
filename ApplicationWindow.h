/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
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

		void centerOnScreen(void)
		{
			if(_size.height != 0 && _size.width != 0)
			{
			}
		}
	};
}; // namespace
