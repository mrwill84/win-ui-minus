#pragma once

#define _WIN32_WINNT 0x0500

#include "resource.h"

#include "../ApplicationWindow.h"
#include "Application.h"
#include "Presentation.h"

class Examples
{
protected:
	Application  _application;
	Presentation _presentation;
public:
	Examples()
		:_presentation(&_application)
	{
		_application.setPresentation(&_presentation);
	}

	int start()
	{
		return _application.start();
	}
};
