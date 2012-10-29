#pragma once

#include "resource.h"
#include "IPresentation.h"
#include "IApplicationEventsHandler.h"
#include "../ApplicationWindow.h"
#include "../SplitPaneView.h"
#include "../DockContainer/View.h"
#include "../Controls.h"
#include "../ScrollPane.h"
#include "../Graph/View.h"
#include "../ListPane.h"
#include "../Properties/View.h"

class Presentation : public IPresentation,
	                 public Gui::ApplicationWindow
{
protected:
	class Controller : public Gui::ApplicationWindow::DefaultController
	{
	protected:
		IApplicationEventsHandler * _handler;
	public:
		Controller(IApplicationEventsHandler * handler)
			:_handler(handler)
		{
		}
		virtual bool onCommand(const Gui::Command & command)
		{
			switch(command.id)
			{
				case ID_VIEW_DOCKABLELAYOUT:
				{
					//_handler->onViewDockableLayout();
					break;
				}
				case ID_VIEW_SPLITLAYOUT:
				{
					//_handler->onViewSplitLayout();
					break;
				}
			}
			return false;
		}
	};

	Controller               _controller;
	Gui::SplitPaneView       _splitPane;
	Gui::DockContainer::View _dockContainer;
	Gui::ScrollPane          _graphScrollPane;
	Gui::Graph::View         _graphView;
	Gui::ScrollPane          _listScrollPane;
	Gui::ListPane::View      _listPane;
	Gui::ScrollPane          _propertiesScrollPane;
	Gui::Properties::View    _propertiesView;


public:
	Presentation(IApplicationEventsHandler * handler)
		:Gui::ApplicationWindow(L"Examples"),
		 _controller(handler)
	{
		_controller.setView(this);
		setTitle(L"Gui::Examples");
	}

	virtual int start(void)
	{
		create();
        Gui::Menu::View mainMenu(IDC_EXAMPLES);
		setMenu(mainMenu);
		
		//_splitPane.add(new Gui::Edit());
		//_splitPane.add(new Gui::Edit());
		//_splitPane.add(new Gui::Edit());
		//_splitPane.getModel()->setSplitterPos(0, 50);
		//_splitPane.getModel()->setSplitterPos(1, 150);
		
		_graphView.getModel()->addNode(Gui::Graph::IModel::Node(10, 10, 0, L"test1"));
		_graphView.getModel()->addNode(Gui::Graph::IModel::Node(20, 20, 0, L"test2"));
		_graphView.getModel()->addNode(Gui::Graph::IModel::Node(30, 30, 0, L"test3"));
		_graphView.getModel()->addNode(Gui::Graph::IModel::Node(40, 40, 0, L"test4"));

		_propertiesView.getPropertiesModel()->addProperty(Gui::Properties::Property(L"test name1", L"test value1"));
		_propertiesView.getPropertiesModel()->addProperty(Gui::Properties::Property(L"test name2", L"test value2"));
		_propertiesView.getPropertiesModel()->addProperty(Gui::Properties::Property(L"test name3", L"test value3"));
		_propertiesView.getPropertiesModel()->addProperty(Gui::Properties::Property(L"test name4", L"test value4"));
		_propertiesView.getPropertiesModel()->addProperty(Gui::Properties::Property(L"test name5", L"test value5"));
		_propertiesView.getPropertiesModel()->addProperty(Gui::Properties::Property(L"test name6", L"test value6"));

		_graphScrollPane.setPane(&_graphView);
		_listPane.setText(L"ListPane");
		_listScrollPane.setPane(&_listPane);
		_propertiesScrollPane.setPane(&_propertiesView);

		_dockContainer.add(&_listScrollPane       , Gui::DockContainer::Dock::WEST  );
		_dockContainer.add(&_propertiesScrollPane , Gui::DockContainer::Dock::EAST  );
		_dockContainer.add(new Gui::Edit(L"SOUTH"), Gui::DockContainer::Dock::SOUTH );
		_dockContainer.add(&_graphScrollPane      , Gui::DockContainer::Dock::CENTER);

		setContextPane(&_dockContainer);
		
		return Gui::ApplicationWindow::start();
	}

	virtual void setSplitLayout(void)
	{
	}
	
	virtual void setDockableLayout(void)
	{
	}

	virtual void setDockContainerModel(Gui::DockContainer::IModel * containerModel)
	{
		_dockContainer.setModel(containerModel);
	}

	virtual void setSplitPaneModel(Gui::ISplitPaneModel * splitModel)
	{
		_splitPane.setModel(splitModel);
	}
};