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
#include <commctrl.h>
#include <RichEdit.h>
#include <uxtheme.h>    //Used to access the UxTheme library
#include <tmschema.h>   //Definitions used by some of the UxTheme
                        //library functions
#include "ControlBase.h"

class RichEdit : public Gui::ControlBase
{
protected:
	std::wstring _text;
	WNDPROC _oldWndProc;
	RECT    _rcClientPos;
	//Function pointers from the UxTheme library
	HMODULE _hUxTheme;
	HTHEME (WINAPI *pOpenThemeData)(HWND, LPCWSTR);
	HRESULT (WINAPI *pCloseThemeData)(HTHEME);
	HRESULT (WINAPI *pDrawThemeBackground)(HTHEME, HDC, int, int, const RECT*, const RECT *);
	HRESULT (WINAPI *pGetThemeBackgroundContentRect)(HTHEME, HDC, int, int,  const RECT *, RECT *);
	BOOL (WINAPI *pIsThemeActive)();
	BOOL (WINAPI *pIsThemeBackgroundPartiallyTransparent)(HTHEME, int, int);
	HRESULT (WINAPI *pDrawThemeParentBackground)(HWND, HDC, RECT*);
public:
    RichEdit(const TCHAR * text)
		:ControlBase(),
		 _text(text)
    {
		_hUxTheme = NULL;
		pOpenThemeData = NULL;
		pCloseThemeData = NULL;
		pDrawThemeBackground = NULL;
		pGetThemeBackgroundContentRect = NULL;
		pIsThemeActive = NULL;
		pDrawThemeParentBackground = NULL;
		pIsThemeBackgroundPartiallyTransparent = NULL;
		ZeroMemory(&_rcClientPos, sizeof(RECT));
    }

	virtual void create(HWND parentWindow)
	{
		::InitCommonControls();
		::LoadLibrary(L"RICHED20.DLL");
		_hWnd = ::CreateWindowEx(0, RICHEDIT_CLASS, 
				                NULL, 
								WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOHSCROLL,
				                0, 0, 0, 0, 
								parentWindow, 
								NULL, 
								GetModuleHandle(0), 
								NULL);
		SetWindowFont(_hWnd, GetStockFont(DEFAULT_GUI_FONT), FALSE);
		::SetWindowText(_hWnd, _text.c_str());

		initializeLibrary();
		
		if(GetWindowLong(_hWnd, GWL_STYLE) & WS_BORDER)
        {
			::SetWindowLong(_hWnd, GWL_STYLE,
							::GetWindowLong(_hWnd, GWL_STYLE)^WS_BORDER);
        }
        if(GetWindowLong(_hWnd, GWL_EXSTYLE) &
                         WS_EX_CLIENTEDGE)
        {
			::SetWindowLong(_hWnd, GWL_EXSTYLE,        
							::GetWindowLong(_hWnd, GWL_EXSTYLE)^
							WS_EX_CLIENTEDGE);
        }

		::SetWindowPos(_hWnd, NULL, NULL, NULL, NULL, NULL,
					SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED);
		::RedrawWindow(_hWnd, NULL, NULL,
					RDW_INVALIDATE|RDW_NOCHILDREN|RDW_UPDATENOW|RDW_FRAME);
		

		_oldWndProc = 
			Gui::subclassControl<RichEdit>(_hWnd, this);
	}

	virtual void setText(const TCHAR * text)
	{
		::SetWindowText(_hWnd, text);
	}

	static std::wstring getOldProcId()
	{
		return L"RichEditOldProc";
	}

