/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include "../ControlBase.h"

namespace Gui
{
	class TextLabel : public ControlBase
	{
	public:
        TextLabel(const std::wstring & text = L""):ControlBase() {setText(text);}

		virtual void create(IView * parentView)
		{
			createBase(WC_STATIC, parentView, WS_CHILD|WS_VISIBLE|SS_LEFT);
		}
	};

}