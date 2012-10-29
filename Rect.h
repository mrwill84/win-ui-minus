/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
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