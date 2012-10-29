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

		Dimension(void)
			:_width(-1), _height(-1)
		{}


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