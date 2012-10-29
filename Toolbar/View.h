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

#include "IModel.h"
#include "../ControlBase.h"
#include "../Modellable.h"

namespace Gui { namespace Toolbar 
{
	/**
	 *	Toolbar wrapper
	 */
	class View : public ControlBase,
		         public Modellable<IModel>
	{
    protected:
        ImageList _imageList;
    public:
        View():ControlBase() {}
		virtual void create(const HWND parentWnd)
		{
            createBase(
                TOOLBARCLASSNAME, 
                parentWnd, 
                WS_CHILD | WS_VISIBLE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS | TBSTYLE_LIST,
                L"",
                TBSTYLE_EX_MIXEDBUTTONS);

			::SendMessage(_hWnd, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
            ::SendMessage(_hWnd, TB_SETIMAGELIST    ,                        0, (LPARAM)(HIMAGELIST)_imageList);

            if(_model)
            {
                initializeFromModel();
            }
		}

        virtual void updateFromModel(void)
        {
            for(size_t index = 0; index < _model->getButtonsCount(); ++index)
            {
                enableButton(index, _model->getButton(index).isEnabled);

				if (_model->getButton(index).isTextNew == true)
				{
					setButtonText(index, _model->getButton(index).text);
				}
            }
        }

    protected:
        void enableButton(size_t index, bool isEnable)
        {
            if(_hWnd)
            {
                ::SendMessage(_hWnd, TB_ENABLEBUTTON, _model->getButton(index).command, isEnable?TRUE:FALSE);
            }
        }

		void setButtonText(size_t index, const std::wstring& text)
        {
            if(_hWnd)
            {
				TBBUTTONINFO buttonInfo;
				buttonInfo.cbSize = sizeof(TBBUTTONINFO);
				buttonInfo.dwMask = TBIF_TEXT;
				buttonInfo.idCommand = _model->getButton(index).command;

				size_t  length = text.length() + 1;
                wchar_t* buffer  = new wchar_t[length];
				memset(buffer, 0, length); 
				wcscpy(buffer, text.c_str());
				buttonInfo.pszText = buffer;		

                ::SendMessage(_hWnd, TB_SETBUTTONINFO, _model->getButton(index).command, (LPARAM)&buttonInfo);
				delete[] buffer;
            }
        }

        void initializeFromModel(void)
        {
            for(size_t index = 0; index < _model->getButtonsCount(); ++index)
			{
                Button button = _model->getButton(index);

                TBBUTTON b;
			    b.idCommand = button.command;
			    b.iBitmap   = 0;
                b.fsState   = button.isEnabled?TBSTATE_ENABLED:0;
			    b.fsStyle   = TBSTYLE_BUTTON | BTNS_AUTOSIZE; //TBSTYLE_SEP, TBSTYLE_CHECK, BTNS_SHOWTEXT

                if(button.isSeparator != TBSTYLE_SEP)
                {
    				b.iBitmap = _imageList.addImage(
                        ::LoadBitmap(::GetModuleHandle(0), MAKEINTRESOURCE(button.bitmapId))
                        );
                }
                else
                {
                    b.fsStyle  = TBSTYLE_SEP;
                }

                if(!button.text.empty())
                {
				    b.iString = ::SendMessage(_hWnd, TB_ADDSTRING, 0, (LPARAM)button.text.c_str());
                }
                else
                {
                    b.iString = -1;
                }
			    ::SendMessage(_hWnd, TB_ADDBUTTONS, 1, (LPARAM)&b);
			}
        }
	};
}};//namespace
