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
#include "ControlBase.h"
#include "Splitbar/View.h"
#include "ISplitPaneModel.h"

namespace Gui 
{

class SplitPaneView : public ControlBase,
	                  public Modellable<ISplitPaneModel>,
			          public IHandler<Splitbar::Event>
{

	class DefaultController : public Controller<SplitPaneView>
    {
    public:
        virtual bool onSize(unsigned long x, unsigned long y)
        {
            _view->resize();
            return false;
        }
    };

	std::vector<IView*>          _views;
	std::vector<Splitbar::View*> _splitbars;
	DefaultController            _defaultController;

public:
	SplitPaneView()
	{
		_defaultController.setView(this);
		Gui::registerClass(L"Gui::SplitPaneView");
	}

	virtual void create(const HWND parentWnd)
	{
		ControlBase::createBase(L"Gui::SplitPaneView", parentWnd);

		for(size_t index = 0; index < _views.size(); ++index)
		{
			if(_views[index]->getHandle() == 0)
			{
				_views[index]->create(_hWnd);
			}
			else
			{
				::SetParent(_views[index]->getHandle(), parentWnd);
			}
		}
		syncWithModel();
	}

	virtual void add(IView * view)
	{
		_views.push_back(view);
		_model->addPane(new Resizable());
		syncWithModel();
	}

	/* IEventHandler implementation */
    virtual void handle(const Splitbar::Event & event)
    {
        if(event.id == L"SplitBarMove")
        {
			Point point = Gui::getMouseOffset();
			POINT p = point;
			
			::MapWindowPoints(0, _hWnd, &p, 1);

			_model->setSplitterPos(event.index, _model->isVertical()?p.x:p.y);
			syncWithModel();
        }
    }

	void resize()
	{
		_model->resize(_size.width, _size.height);
		syncWithModel();
	}

protected:
	void syncWithModel(void)
	{
		syncViews();
		syncSplitters();
	}
	void syncViews(void)
	{
		for(size_t index = 0; index < _model->getPanesCount(); ++index)
		{
			Resizable pane = _model->getPane(index);
			_views.at(index)->setPosition(Point(pane.x, pane.y), Dimension(pane.width, pane.height));
		}
	}

	void syncSplitters(void)
	{
		if(_hWnd)
		{
			for(size_t index = 0; index < _model->getSplittersCount(); ++index)
			{
				Splitter splitter = _model->getSplitter(index);
				if(_splitbars.size() == index)
				{
					Splitbar::View * splitbar = new Splitbar::View(splitter.type == Splitter::VERTICAL);
					splitbar->setEventHandler(this);
					splitbar->create(_hWnd);
					_splitbars.push_back(splitbar);
				}
				_splitbars.at(index)->setPosition(Point(splitter.x, splitter.y), Dimension(splitter.width, splitter.height));
				_splitbars.at(index)->setIndex(index);
			}
		}
	}
};

};//namespace