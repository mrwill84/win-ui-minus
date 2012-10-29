#pragma once

class IApplicationEventsHandler
{
public:
	virtual void onViewSplitLayout   () = 0;
	virtual void onViewDockableLayout() = 0;
};