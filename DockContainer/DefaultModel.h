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

#include <vector>

#include "IModel.h"
#include "../SplitPane.h"

namespace Gui { namespace DockContainer
{

class ContextPane : public Resizable
{
protected:
	Resizable * _pane;
public:
	ContextPane()
		:Resizable(), _pane(0)
	{
	}

	virtual void addPane(Resizable * pane)
	{
		_pane = pane;
	}

	virtual void setPosition(int x_, int y_)
	{
		Resizable::setPosition(x_, y_);
		if(_pane)
		{
			_pane->setPosition(x_, y_);
		}
	}

	virtual void setSize(size_t width_, size_t height_)
	{
		Resizable::setSize(width_, height_);
		if(_pane)
		{
			_pane->setSize(width_, height_);
		}
	}
};

class DefaultModel : public IModel
{
protected:
	std::vector<Dock*>      _docks;
	std::vector<SplitPane*> _splitPanes;
	
	SplitPane               _westPane;
	SplitPane               _eastPane;
	SplitPane               _southPane;

	SplitPane               _westCenterSplit;
	SplitPane               _eastCenterSplit;
	SplitPane               _southCenterSplit;
	ContextPane             _contextPane;

public:
	DefaultModel()
		:_westPane(Splitter::HORIZONTAL),
		 _eastPane(Splitter::HORIZONTAL),
		 _southPane(Splitter::VERTICAL),
		 _westCenterSplit(Splitter::VERTICAL),
		 _eastCenterSplit(Splitter::VERTICAL),
		 _southCenterSplit(Splitter::HORIZONTAL)
	{
		_southCenterSplit.addPane(&_eastCenterSplit);

		_westCenterSplit.addPane(&_southCenterSplit);

		_eastCenterSplit.addPane(&_contextPane     );

		_splitPanes.push_back(&_westPane);
		_splitPanes.push_back(&_eastPane);
		_splitPanes.push_back(&_southPane);

		_splitPanes.push_back(&_westCenterSplit);
		_splitPanes.push_back(&_eastCenterSplit);
		_splitPanes.push_back(&_southCenterSplit);
	}

	virtual size_t getDocksCount(void) const
	{
		return _docks.size();
	}

	virtual void addDock(const Dock::Position & pos)
	{
		_docks.push_back(new Dock(pos));
		dockPane(_docks.at(_docks.size() - 1), pos);
		recalculateDockIds();
	}

	virtual Dock getDock(size_t index) const
	{
		return *_docks.at(index);
	}

	virtual void resize(size_t width, size_t height)
	{
		_westCenterSplit.setSize(width, height);
        if(_westPane.getPanesCount() == 0)
        {
            _westPane.setPosition(0, 0);
            _westPane.setSize(200, height);
        }
        if(_eastPane.getPanesCount() == 0)
        {
            _eastPane.setPosition(static_cast<int>(width) - 200, 0);
            _eastPane.setSize(200, static_cast<int>(height) - 200);
        }
        if(_southPane.getPanesCount() == 0)
        {
            _southPane.setPosition(200, static_cast<int>(height) - 200);
            _southPane.setSize(width - 200, 200);
        }
	}

	virtual size_t getSplittersCount(void) const
	{
		size_t count = 0;
		for(size_t index = 0; index < _splitPanes.size(); ++index)
		{
			count += _splitPanes.at(index)->getSplittersCount();
		}
		return count;
	}
	
	virtual Splitter getSplitter(size_t index) const
	{
		return findSplitter(index);;
	}

	virtual void setSplitterPos(size_t index, int x, int y)
	{
		SplitPane * splitPane = findSplitPaneForBigSplitterIndex(index);
		size_t smallIndex     = findSmallSplitterIndex(index);
		if(splitPane->isVertical())
		{
			splitPane->setSplitterPos(smallIndex, x);
		}
		else
		{
			splitPane->setSplitterPos(smallIndex, y);
		}
	}

	virtual Rect getDockArea(const Dock::Position & pos )
	{
		RECT rect;
		SplitPane * splitPane = getSplitPane(pos);
		rect.left   = splitPane->x;
		rect.top    = splitPane->y;
		rect.right  = splitPane->x + static_cast<int>(splitPane->width );
		rect.bottom = splitPane->y + static_cast<int>(splitPane->height);
		return rect;
	}

	virtual Dock::Position getDockPosition(int x, int y) const
	{
		if(isPointInWestSide(x, y))
		{
			return Dock::WEST;
		}
		else if(isPointInEastSide(x, y))
		{
			return Dock::EAST;
		}
		else if(isPointInSouthSide(x, y))
		{
			return Dock::SOUTH;
		}
		return Dock::FREE;
	}

	virtual void redock(size_t index, const Dock::Position & pos)
	{
		Dock * dock = _docks.at(index);
		if(dock->dock != Dock::FREE)
		{
			SplitPane * splitPane = _splitPanes.at(dock->splitPaneIndex);
			splitPane->removePane(dock->paneInSplitterIndex);
			if(splitPane->getPanesCount() == 0)
			{
				detachSplitPane(dock->dock);
			}
		}
		dockPane(dock, pos);
	}

