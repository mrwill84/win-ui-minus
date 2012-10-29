#pragma once

#include "IPresentation.h"
#include "IApplicationEventsHandler.h"
#include "../DockContainer/DefaultModel.h"
#include "../SplitPane.h"

class Application : public IApplicationEventsHandler
{
protected:
	IPresentation                  * _presentation;
	Gui::DockContainer::DefaultModel _dockContainerModel;
	Gui::SplitPane                   _splitPaneModel;

public:
	Application()
		:_splitPaneModel(Gui::Splitter::VERTICAL)
	{
	}

	void setPresentation(IPresentation * presentation)
	{
		_presentation = presentation;
		_presentation->setDockContainerModel(&_dockContainerModel);
		_presentation->setSplitPaneModel(&_splitPaneModel);
	}

	int start()
	{
		if(_presentation)
		{
			return _presentation->start();
		}
		return 0;
	}

	virtual void onViewSplitLayout()
	{
		_presentation->setSplitLayout();
	}

	virtual void onViewDockableLayout()
	{
		_presentation->setDockableLayout();
	}
};