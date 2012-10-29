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

namespace Gui
{

class Rect
{
protected:
    RECT _rect;
public:
    Rect(int left = 0, int top = 0, int right = 0, int bottom = 0)
    {
        _rect.left   = left;
        _rect.top    = top;
        _rect.right  = right;
        _rect.bottom = bottom;
    }

    Rect(const RECT & rect)
        :_rect(rect)
    {
    }

    operator RECT() const
    {
        return _rect;
    }

    bool operator ==(const Rect & other) const
    {
        if((other._rect.top    == _rect.top   ) &&
           (other._rect.bottom == _rect.bottom) &&
           (other._rect.left   == _rect.left  ) &&
           (other._rect.right  == _rect.right ))
        {
            return true;
        }
        return false;
    }

    static Rect Null()
    {
        static Rect null;
        return null;
    }
};
};//namespace