#pragma once

#include "../DockContainer/IModel.h"
#include "../ISplitPaneModel.h"

class IPresentation
{
public:
	virtual int  start                (void                                       ) = 0;
	virtual void setSplitLayout       (void                                       ) = 0;
	virtual void setDockableLayout    (void                                       ) = 0;
	virtual void setDockContainerModel(Gui::DockContainer::IModel * containerModel) = 0;
	virtual void setSplitPaneModel    (Gui::ISplitPaneModel * splitModel          ) = 0;
};