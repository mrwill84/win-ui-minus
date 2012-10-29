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

#include "../ControlBase.h"
#include "../IEventHandler.h"

namespace Gui { namespace Splitbar
{
class Event
{
public:
    std::wstring id;
    size_t       index;
    Event(const std::wstring & id_, const size_t & index_)
        :id(id_), index(index_){}
};

class View : public ControlBase,
		     public Controller<View>

{
protected:
	HCURSOR                _sizeCursor;
	HCURSOR                _verticalCursor;
	HCURSOR                _horizontalCursor;
    Point                  _lastMousePos;
    Gui::IHandler<Event> * _eventHandler;
    size_t                 _index;

public:
	View(bool isVertical, const size_t & index = 0)
        :_index(index)
	{
		Gui::registerClass(L"Gui::Splitbar::View");
		_horizontalCursor = ::LoadCursor(NULL, IDC_SIZENS);
		_verticalCursor = ::LoadCursor(NULL, IDC_SIZEWE);
		setVertical(isVertical);
	}

    void setIndex(const size_t index)
    {
        _index = index;
    }

	virtual void setVertical(bool isVertical)
	{
		if(isVertical)
		{
			_sizeCursor = _verticalCursor;
		}
		else
		{
			_sizeCursor = _horizontalCursor;
		}
	}

	virtual void create(const HWND parentWindow)
	{
		createBase(L"Gui::Splitbar::View", parentWindow);
		setView(this);
    }
	virtual bool onPaint(WPARAM wParam, LPARAM lParam)
    { 
		Graphics graphics(_hWnd);
        graphics.setBkColor(Colors::DIALOG);
		graphics.fillRect(getClientRect());
        return true;
    }
	
    virtual bool onMouseMove(int x, int y, WPARAM wParam)
	{
		if((wParam & MK_LBUTTON) && (::GetCapture() == _hWnd))
		{
            Point mousePos(x, y);
            if(mousePos != _lastMousePos)
            {
                _eventHandler->handle(Event(L"SplitBarMove", _index));
                _lastMousePos = mousePos;
            }
		}
		else
		{
			::SetCursor(_sizeCursor);
		}
		return true;
	}
	virtual bool onLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		::SetCapture(_hWnd);
		::SetCursor(_sizeCursor);
        RECT windowRect;
		GetWindowRect(::GetParent(_hWnd), &windowRect);

        ::ClipCursor(&windowRect);
		return true;
	}
		
	virtual bool onLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		::ReleaseCapture();
        ::ClipCursor(0);
		return true;
	}

    void setEventHandler(Gui::IHandler<Event> * handler)
    {
        _eventHandler = handler;
    }
};
}};//namespace