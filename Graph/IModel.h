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

#include <string>

#include "../Point.h"

namespace Gui { namespace Graph {

class IModel
{
public:
    struct Node
    {
        Point        point;
        HBITMAP      bitmap;
        bool         isSelected;
        std::wstring name;
        size_t       id;
        Node()
            :point(0, 0), bitmap(0),isSelected(false), name(L""), id(0)
        {
        }
		Node(const int x, const int y, const HBITMAP & bitmap_, const std::wstring & name_ = L"")
            :point(x, y), bitmap(bitmap_), name(name_), isSelected(false){}

        bool hasPoint(const POINT & point_) const
        {
            if((point_.x > (point.x - 5)) && (point_.x < (point.x + 47)) &&
               (point_.y > (point.y - 5)) && (point_.y < (point.y + 47)))
            {
                return true;
            }
            return false;
        }
    };

    struct Link
    {
        size_t source;
        size_t target;
        int    color;
        size_t width;
        size_t id;
        Link():source(0), target(0), color(0), width(1), id(0) {}
    };

    virtual size_t getNodesCount(void              ) const = 0;
    virtual Node   getNode      (const size_t index) const = 0;
    virtual void   setNode      (const size_t index, 
                                 const Node & node )       = 0;
    virtual void   addNode      (const Node & node )       = 0;
    virtual size_t getLinksCount(void              ) const = 0;
    virtual Link   getLink      (const size_t index) const = 0;
    virtual void   selectNode   (const size_t index)       = 0;
    virtual void   shiftSelectedNodeUp   (void     )       = 0;
    virtual void   shiftSelectedNodeDown (void     )       = 0;
    virtual void   shiftSelectedNodeLeft (void     )       = 0;
    virtual void   shiftSelectedNodeRight(void     )       = 0;
};

}};//namespace