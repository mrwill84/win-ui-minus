/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once
//#define _WIN32_WINNT 0x400
#include <windows.h>
#include <string>
#include <commdlg.h>
#include "IMessageProcessor.h"

namespace Gui
{
    template<class T>
    BOOL CALLBACK DialogProcedure (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        T * messageProcessor = reinterpret_cast<T*>(::GetProp(hWnd, L"Gui::IMessageProcessor"));

        switch (message)
        {
            case WM_INITDIALOG:
            {
				messageProcessor = reinterpret_cast<T*>(lParam);
                ::SetProp(hWnd, L"Gui::IMessageProcessor", reinterpret_cast<HANDLE>(messageProcessor));
				messageProcessor->setWnd(hWnd);
				messageProcessor->onInitDialog(wParam, lParam);
				return FALSE;
            }
            case WM_DESTROY:
            {
                ::EndDialog(hWnd, 0);
                return TRUE;
			}
            case WM_CLOSE:
            {
				messageProcessor->onClose();
                ::EndDialog(hWnd, 0);
                return TRUE;
			}
			default:
			{
				if(messageProcessor)
				{
					messageProcessor->processMessage(message, wParam, lParam);
				}
			}
        }
        return 0;
    };

	inline void registerClass(const std::wstring & className)
	{
		WNDCLASSEX	wcx;

		wcx.cbSize			= sizeof(wcx);
		wcx.style			= 0;
		wcx.lpfnWndProc		= DefWindowProc;
		wcx.cbClsExtra		= 0;
		wcx.cbWndExtra		= 0;
		wcx.hInstance		= ::GetModuleHandle(0);
		wcx.hIcon			= 0;
		wcx.hCursor			= ::LoadCursor(0, IDC_ARROW);
		wcx.hbrBackground	= (HBRUSH) 0;	//NO FLICKERING
		wcx.lpszMenuName	= 0;
		wcx.lpszClassName	= className.c_str();	
		wcx.hIconSm			= 0;

		BOOL status = ::RegisterClassEx(&wcx);
        if(status == 0)
        {
            DWORD error = ::GetLastError();
            error = error;//test
        }
	};

	bool showOpenFileDlg(HWND hwnd, TCHAR *pstrFileName, TCHAR *pstrTitleName)
	{
		TCHAR *szFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0";
		
		OPENFILENAME ofn	= { sizeof(ofn) };

		ofn.hwndOwner		= hwnd;
        ofn.hInstance		= ::GetModuleHandle(0);
		ofn.lpstrFilter		= szFilter;
		ofn.lpstrFile		= pstrFileName;
		ofn.lpstrFileTitle	= pstrTitleName;
		
		ofn.nFilterIndex	= 1;
		ofn.nMaxFile		= _MAX_PATH;
		ofn.nMaxFileTitle	= _MAX_FNAME + _MAX_EXT; 

		// flags to control appearance of open-file dialog
		ofn.Flags			=	OFN_EXPLORER			| 
								OFN_ENABLESIZING		|
								OFN_ALLOWMULTISELECT	| 
								OFN_FILEMUSTEXIST;

		return ::GetOpenFileName(&ofn)?true:false;
	}

	typedef void (*myType)(void);

#pragma warning( push )
#pragma warning( disable : 4312 4311)
	template <typename T>
    LRESULT CALLBACK controlProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
		WNDPROC superProc  = (WNDPROC)(::GetProp(hWnd, L"OldProc"));
        IMessageProcessor * messageProcessor = 
            reinterpret_cast<IMessageProcessor*>(::GetProp(hWnd, L"Gui::IMessageProcessor"));

        LRESULT result = 1;
		if(!messageProcessor)
		{
			return ::CallWindowProc(superProc, hWnd, message, wParam, lParam);
		}
        if(!messageProcessor->processMessage(message, wParam, lParam))
        {
			result = ::CallWindowProc(superProc, hWnd, message, wParam, lParam);
        }
        else if(messageProcessor->getMessageResult())
        {
            result = messageProcessor->getMessageResult();
        }

        return result;
    };
#pragma warning( pop ) 
}// namespace
