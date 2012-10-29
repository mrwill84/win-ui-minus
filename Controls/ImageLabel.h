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
	class ImageLabel : public ControlBase
	{
	private:
		HBITMAP _hBitmap;
		int _imageId;
	public:
        ImageLabel(const int imageId):ControlBase(), _imageId(imageId) {}

		virtual void create(const HWND parentWindow)
		{
			_hBitmap = (HBITMAP)::LoadImage(::GetModuleHandle(NULL), MAKEINTRESOURCE(_imageId), IMAGE_BITMAP, 100, 100, LR_DEFAULTCOLOR);
			createBase(WC_STATIC, parentWindow, WS_CHILD|WS_VISIBLE|SS_BITMAP);
			::SendMessage(_hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)_hBitmap);
		}
	};
}