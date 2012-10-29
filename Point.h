/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include <windows.h>

namespace Gui
{
	class Point
	{
	public:
		int x;
		int y;
		
		Point(int x_ = 0, int y_ = 0):x(x_), y(y_){}
		Point(const Point & other):x(other.x), y(other.y) {}
        Point(POINT point):x(point.x), y(point.y){}

        operator POINT () const 
		{
			POINT point;
			point.x = x;
			point.y = y;
			return point;
		}

		bool operator ==(const Point & other)
		{
			return (x == other.x) && (y == other.y);
		}

		bool operator !=(const Point & other)
		{
			return !operator==(other);
		}

		Point operator -(const Point & other) const
        {
            Point point(x, y);
            point.x -= other.x;
            point.y -= other.y;
            return point;
        }
	};
};//namespace