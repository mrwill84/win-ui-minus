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

namespace Gui { namespace ListPane
{

    class IModel
    {
    public:
        class Item
        {
        public:
            int  top;
            int  bottom;
            bool isSelected;
            Item()
                :top(0), bottom(0), isSelected(false)
            {
            }

            Item(const Item & other)
                :top(other.top), bottom(other.bottom), isSelected(other.isSelected)
            {
            }
        };

        virtual size_t getItemsCount   (void        ) const = 0;
        virtual Item   getItem         (size_t index) const = 0;
        virtual void   recalculateItems(void        )       = 0;
        virtual void   setSelection    (size_t index)       = 0;
        virtual size_t getSelection    (void        ) const = 0;

        virtual ~IModel() {}
    };
}};//namespace