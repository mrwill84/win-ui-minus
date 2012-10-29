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
 */#pragma once

#include "ControlBase.h"
#include "WinUtils.h"

namespace Gui
{
	class ITableModel
	{
	public:
		virtual void getRowCount(void) = 0;
		virtual void getColumnCount(void) = 0;
		//virtual void getText(int row, int column) = 0;
		virtual int  getRowHeight(int row) = 0;
		virtual Font getFont(void) = 0;
		virtual IControl * getControl(int row, int column) = 0;
	};

	class Table : public ControlBase,
		          public Gui::MessageProcessor<Table>
	{
	protected:
		ITableModel * tableModel;
		Button      * _testButton;
		Label       * _testLabel;
		Edit        * _testEdit;
	public:
		Table()
			:ControlBase(),
			_testButton(0)
		{
			Gui::WinRegisterClass<Table>(L"Gui::Table");
		}
	
		void create(HWND parentWindow)
		{
			_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, 
								   _T("Gui::Table"), _T(""), 
								   WS_VSCROLL |WS_HSCROLL | WS_CHILD | WS_VISIBLE,
		                           0, 0, 0, 0, 
		                           parentWindow, 
		                           0, 
		                           GetModuleHandle(0), 
		                           this);
			//_testButton = new Button("Test");
			//_testLabel  = new Label("Test label");
			//_testEdit   = new Edit("Test edit");
		}

		virtual bool onPaint(WPARAM wParam, LPARAM lParam)
		{
			paint();
		}

		virtual void paint(void)
		{
			Graphics graphics(_hWnd);
			Font font(DEFAULT_GUI_FONT);
			graphics.setFont(font);
			
			RECT  rect;
			// Get the area we want to update
			GetClientRect(getHandle(), &rect);

			graphics.setBkColor(Colors::WINDOW);
			ExtTextOut(graphics, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);

			//for(int i = 0; i < 15; i++)
			//{
			//	graphics.line(0, i*14, rect.right, i*14);
			//}
		}
		
		virtual bool onSize(unsigned long x, unsigned long y)
		{
			if(_testButton)
			{
				_testLabel->setPosition(10, 20);
				_testLabel->setSize(50, 20);
				_testEdit->setPosition(70, 20);
				_testEdit->setSize(30, 20);
				_testButton->setPosition(140, 20);
				_testButton->setSize(50, 20);
			}
		}
	};
}// namespace