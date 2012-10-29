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

#include "../ResizablePane.h"
#include "../Rect.h"

namespace Gui { namespace DockContainer
{

class Dock : public Resizable
{
public:
	enum Position
	{
		WEST, 
		EAST,
		SOUTH,
		CENTER,
		NORTH,
		FREE
	};
	Position  dock;
	size_t    id;
	size_t    splitPaneIndex;
	size_t    paneInSplitterIndex;

	Dock(const Position & dock_ = FREE)
		:Resizable(), dock(dock_)
	{}

	virtual void setIndex(size_t index)
	{
		paneInSplitterIndex = index;
	}
};


class IModel
{
public:
	virtual size_t   getDocksCount    (void                       ) const = 0;
	virtual Dock     getDock          (size_t index               ) const = 0;
	virtual size_t   getSplittersCount(void                       ) const = 0;
	virtual Splitter getSplitter      (size_t index               ) const = 0;
	virtual void     addDock          (const Dock::Position & pos )       = 0;
	virtual void     resize           (size_t width, size_t height)       = 0;
	virtual void     setSplitterPos   (size_t index, int x, int y )       = 0;
	virtual Rect     getDockArea      (const Dock::Position & pos )       = 0;
	virtual Dock::Position getDockPosition(int x, int y           ) const = 0;
	virtual void     redock           (size_t index, const Dock::Position & pos) = 0;
	virtual void     undock           (size_t index               )       = 0;
    virtual void     hideDock         (size_t index, bool isHide  )       = 0;
};

}};//namespace