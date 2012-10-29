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
#include "../ListPaneIModel.h"

namespace Gui { namespace Properties 
{

class IButtonHandler
{
public:
    virtual void onButtonPressed() = 0;
};

class Property
{
public:
    std::wstring           name;
    std::wstring           value;
    ListPane::IModel::Item listItem;
    bool                   isButton;

    Property(const std::wstring & name_ = L"", const std::wstring & value_ = L"")
        :name(name_), value(value_), isButton(false) {}
};

class IPropertiesModel : public ListPane::IModel
{
public:
    virtual void     addProperty       (const Property & property               )       = 0;
    virtual Property getProperty       (size_t index                            ) const = 0;
    virtual size_t   getPropertiesCount(                                        ) const = 0;
    virtual void     clear             (                                        )       = 0;
    virtual void     setValue          (size_t index, const std::wstring & value)       = 0;
    virtual size_t   getSelection      (                                        ) const = 0;
    virtual ~IPropertiesModel() {};
};

}};//namespace