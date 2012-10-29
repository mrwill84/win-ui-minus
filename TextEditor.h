/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */
 
#pragma once

#include "ControlBase.h"
#include "Graphics.h"
#include "WinUtils.h"
#include <tchar.h>

namespace Gui
{

	/**
	 *	Text Editor custom control 
	 */
	class TextEditor : public ControlBase
	{
	public:
		class IDocument
		{
		public:
			virtual int getLine(int lineNo, TCHAR * buffer, int bufferSize) = 0;
			virtual int getLineCount(void) = 0;
			virtual int getLongestLineSize(void) = 0;
			virtual ~IDocument() {};
		};
	private:
		IDocument * _document;
		int         _lineCount;
		int         _horizontalScrollPos;
		int         _verticalScrollPos;
		int         _horizontalScrollMax;
		int         _verticalScrollMax;
		int         _longestLine;
		int         _windowColumns;
		int         _windowLines;
	public:
        TextEditor()
			:ControlBase(),
			 _horizontalScrollPos(0),
			 _verticalScrollPos(0),
			 _document(0)
        {
			Gui::WinRegisterClass<TextEditor>("TextEditor32");
        }

		void create(HWND parentWindow)
		{
			_hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, 
								   _T("TextEditor32"), _T(""), 
								   WS_VSCROLL |WS_HSCROLL | WS_CHILD | WS_VISIBLE,
		                           0, 0, 0, 0, 
		                           parentWindow, 
		                           0, 
		                           GetModuleHandle(0), 
		                           this);
		}

		void setDocument(IDocument * document)
		{
			_document = document;
		}

		virtual void paint(void)
		{
			Graphics graphics(_hWnd);
			Font font(DEFAULT_GUI_FONT);
			graphics.setFont(font);
			
			

			// figure out which lines to redraw
			unsigned long first = graphics.getTop()    / graphics.getFontHeight();
			unsigned long last  = graphics.getBottom() / graphics.getFontHeight();	
			// set the colours

			graphics.setTextColor(Colors::WINDOWTEXT);
			graphics.setBkColor(Colors::WINDOW);
			// draw the display line-by-line
			for(int i = first; i <= last; i++)
			{
				paintLine(graphics, i);
			}
		}

		void paintLine(Graphics & graphics, int lineNo)
		{
			RECT  rect;
			TCHAR buffer[4096];
			
			// Get the area we want to update
			GetClientRect(getHandle(), &rect);

			// calculate rectangle for entire length of line in window
			rect.left       = (long)(- _horizontalScrollPos * graphics.getFontHeight());
			rect.top		= (long)((lineNo - _verticalScrollPos) * graphics.getFontHeight());
			rect.right		= (long)(rect.right);
			rect.bottom		= (long)(rect.top + graphics.getFontHeight());

			// check we have data to draw on this line
			if(lineNo >= _lineCount)
			{
				//SetBkColor(hdc, GetColour(TXC_BACKGROUND));
				ExtTextOut(graphics, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
				return;
			}

			// get the data for this single line of text
			int len = _document->getLine(lineNo, buffer, 4096);
			
			// draw text and fill line background at the same time
			TabbedExtTextOut(graphics, &rect, buffer, len);
		}

		void TabbedExtTextOut(Graphics & graphics, RECT *rect, TCHAR *buf, int len)
		{
			int  tab = 4 * 7;
			int  width;
			RECT fill = *rect;

			// Draw line and expand tabs
			width = TabbedTextOut(graphics, rect->left, rect->top, buf, len, 1, &tab, rect->left);
			
			// Erase the rest of the line with the background colour
			fill.left += LOWORD(width);
			ExtTextOut(graphics, 0, 0, ETO_OPAQUE, &fill, 0, 0, 0);
		}

		virtual void onCommand(WPARAM wParam, LPARAM lParam)
		{
		}
		
		virtual void onCreate(WPARAM wParam, LPARAM lParam)
		{
		}
		
		virtual void onSize(unsigned long x, unsigned long y)
		{
			_windowLines   = min((unsigned)y / 10, _lineCount);
			_windowColumns = min(x  / 10,  _longestLine);

			//refresh();
			setupScrollbars();
		}

		virtual void onPaint(WPARAM wParam, LPARAM lParam)
		{
			paint();
		}

		virtual void onVScroll(UINT sBCode, UINT pos)
		{
			ULONG oldpos = _verticalScrollPos;

			switch(sBCode)
			{
			case SB_TOP:
				_verticalScrollPos = 0;
				refresh();
				break;

			case SB_BOTTOM:
				_verticalScrollPos = _verticalScrollMax;
				refresh();
				break;

			case SB_LINEUP:
				Scroll(0, -1);
				break;

			case SB_LINEDOWN:
				Scroll(0, 1);
				break;

			case SB_PAGEDOWN:
				Scroll(0, _windowLines);
				break;

			case SB_PAGEUP:
				Scroll(0, -_windowLines);
				break;

			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:

				_verticalScrollPos = GetTrackPos32(_hWnd, SB_VERT);
				refresh();
				break;
			}

			if(oldpos != _verticalScrollPos)
			{
				setupScrollbars();
			}
		}

		virtual void onHScroll(UINT sBCode, UINT pos)
		{
			int oldpos = _horizontalScrollPos;

			switch(sBCode)
			{
			case SB_LEFT:
				_horizontalScrollPos = 0;
				refresh();
				break;

			case SB_RIGHT:
				_horizontalScrollPos = _horizontalScrollMax;
				refresh();
				break;

			case SB_LINELEFT:
				Scroll(-1, 0);
				break;

			case SB_LINERIGHT:
				Scroll(1, 0);
				break;

			case SB_PAGELEFT:
				Scroll(-_windowColumns, 0);
				break;

			case SB_PAGERIGHT:
				Scroll(_windowColumns, 0);
				break;

			case SB_THUMBPOSITION:
			case SB_THUMBTRACK:

				_horizontalScrollPos = GetTrackPos32(_hWnd, SB_HORZ);
				refresh();
				break;
			}

			if(oldpos != _horizontalScrollPos)
			{
				setupScrollbars();
			}
		}

		virtual void onMouseWheel(int delta)
		{
			int nScrollLines;

			SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &nScrollLines, 0);

			if(nScrollLines <= 1)
				nScrollLines = 3;

			Scroll(0, (-delta/120) * nScrollLines);
		}

