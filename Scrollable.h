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

namespace Gui
{
class Scrollable
{
protected:
    int _verticalScrollPos;
    int _horizontalScrollPos;

public:
    Scrollable()
        :_verticalScrollPos(0)
    {
    }
    virtual HWND getScrollableWnd      (             ) const = 0;
    virtual int  getVirtualHeight      (             ) const = 0;
    virtual int  getVirtualWidth       (             ) const = 0;
    virtual void handleVerticalScroll  (int scrollPos) {}
    virtual void handleHorizontalScroll(int scrollPos) {}

    virtual void setupScrollbars(void)
    {
        setupVerticalScrollbar();
        setupHorizontalScrollbar();
    }

    virtual void setVerticalScrollPos(int scrollPos)
    {
        if(_verticalScrollPos != scrollPos)
        {
            RECT rect;
            ::GetClientRect(getScrollableWnd(), &rect);
            rect.top    += 1;
            rect.bottom -= 1;

            ::ScrollWindowEx(
                getScrollableWnd(), 
                0, 
                _verticalScrollPos - scrollPos, 
                &rect, 
                &rect, 
                0, 0, 
                SW_INVALIDATE | SW_SCROLLCHILDREN /*| SW_SMOOTHSCROLL*/);
            _verticalScrollPos = scrollPos;

            handleVerticalScroll(scrollPos);
            setupVerticalScrollbar();
        }
    }

    virtual void setHorizontalScrollPos(int scrollPos)
    {
        if(_verticalScrollPos != scrollPos)
        {
            RECT clip;
            ::GetClientRect(getScrollableWnd(), &clip);
            Rect update = clip;
            clip.left  += 1;
            clip.right -= 1;

            ::ScrollWindowEx(
                getScrollableWnd(), 
                _horizontalScrollPos - scrollPos, 
                0, 
                &clip, 
                &clip, 
                0, 0, 
                SW_INVALIDATE | SW_SCROLLCHILDREN /*| SW_SMOOTHSCROLL*/);
            _horizontalScrollPos = scrollPos;

            handleHorizontalScroll(scrollPos);
            //update = update - clip;
            ::InvalidateRect(getScrollableWnd(), &clip, TRUE);
            setupHorizontalScrollbar();
        }
    }

    void scrollUp()
    {
        if((_verticalScrollPos + 3) < getVirtualHeight())
        {
            setVerticalScrollPos(_verticalScrollPos + 3);
        }
    }

    void scrollDown()
    {
        if((_verticalScrollPos - 3) > 0)
        {
            setVerticalScrollPos(_verticalScrollPos - 3);
        }
    }

    virtual void setupVerticalScrollbar(void)
    {
        SCROLLINFO si = { sizeof(si) };
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        
        RECT  clientRect = {0};
        ::GetClientRect(getScrollableWnd(), &clientRect);
        
        int height = (clientRect.bottom - clientRect.top );
        si.nPage = height;
        if((si.nPage >= static_cast<unsigned int>(getVirtualHeight())) && (_verticalScrollPos != 0))
        {
            _verticalScrollPos = 0;
            ::RedrawWindow(getScrollableWnd(), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
        si.nPos = _verticalScrollPos;
        si.nMin  =  0;
        si.nMax  = getVirtualHeight() - 1;

        ::SetScrollInfo(getScrollableWnd(), SB_VERT, &si, TRUE);
        
        RECT parentRect;
        ::GetClientRect(::GetParent(getScrollableWnd()), &parentRect);
        parentRect.left = parentRect.right - 20;
        ::RedrawWindow(::GetParent(getScrollableWnd()), &parentRect, 0, 
                       RDW_INVALIDATE | RDW_ALLCHILDREN);
    }

    virtual void setupHorizontalScrollbar(void)
    {
        SCROLLINFO si = { sizeof(si) };
        si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
        
        RECT  clientRect = {0};
        ::GetClientRect(getScrollableWnd(), &clientRect);
        
        int width  = (clientRect.right  - clientRect.left);
        si.nPage = width;
        if((si.nPage >= static_cast<unsigned int>(getVirtualWidth())) && (_horizontalScrollPos != 0))
        {
            _horizontalScrollPos = 0;
            ::RedrawWindow(getScrollableWnd(), 0, 0, RDW_INVALIDATE | RDW_ALLCHILDREN);
        }
        si.nPos = _horizontalScrollPos;
        si.nMin  =  0;
        si.nMax  = getVirtualWidth() - 1;

        ::SetScrollInfo(getScrollableWnd(), SB_HORZ, &si, TRUE);
        
        RECT parentRect;
        ::GetClientRect(::GetParent(getScrollableWnd()), &parentRect);
        parentRect.top = parentRect.bottom - 20;
        ::RedrawWindow(::GetParent(getScrollableWnd()), &parentRect, 0, 
                       RDW_INVALIDATE | RDW_ALLCHILDREN);    
    }
};
};//namespace