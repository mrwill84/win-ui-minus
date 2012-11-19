/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include <string>
#include "WinUtils.h"
#include "Graphics.h"
#include "Controller.h"
#include "IEventHandler.h"
#include "ControlBase.h"

namespace Gui
{
	/**
	 *	Titled frame view 
	 */
	class TitledFrame : public ControlBase,
		                public Controller<TitledFrame>,
                        public Informator<Event>
	{
	protected:
		std::wstring _title;
		IView      * _frame;
        bool         _isCreated;
	public:
		TitledFrame(const std::wstring & title = L"", IView * frame = 0)
			:_title(title), _frame(frame), _isCreated(false)
        {
			Gui::registerClass(L"Gui::TitledFrame");
        }

		void create(IView * parentView)
		{
			createBase(L"Gui::TitledFrame", parentView);
			setView(this);

			if(_frame)
			{
			    _frame->create(this);
                if(_title.empty())
                {
                    setTitle(_frame->getText());
                }
			}
		}

		virtual void setTitle(const std::wstring & title)
		{
			_title = title;
		}

        virtual void setText(const std::wstring & text)
        {
            ControlBase::setText(text);
            setTitle(text);
        }

		virtual void setFrame(IView * view)
		{
			_frame = view;
            if(_hWnd && _frame)
            {
				if(_frame->getHandle() == 0)
				{
					_frame->create(this);
                    _isCreated = true;
				}
				else
				{
					::SetParent(_frame->getHandle(), _hWnd);
				}
                if(_title.empty())
                {
                    setTitle(_frame->getText());
                }
            }
		}

        virtual void destroy()
        {
            if(_frame && _isCreated)
            {
                _frame->destroy();
            }
            ControlBase::destroy();
        }

        virtual Dimension getDefaultSize() const 
        { 
            if(_frame)
            {
                return _frame->getDefaultSize(); 
            }
            return Dimension();
        }
		
        virtual bool onPaint(WPARAM wParam, LPARAM lParam)
		{
            Graphics graphics(_hWnd);
			Pen greyPen = Pen::create(PS_SOLID, 1, Colors::GREY);
			
			RECT clientRect = getClientRect();
			RECT contentRect = {0, 20, clientRect.right, clientRect.bottom};

			graphics.setPen(greyPen);
			graphics.rectangle(contentRect);

            paintTitle(graphics);

			return true;
		}

		virtual bool onSize(unsigned long x, unsigned long y)
		{
            refreshTitle();
			if(_frame)
			{
				_frame->setPosition(Point(1, 21), Dimension(_size.width - 2, _size.height - 22));
			}
            return false;
		}
		
    protected:
        virtual void refreshTitle(void)
        {
            refresh(Rect(static_cast<int>(_size.width) - 40, 0, static_cast<int>(_size.width), 20));
        }

        virtual void paintTitle(Graphics & graphics)
        {
			Pen greyPen = Pen::create(PS_SOLID, 1, Colors::GREY);
			
			RECT clientRect = getClientRect();
			RECT titleRect = {0, 0, clientRect.right, 20};

			graphics.setPen(greyPen);
			graphics.rectangle(titleRect);
			
			::InflateRect(&titleRect, -1, -1);

			TRIVERTEX vertexes[2];
			GRADIENT_RECT gradientRects = {0};

			vertexes[0].x = titleRect.left;
			vertexes[0].y = titleRect.top;
			vertexes[0].Red   = 0xD500;
			vertexes[0].Green = 0xDD00;
			vertexes[0].Blue  = 0xE600;

			vertexes[1].x = titleRect.right  - titleRect.left + 1;
			vertexes[1].y = titleRect.bottom - titleRect.top  + 1;
			vertexes[1].Red   = 0x9A00;
			vertexes[1].Green = 0xAD00;
			vertexes[1].Blue  = 0xC220;

			gradientRects.UpperLeft  = 0;
			gradientRects.LowerRight = 1;

			::GradientFill(graphics, vertexes, 2, &gradientRects, 1, GRADIENT_FILL_RECT_V);

            graphics.setBkMode(TRANSPARENT);
			graphics.setFont(Gui::Fonts::DEFAULT());
			graphics.text(3, 3, _title.c_str());
        }
	};
}; // namespace