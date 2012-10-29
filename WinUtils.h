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
