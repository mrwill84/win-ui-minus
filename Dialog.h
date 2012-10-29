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

#include "WinUtils.h"
#include "MessageProcessor.h"

namespace Gui
{
	class Dialog : public MessageProcessor,
                   public ICommandListener
	{
	protected:
		HWND         _hWnd;
		int          _resourceId;
        std::wstring _title;
	public:
        Dialog(int dialogResourceId, const std::wstring & title = L"")
			:_resourceId(dialogResourceId), _title(title)
		{
            setCommandListener(this);
		}

		virtual void create(HWND parentWnd)
		{
            if(_resourceId != 0)
            {
			    _hWnd = ::CreateDialogParam(::GetModuleHandle(0), 
				                    MAKEINTRESOURCE(_resourceId), 
								    parentWnd, 
								    Gui::DialogProcedure<Dialog>,
								    (LPARAM)this);
            }
            else
            {
                //DLGTEMPLATE * dlgTemplate = new DLGTEMPLATE();
                //dlgTemplate->dwExtendedStyle = WS_EX_CONTROLPARENT;
                //dlgTemplate->style           = DS_SETFONT|DS_FIXEDSYS|DS_CONTROL|WS_CHILD;
                //dlgTemplate->x               =   5;
                //dlgTemplate->y               =   5;
                //dlgTemplate->cx              = 263;
                //dlgTemplate->cy              = 147;
                //dlgTemplate->cdit            =   0;
                //_hWnd = ::CreateDialogIndirectParam(
                //    ::GetModuleHandle(0), 
                //    dlgTemplate, 
                //    parentWnd, 
                //    Gui::DialogProcedure<Dialog>, 
                //    (LPARAM)this);
            }
		}

		void setWnd(HWND hWnd)
		{
			_hWnd = hWnd;
            setTitle(_title);
		}

		virtual INT_PTR showModal(HWND parentWnd)
		{
			return ::DialogBoxParam(::GetModuleHandle(0), 
				                   MAKEINTRESOURCE(_resourceId), 
								   parentWnd, 
								   Gui::DialogProcedure<Dialog>,
								   (LPARAM)this);
		}

		virtual void show(void)
		{
			::ShowWindow(_hWnd, 1);
			::UpdateWindow(_hWnd);

			MSG message;
			while(::GetMessage(&message, 0, 0, 0) != 0)
			{
				if(!::IsDialogMessage(_hWnd, &message))
				{
					::TranslateMessage(&message);
					::DispatchMessage(&message);
				}
			}
		}
		
        virtual void setTitle(const std::wstring & title)
        {
            _title = title;
            if(_hWnd)
            {
                ::SetWindowText(_hWnd, _title.c_str());
            }
        }

        HWND getDlgItem(int id)
        {
            return ::GetDlgItem(_hWnd, id);
        }

        virtual void refresh(void)
        {
            ::RedrawWindow(_hWnd, 0, 0, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASENOW|RDW_ERASE);
        }

		virtual bool onCommand(const Command & command)
		{
			if(command.id == IDOK)
			{
				onOk();
				::EndDialog(_hWnd, command.id);
			}
			if(command.id == IDCANCEL)
			{
				onCancel();
				::EndDialog(_hWnd, command.id);
			}
			return true;
		}

		virtual void onInitDialog(WPARAM wParam, LPARAM lParam){}
		virtual void onOk    (void){}
		virtual void onClose (void){}
		virtual void onCancel(void){}
	};
};// namespace