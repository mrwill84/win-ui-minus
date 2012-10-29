/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include "../ControlBase.h"

namespace Gui
{
	class Panel : public ControlBase
	{
    public:
        class DefaultController : public Controller<Panel>
        {
        public:
		    virtual bool onPaint(WPARAM wParam, LPARAM lParam)
            { 
                _view->paint();
                return true;
            }

            virtual bool onSize(unsigned long x, unsigned long y)
            {
                _view->resize();
                return false;
            }
        };
	protected:
		Gui::ILayout    * _layout;
		Colors::Color     _color;
        DefaultController _defaultController;
	public:
		Panel()
			:ControlBase(),
             _color(Colors::DIALOG),
			 _layout(0)
		{
            _defaultController.setView(this);
			Gui::registerClass(L"Gui::Panel");
		}

		virtual void create(const HWND parentWindow)
		{
			createBase(L"Gui::Panel", parentWindow);
			if(_layout)
			{
				_layout->createControls(_hWnd);
			}
		}
		
		virtual void paint()
		{
			Graphics graphics(_hWnd);
			graphics.setBkColor(_color);
			graphics.fillRect(getClientRect());
		}

		virtual void setColor(const Colors::Color & color)
		{
			_color = color;
		}

		virtual void setLayout(ILayout * layout)
		{
			_layout = layout;
		}

		virtual void add(IView * view/*, int position = 0*/)
		{
			_layout->add(view);
		}
		
        virtual void resize(void)
        {
			if(_layout)
			{
				_layout->setResize(_size.width, _size.height);
			}
        }
	};

};