/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
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

	virtual void create(IView * parentView)
	{
		ControlBase::createBase(L"Gui::SplitPaneView", parentView);

		for(size_t index = 0; index < _views.size(); ++index)
		{
			if(_views[index]->getHandle() == 0)
			{
				_views[index]->create(this);
			}
			else
			{
				::SetParent(_views[index]->getHandle(), parentView->getHandle());
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
					splitbar->create(this);
					_splitbars.push_back(splitbar);
				}
				_splitbars.at(index)->setPosition(Point(splitter.x, splitter.y), Dimension(splitter.width, splitter.height));
				_splitbars.at(index)->setIndex(index);
			}
		}
	}
};

};//namespace