	void undock(size_t index)
	{
		Dock * dock = _docks.at(index);
		SplitPane * splitPane = _splitPanes.at(dock->splitPaneIndex);
		splitPane->removePane(dock->paneInSplitterIndex);
		if(splitPane->getPanesCount() == 0)
		{
			detachSplitPane(dock->dock);
		}
		dock->dock = Dock::FREE;
	}

    virtual void hideDock(size_t index, bool isHide)
    {
        Dock * dock = _docks.at(index);
        if(isHide)
        {
            Dock::Position pos = dock->dock;
            if(pos != Dock::FREE)
            {
                undock(index);
                dock->dock = pos;
            }
        }
        else
        {
            dockPane(dock, dock->dock);
        }
    }
protected:
	void dockPane(Dock * pane, const Dock::Position & pos)
	{
		if(pos == Dock::CENTER)
		{
			_contextPane.addPane(pane);
		}
		else
		{
            SplitPane * splitPane = 0;
            if(pos != Dock::FREE)
            {
			    splitPane = getSplitPane(pos);
			    if(splitPane->getPanesCount() == 0)
			    {
				    attachSplitPane(pos);
			    }
            }
			pane->dock = pos;
			pane->splitPaneIndex = (size_t)pos;
            if(splitPane)
            {
			    pane->paneInSplitterIndex = splitPane->getPanesCount();
			    splitPane->addPane(pane);
            }
		}
	}


	void recalculateDockIds(void)
	{
		for(size_t index = 0; index < _docks.size(); ++index)
		{
			_docks[index]->id = index;
		}
	}
	
	Splitter findSplitter(size_t bigSplitterIndex) const
	{
		SplitPane * splitPane = findSplitPaneForBigSplitterIndex(bigSplitterIndex);
		return splitPane->getSplitter(findSmallSplitterIndex(bigSplitterIndex));
	}

	SplitPane * findSplitPaneForBigSplitterIndex(size_t bigSplitterIndex) const
	{
		SplitPane * splitPane = 0;
		size_t mutableIndex = bigSplitterIndex;
		for(size_t index = 0; index < _splitPanes.size(); ++index)
		{
			if(mutableIndex < _splitPanes[index]->getSplittersCount())
			{
				splitPane = _splitPanes[index];
				break;
			}
			else
			{
				mutableIndex -= _splitPanes[index]->getSplittersCount();
			}
		}
		return splitPane;
	}

	size_t findSmallSplitterIndex(size_t bigSplitterIndex) const
	{
		size_t mutableIndex = bigSplitterIndex;
		for(size_t index = 0; index < _splitPanes.size(); ++index)
		{
			if(mutableIndex < _splitPanes[index]->getSplittersCount())
			{
				break;
			}
			else
			{
				mutableIndex -= _splitPanes[index]->getSplittersCount();
			}
		}
		return mutableIndex;
	}

	bool isPointInWestSide(int x, int y) const
	{
		return hasPoint(_westPane, x, y);
	}

	bool isPointInEastSide(int x, int y) const
	{
		return hasPoint(_eastPane, x, y);
	}
	bool isPointInSouthSide(int x, int y) const
	{
		return hasPoint(_southPane, x, y);
	}

	bool hasPoint(const Resizable & pane, int x, int y) const
	{
		if((pane.x < x) && ((pane.x + static_cast<int>(pane.width )) > x) &&
		   (pane.y < y) && ((pane.y + static_cast<int>(pane.height)) > y))
		{
			return true;
		}
		return false;
	}

	SplitPane * getSplitPane(const Dock::Position & pos)
	{
		SplitPane * pane = 0;
		if(pos == Dock::WEST)
		{
			pane = &_westPane;
		}
		else if(pos == Dock::EAST)
		{
			pane = &_eastPane;
		}
		else if(pos == Dock::SOUTH)
		{
			pane = &_southPane;
		}
		return pane;
	}

	void detachSplitPane(const Dock::Position & pos)
	{
		if(pos == Dock::WEST)
		{
			_westCenterSplit.removePane(0);
		}
		else if(pos == Dock::EAST)
		{
			_eastCenterSplit.removePane(1);
		}
		else if(pos == Dock::SOUTH)
		{
			_southCenterSplit.removePane(1);
		}
	}

	void attachSplitPane(const Dock::Position & pos)
	{
		if(pos == Dock::WEST)
		{
			_westCenterSplit.insertPane(0, &_westPane);
		}
		else if(pos == Dock::EAST)
		{
			_eastCenterSplit.insertPane(1, &_eastPane);
		}
		else if(pos == Dock::SOUTH)
		{
			_southCenterSplit.insertPane(1, &_southPane);
		}
	}
};

}};//namespace