		void setupScrollbars()
		{
			if(_document)
			{
				_lineCount   = _document->getLineCount();
				_longestLine = _document->getLongestLineSize();
			}

			SCROLLINFO si = { sizeof(si) };

			si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_DISABLENOSCROLL;

			//
			//	Vertical scrollbar
			//
			si.nPos  = _verticalScrollPos;		// scrollbar thumb position
			si.nPage = _windowLines;		// number of lines in a page
			si.nMin  = 0;					
			si.nMax  = _lineCount - 1;	// total number of lines in file
			
			SetScrollInfo(_hWnd, SB_VERT, &si, TRUE);

			//
			//	Horizontal scrollbar
			//
			si.nPos  = _horizontalScrollPos;		// scrollbar thumb position
			si.nPage = _windowColumns;	// number of lines in a page
			si.nMin  = 0;
			si.nMax  = _longestLine - 1;	// 

			SetScrollInfo(_hWnd, SB_HORZ, &si, TRUE);

			// adjust our interpretation of the max scrollbar range to make
			// range-checking easier. The scrollbars don't use these values, they
			// are for our own use.
			_verticalScrollMax = _lineCount   - _windowLines;
			_horizontalScrollMax = _longestLine - _windowColumns;
		}

		void Scroll(int dx, int dy)
		{
			//
			// make sure that dx,dy don't scroll us past the edge of the document!
			//

			// scroll up
			if(dy < 0)
			{
				dy = -(int)min((ULONG)-dy, _verticalScrollPos);
			}
			// scroll down
			else if(dy > 0)
			{
				dy = min((ULONG)dy, _verticalScrollMax - _verticalScrollPos);
			}


			// scroll left
			if(dx < 0)
			{
				dx = -(int)min(-dx, _horizontalScrollPos);
			}
			// scroll right
			else if(dx > 0)
			{
				dx = min((unsigned)dx, (unsigned)_horizontalScrollMax - _horizontalScrollPos);
			}

			// adjust the scrollbar thumb position
			_horizontalScrollPos += dx;
			_verticalScrollPos += dy;

			// perform the scroll
			if(dx != 0 || dy != 0)
			{
				ScrollWindowEx(
					_hWnd, 
					-dx * 12, 
					-dy * 12,
					NULL,
					NULL,
					0, 0, SW_INVALIDATE
					);

				setupScrollbars();
			}
		}

		LONG GetTrackPos32(HWND hwnd, int nBar)
		{
			SCROLLINFO si = { sizeof(si), SIF_TRACKPOS };
			GetScrollInfo(hwnd, nBar, &si);
			return si.nTrackPos;
		}

		virtual bool processMessage(UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)                  /* handle the messages */
			{
				case WM_MOUSEWHEEL:
				{
					onMouseWheel((short)HIWORD(wParam));
					return false;
				}
				case WM_VSCROLL:
				{
					onVScroll(LOWORD(wParam), HIWORD(wParam));
					return false;
				}
				case WM_HSCROLL:
				{
					onHScroll(LOWORD(wParam), HIWORD(wParam));
					return false;
				}
				case WM_SETFOCUS:
				{
					onSetFocus((HWND)wParam);
					return false;
				}
				case WM_ERASEBKGND:
				{
					return false;
				}
			}
			return true;
		}

		virtual void onSetFocus(HWND oldWnd)
		{
			CreateCaret(_hWnd, (HBITMAP)NULL, 2, 12/*m_nLineHeight*/);
			RepositionCaret();

			ShowCaret(_hWnd);
			refresh();
		}

		void RepositionCaret()
		{
			ULONG lineno, charoff;
			ULONG offset = 0;
			int   xpos   = 0;
			int   ypos   = 0;
			
			//TCHAR buf[TEXTBUFSIZE];

			//ULONG nOffset = m_nCursorOffset;

			// make sure we are using the right font
			//HDC hdc = GetDC(m_hWnd);
			//SelectObject(hdc, m_FontAttr[0].hFont);

			// get line number from cursor-offset
			//if(!m_pTextDoc->offset_to_line(nOffset, &lineno, &charoff))
			//	return 0;

			// y-coordinate from line-number
			//ypos = (lineno - m_nVScrollPos) * m_nLineHeight;

			// now find the x-coordinate on the specified line
			//while(offset < charoff)
			//{
			//	int tlen = min((charoff - offset), TEXTBUFSIZE);
			//	tlen     = m_pTextDoc->getdata(nOffset - charoff + offset, buf, tlen);

			//	offset += tlen;
			//	xpos   += NeatTextWidth(hdc, buf, tlen, -xpos);
			//}
			
			//ReleaseDC(m_hWnd, hdc);

			// take horizontal scrollbar into account
			//xpos -= m_nHScrollPos * m_nFontWidth;

			SetCaretPos(20 /*xpos*/, 20/*ypos*/);
		}
	};
};