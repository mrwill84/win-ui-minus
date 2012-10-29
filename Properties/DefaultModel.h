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
#include <vector>

namespace Gui { namespace Properties
{

class DefaultModel : public IPropertiesModel, 
                     public std::vector<Property>
{
public:
    virtual void addProperty(const Property & property)
    {
        push_back(property);
    }
	
    virtual Property getProperty(size_t index) const
    {
		return operator[](index);
    }

    virtual size_t getPropertiesCount() const
    {
        return size();
    }

    virtual void clear()
    {
        std::vector<Property>::clear();
    }

    virtual void setValue(size_t index, const std::wstring & value)
    {
    }
    
    virtual size_t getSelection() const
    {
        size_t selection = (size_t)(-1);
        for(size_t index_ = 0; index_ < size(); ++index_)
        {
            Property property = at(index_);
            if (property.listItem.isSelected)
            {
                selection = index_;
                break;
            }
        }
        return selection;
    }

	virtual size_t getItemsCount(void) const
	{
		return size();
	}

	virtual Item getItem(size_t index) const
	{
		return at(index).listItem;
	}

	virtual void recalculateItems(void)
	{
		for(size_t index = 0; index < size(); ++index)
		{
			Property property = at(index);
			property.listItem.top = static_cast<int>(index) * 16;
			property.listItem.bottom = property.listItem.top + 16;
			operator[](index) = property;
		}
	}

	virtual void setSelection(size_t index)
	{
        for(size_t index_ = 0; index_ < size(); ++index_)
        {
            Property property = at(index_);
            if(index == index_)
            {
                property.listItem.isSelected = true;
            }
            else
            {
                property.listItem.isSelected = false;
            }
            operator[](index_) = property;
        }
	}
};

}};//namespace