/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include "IView.h"

namespace Gui
{
	class ILayout
    {
    public:
		virtual void add           (IView * view                           ) = 0;
		virtual void setResize     (const size_t width, const size_t height) = 0;
		virtual void createControls(const HWND parentWindow                ) = 0;
		virtual void clean         (void                                   ) = 0;
		virtual ~ILayout() {};
    };
};