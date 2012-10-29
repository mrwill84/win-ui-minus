/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

namespace Gui
{
	class IMessageProcessor
	{
	public:
		virtual bool processMessage  (UINT message, WPARAM wParam, LPARAM lParam) = 0;
        virtual LONG getMessageResult(void                                      ) = 0;
	};
};
