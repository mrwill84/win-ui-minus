/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */


#pragma once

#include <windows.h>
#include <string>
#include "Point.h"
#include "Rect.h"

namespace Gui
{
    const int WM_ASYNC_UPDATE = WM_APP + 1;

	class Dimension
	{
	public:
		size_t width;
		size_t height;

		Dimension(const size_t width_ = 0, const size_t height_ = 0)
			:width(width_), height(height_) {}

        Dimension operator +(const Dimension & other)
        {
            width  += other.width;
            height += other.height;
            return *this;
        }

		bool operator ==(const Dimension & other) const
		{
			return (width == other.width) && (height == other.height);
		}

		bool operator !=(const Dimension & other) const
		{
			return !operator==(other);
		}
	};

    class IView
    {
    public:
		virtual void create             (const HWND parentWindow             )       = 0;
		virtual void destroy            (void                                )       = 0;
		virtual HWND getHandle          (void                                ) const = 0;
        virtual std::wstring getText    (void                                ) const = 0;
        virtual void setText            (const std::wstring & text                  )       = 0;
		virtual void setPosition        (const Point & point, 
			                             const Dimension & size = Dimension())       = 0;
		virtual void setSize            (const Dimension & dimension         )       = 0;
        virtual void refresh            (const Rect & rect = Rect::Null()    )       = 0;
		virtual void show               (bool isShow                         )       = 0;
		virtual Dimension getDefaultSize(void                                ) const = 0;
		virtual Dimension getMinSize    (void                                ) const = 0;
		virtual Dimension getSize       (void                                ) const = 0;
		virtual Point     getPosition   (void                                ) const = 0;
		virtual bool      isWindow      (void                                ) const = 0;
		virtual RECT      getWindowRect (void                                ) const = 0;
        virtual RECT      getClientRect (void                                ) const = 0;
		virtual ~IView() {};
    };
};