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

namespace Gui { namespace Toolbar
{

class Button
{
public:
	std::wstring text;
    int          bitmapId;
    int          command;
    bool         isEnabled;
    bool         isSeparator;

	//To update the button text during execution
	bool		 isTextNew;

	Button(const std::wstring & text_ = L"", int bitmapId_ = 0, int command_ = 0)
        :text(text_), bitmapId(bitmapId_), command(command_), isSeparator(false), isEnabled(true),
		isTextNew(false)
	{
	}
};

class Separator : public Button
{
public:
    Separator()
        :Button(L"", 0, 0)
    {
        isSeparator = true;
    }
};

class IModel
{
public:
	virtual void   addButton      (const Button & button)       = 0;
    virtual size_t getButtonsCount(void                 ) const = 0;
    virtual Button getButton      (size_t index         ) const = 0;
    virtual Button getButtonById  (int buttonId         ) const = 0;
};

}};//namespace