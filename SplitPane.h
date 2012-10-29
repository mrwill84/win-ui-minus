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

#include "ResizablePane.h"
#include "ISplitPaneModel.h"
#include "Modellable.h"

namespace Gui 
{
class SplitPane : public Resizable,
	              public ISplitPaneModel
{
	std::vector<Resizable*> _panes;
	std::vector<Splitter>   _splitters;
	Splitter::Type          _type;
public:
	static const int MIN_PANE_SIZE = 7;

	SplitPane(const Splitter::Type & type)
		:Resizable(), _type(type)
	{
	}

	virtual void setPosition(int x_, int y_)
	{
        int oldX = x;
        int oldY = y;
		Resizable::setPosition(x_, y_);
		if(width && height)
		{
            for(size_t index = 0; index <_splitters.size(); ++index)
            {
                int pos = _splitters[index].pos;
                int newPos = 0;
                if(_type == Splitter::VERTICAL)
                {
                    newPos = pos + (static_cast<int>(x) - static_cast<int>(oldX));
                    _splitters[index].pos = newPos;
                }
                else
                {
                    newPos = pos + (static_cast<int>(y) - static_cast<int>(oldY));
                    _splitters[index].pos = newPos;

                }
            }
			recalculatePanes();
			resizeSplitters();
		}
	}

	virtual void setSize(size_t width_, size_t height_)
	{
		bool isInitialResize = false;
		if((width == 0) && (height == 0))
		{
			isInitialResize = true;
		}
        size_t oldWidth  = width;
        size_t oldHeight = height;

		Resizable::setSize(width_, height_);
		if(width && height)
		{
            for(size_t index = 0; index <_splitters.size(); ++index)
            {
                int pos = _splitters[index].pos;
                int newPos = 0;
                if(_type == Splitter::VERTICAL)
                {
                    newPos = 
                        pos + ((static_cast<int>(width) - static_cast<int>(oldWidth))/static_cast<int>(_panes.size()));
                    setSplitterPos(index, newPos);
                }
                else
                {
                    newPos = 
                        pos + ((static_cast<int>(height) - static_cast<int>(oldHeight))/static_cast<int>(_panes.size()));
                    setSplitterPos(index, newPos);
                        
                }
            }

			if(isInitialResize)
			{
				recalculateSplitters();
			}
			recalculatePanes();
			resizeSplitters();
		}
	}

	virtual void addPane(Resizable * pane)
	{
		if(_panes.size())
		{
			Splitter splitter(_type);
	        _splitters.push_back(splitter);
		}
		_panes.push_back(pane);
		if(width && height)
		{
			recalculateSplitters();
			recalculatePanes();
		}
	}
	
	virtual void removePane(size_t index)
	{
		_panes.erase(_panes.begin() + index);
		cleanupSplitters();
		recalculateSplitters();
		recalculatePanes();
	}

	virtual void insertPane(size_t index, Resizable * pane)
	{
		if(_panes.size())
		{
			Splitter splitter(_type);
	        _splitters.push_back(splitter);
		}
		_panes.insert(_panes.begin() + index, pane);
		if(width && height)
		{
			recalculateSplitters();
			recalculatePanes();
		}
	}

	virtual bool isVertical(void) const
	{
		return (_type == Splitter::VERTICAL);
	}
	
	virtual void setSplitterPos(size_t index, int pos)
	{
		Splitter prevSplitter = getSplitter(index - 1);
		Splitter nextSplitter = getSplitter(index + 1);

		int correctedPos = pos;
		if(correctedPos < (prevSplitter.pos + 2 + MIN_PANE_SIZE))
		{
			correctedPos = prevSplitter.pos + 2 + MIN_PANE_SIZE;
		}
		else if(correctedPos > (nextSplitter.pos - 2 - MIN_PANE_SIZE))
		{
			correctedPos = nextSplitter.pos - 2 - MIN_PANE_SIZE;
		}

        if((index >= 0) && (index < _splitters.size()))
        {
		    _splitters[index].pos = correctedPos;
        }
		resizeSplitters();
        recalculatePanes();
	}

	size_t getPanesCount(void) const
	{
		return _panes.size();
	}

	virtual Resizable getPane(size_t index) const
	{
		return *_panes.at(index);
	}

	size_t getSplittersCount(void) const
	{
		return _splitters.size();
	}

	Splitter getSplitter(size_t index) const
	{
		Splitter splitter(_type);
		if(index == std::wstring::npos)
		{
			splitter.pos = ((_type == Splitter::VERTICAL)?x:y) - 2;
		}
		else if(index >= _splitters.size())
		{
			splitter.pos = ((_type == Splitter::VERTICAL)?
                x + static_cast<int>(width):y + static_cast<int>(height)) + 2;
		}
		else
		{
			splitter = _splitters.at(index);
		}

		return splitter;
	}

	virtual void resize(size_t width, size_t height)
	{
		setSize(width, height);
	}

protected:
	void recalculatePanes(void)
	{
		for(size_t index = 0; index < _panes.size(); ++index)
		{
			Splitter lefttopSplitter     = getSplitter(index - 1);
			Splitter rightbottomSplitter = getSplitter(index);
			
			Resizable * pane = _panes[index];
			pane->setIndex(index);

			if(_type == Splitter::VERTICAL)
			{
				pane->setPosition(lefttopSplitter.pos + 2, y);
				pane->setSize    (rightbottomSplitter.pos - lefttopSplitter.pos - 4, height);
			}
			else 
			{
				pane->setPosition(x, lefttopSplitter.pos + 2);
				pane->setSize    (width, rightbottomSplitter.pos - lefttopSplitter.pos - 4);
			}
		}
	}

	void resizeSplitters(void)
	{
		for(size_t index = 0; index < _splitters.size(); ++index)
		{
			Splitter splitter = _splitters[index];
			if(_type == Splitter::VERTICAL)
			{
				splitter.y      = y;
				splitter.x      = splitter.pos - 2;
				splitter.width  = 4;
				splitter.height = height;
			}
			else
			{
				splitter.x      = x;
				splitter.y      = splitter.pos - 2;
				splitter.height = 4;
				splitter.width  = width;
			}
			_splitters[index] = splitter;
		}
	}

	void recalculateSplitters(void)
	{
		for(size_t index = 0; index < _splitters.size(); ++index)
		{
			_splitters[index].pos =  
				(_type == Splitter::VERTICAL)?
					x + ((static_cast<int>(width )/static_cast<int>(_panes.size())) * (static_cast<int>(index) + 1)):
                    y + ((static_cast<int>(height)/static_cast<int>(_panes.size())) * (static_cast<int>(index) + 1));
		}
		resizeSplitters();
	}

	void cleanupSplitters(void)
	{
		while(_splitters.size() && (_splitters.size() >= _panes.size()))
		{
			_splitters.erase(_splitters.begin() + 0);
		}
	}
};

};//namespace