	bool processMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)                  /* handle the messages */
        {
            case WM_NCCALCSIZE:
            {
				if(wParam)
				{
					//Ask the control to first calculate the space it needs
					LRESULT nOriginalReturn =
						::CallWindowProc(_oldWndProc, _hWnd, message, wParam, lParam);

					//Alter the size for your own border, if necessary
					NCCALCSIZE_PARAMS *csparam = (NCCALCSIZE_PARAMS*)lParam;
					if(onNCCalcSize(csparam))
						return WVR_REDRAW;
					else
						return nOriginalReturn;
				}
			}
            case WM_NCPAINT:
            {
				//Let the control paint its own non-client elements (such as its scrollbars)
				LRESULT nOriginalReturn = ::CallWindowProc(_oldWndProc, _hWnd, message, wParam, lParam);

				//Draw the theme, if necessary
				if(onNCPaint())
					return 0;
				else
					return nOriginalReturn;
			}
		}
		return false;
	}

	bool initializeLibrary()
	{
		//Are we already initialised?
		if(pOpenThemeData && pCloseThemeData && pDrawThemeBackground && pGetThemeBackgroundContentRect &&
			pIsThemeActive && pDrawThemeParentBackground && pIsThemeBackgroundPartiallyTransparent)
			return true;

		//Try to get the function pointers of the UxTheme library
		if(!_hUxTheme)
		{
			_hUxTheme = LoadLibrary(_T("UxTheme.dll"));
			if(!_hUxTheme)
				return false;
		}

		pOpenThemeData = (HTHEME (WINAPI *)(HWND, LPCWSTR))GetProcAddress(_hUxTheme, "OpenThemeData");
		pCloseThemeData = (HRESULT (WINAPI *)(HTHEME))GetProcAddress(_hUxTheme, "CloseThemeData");
		pDrawThemeBackground = (HRESULT (WINAPI *)(HTHEME, HDC, int, int, const RECT*, const RECT *))GetProcAddress(_hUxTheme, "DrawThemeBackground");
		pGetThemeBackgroundContentRect = (HRESULT (WINAPI *)(HTHEME, HDC, int, int,  const RECT *, RECT *))GetProcAddress(_hUxTheme, "GetThemeBackgroundContentRect");
		pIsThemeActive = (BOOL (WINAPI *)())GetProcAddress(_hUxTheme, "IsThemeActive");
		pDrawThemeParentBackground = (HRESULT (WINAPI *)(HWND, HDC, RECT*))GetProcAddress(_hUxTheme, "DrawThemeParentBackground");
		pIsThemeBackgroundPartiallyTransparent = (BOOL (WINAPI *)(HTHEME, int, int))GetProcAddress(_hUxTheme, "IsThemeBackgroundPartiallyTransparent");

		if(pOpenThemeData && pCloseThemeData && pDrawThemeBackground && pGetThemeBackgroundContentRect &&
			pIsThemeActive && pDrawThemeParentBackground && pIsThemeBackgroundPartiallyTransparent)
			return true;
		else
			return false;
	}

	bool onNCCalcSize(NCCALCSIZE_PARAMS *csparam)
	{
		//Load the theme associated with edit boxes
		HTHEME hTheme = pOpenThemeData(_hWnd, L"edit");
		if(hTheme)
		{
			bool bToReturn = false;

			//Get the size required by the current theme to be
			//displayed properly
			RECT rcClient; ::ZeroMemory(&rcClient, sizeof(RECT));
			HDC hdc = GetDC(GetParent(_hWnd));
			if(pGetThemeBackgroundContentRect(hTheme, hdc,
											EP_EDITTEXT, ETS_NORMAL,
											&csparam->rgrc[0],
											&rcClient) == S_OK)
			{
				//Add a pixel to every edge so that the client area is
				//not too close to the border drawn by the theme (thus
				//simulating a native edit box)
				::InflateRect(&rcClient, -1, -1);
				//RECT rcClientPos;
				//::GetClientRect(_hWnd, &rcClientPos);
				
				_rcClientPos.left =
				rcClient.left-csparam->rgrc[0].left;
				_rcClientPos.top =
				rcClient.top-csparam->rgrc[0].top;
				_rcClientPos.right =
				csparam->rgrc[0].right-rcClient.right;
				_rcClientPos.bottom =
				csparam->rgrc[0].bottom-rcClient.bottom;

				memcpy(&csparam->rgrc[0], &rcClient, sizeof(RECT));
				bToReturn = true;
			}
			::ReleaseDC(GetParent(_hWnd), hdc);
			pCloseThemeData(hTheme);

			return bToReturn;
		}

		return false;
	}

	bool onNCPaint()
	{
		try
		{
			HTHEME hTheme = pOpenThemeData(_hWnd, L"edit");
			if(hTheme)
			{
				HDC hdc = ::GetWindowDC(_hWnd);

				//Clip the DC so that we only draw on the non-client area
				RECT rcBorder;
				::GetWindowRect(_hWnd, &rcBorder);
				rcBorder.right -= rcBorder.left; rcBorder.bottom -= rcBorder.top;
				rcBorder.left = rcBorder.top = 0;

				RECT rcClient; memcpy(&rcClient, &rcBorder, sizeof(RECT));
				rcClient.left += _rcClientPos.left;
				rcClient.top  += _rcClientPos.top;
				rcClient.right -= _rcClientPos.right;
				rcClient.bottom -= _rcClientPos.bottom;
				::ExcludeClipRect(hdc, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

				//Make sure the background is in a proper state
				if(pIsThemeBackgroundPartiallyTransparent(hTheme, EP_EDITTEXT, ETS_NORMAL))
					pDrawThemeParentBackground(_hWnd, hdc, &rcBorder);

				//Draw the border of the edit box
				int nState;
				if(!::IsWindowEnabled(_hWnd))
					nState = ETS_DISABLED;
				else if(::SendMessage(_hWnd, EM_GETOPTIONS, NULL, NULL) & ECO_READONLY)
					nState = ETS_READONLY;
				else
					nState = ETS_NORMAL;
				
				pDrawThemeBackground(hTheme, hdc, EP_EDITTEXT, nState, &rcBorder, NULL);
				pCloseThemeData(hTheme);

				::ReleaseDC(_hWnd, hdc);
				return true;
			}
		}
		catch(...)
		{
			//No exception is supposed to be thrown here but you can never be too safe
			//Trace an error here with your favorite TRACE macro
		}
		return false;
	}

};