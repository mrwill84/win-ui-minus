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

#include "ControlBase.h"

namespace Gui
{

class TransparentWindow : public ControlBase
{
public:
    class DefaultController : public Controller<TransparentWindow>
    {
    public:
        virtual bool onPaint(WPARAM wParam, LPARAM lParam)
        {
            _view->paint();
            return false;
        }
    };
private:
    DefaultController _defaultController;
public:
    TransparentWindow()
    {
        _defaultController.setView(this);
        _controller = &_defaultController;
        Gui::registerClass(L"Gui::TransparentWindow");
    }

    virtual void create(IView * parentView)
    {
        createBase(
            L"Gui::TransparentWindow", 
            parentView, 
            WS_CHILD|WS_VISIBLE|WS_POPUP,
            L"",
            WS_EX_TOOLWINDOW|WS_EX_CONTROLPARENT|WS_EX_LAYERED/*|WS_EX_TOPMOST*/);
        
        // Set WS_EX_LAYERED on this window 
        //SetWindowLong(hwnd, GWL_EXSTYLE,
        //    GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(_hWnd, 0, (255 * 50) / 100, LWA_ALPHA);
    }

    virtual void paint(void)
    {
        Graphics graphics(_hWnd);
        graphics.setBkColor(Color(179, 217, 255));
        graphics.fillRect(getClientRect());
    }
};

};//namespace