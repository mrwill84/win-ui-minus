/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */
 
#pragma once

#include "ILayout.h"
#include "IView.h"
#include <vector>

namespace Gui
{
	class BorderLayout : public ILayout
    {
	private:
		IView * _centerView;
		IView * _eastView;
		IView * _westView;
		IView * _northView;
		IView * _southView;
    public:
		enum Position
		{
			CENTER,
			EAST,
			WEST,
			NORTH,
			SOUTH
		};
		
		BorderLayout()
			:_centerView(0), _eastView(0), _westView(0), _northView(0), _southView(0)
		{
		}

		virtual void add(IView * control)
		{
			if(!_centerView)
			{
				add(control, CENTER);
			}
			else if(!_eastView)
			{
				add(control, EAST);
			}
			else if(!_westView)
			{
				add(control, WEST);
			}
			else if(!_northView)
			{
				add(control, NORTH);
			}
			else if(!_southView)
			{
				add(control, SOUTH);
			}
		}

		virtual void add(IView * control, Position position)
		{
			switch(position)
			{
				case CENTER:
				{
					_centerView = control;
					break;
				}
				case EAST:
				{
					_eastView = control;
					break;
				}
				case WEST:
				{
					_westView = control;
					break;
				}
				case NORTH:
				{
					_northView = control;
					break;
				}
				case SOUTH:
				{
					_southView = control;
					break;
				}
			}
		}

		virtual void setResize(const size_t x, const size_t y)
		{
			if(_centerView)
			{
				_centerView->setPosition(Point((_westView?static_cast<int>(x * 0.25):0) + 1, 1));
				_centerView->setSize(Dimension((_westView?static_cast<int>(x - (x * 0.25)):x) - 2, (_southView?(y - 20):y) - 2));
				_centerView->refresh();
			}
			if(_westView)
			{
				_westView->setPosition(Point(0, 0));
				_westView->setSize(Dimension(static_cast<size_t>(x * 0.25) - 5, y));
				_westView->refresh();
			}
			if(_southView)
			{
				_southView->setPosition(Point(1, (y - 20) + 1));
				_southView->setSize(Dimension(x  - 2, (20) - 2));
			}
		}
		
		virtual void createControls(IView * parentView)
		{
			if(_centerView)
			{
				_centerView->create(parentView);
			}
			if(_westView)
			{
				_westView->create(parentView);
			}
			if(_southView)
			{
				_southView->create(parentView);
			}
		}

		virtual void clean(void)
		{
			if(_centerView)
			{
				_centerView->destroy();
			}
			if(_westView)
			{
				_westView->destroy();
			}
			if(_southView)
			{
				_southView->destroy();
			}
		}
		
		virtual ~BorderLayout() {};
    };

	template <int COLUMN_COUNT, int ROW_COUNT = 1>
	class GridLayout : public ILayout
    {
	private:
		size_t     _insertionColumn;
		size_t     _insertionRow;
		std::vector<std::vector<IView*> > 
			       _controls;
		size_t     _verticalLines[COLUMN_COUNT];
		size_t     _horizontalLines[ROW_COUNT];
		HWND       _hParent;
		size_t     _width;
		size_t     _height;
    public:
		GridLayout()
			:_insertionColumn(0),
			 _insertionRow(0),
			 _hParent(0)
		{
		}

		virtual void add(IView * view/*, int position = 0*/)
		{
			if(_insertionColumn == 0)
			{
				std::vector<IView*> row;
				_controls.push_back(row);
			}
			
			_verticalLines[_insertionColumn] = 100;

			std::vector<IView*> row = _controls.at(_insertionRow);
			row.push_back(view);
			_controls[_insertionRow] = row;
			_insertionColumn++;

			if(_insertionColumn >= COLUMN_COUNT)
			{
				_insertionRow++;
				_insertionColumn = 0;
			}
			if(_hParent)
			{
				view->create(_hParent);
				refresh();
			}
		}

		virtual void setControl(size_t col, size_t row, IView * control)
		{
			if(row < _controls.size())
			{
				std::vector<IView*> rowVector = _controls.at(row);
				rowVector[col] = control;
				_controls[row] = rowVector;
				refresh();
			}
		}

		void refresh()
		{
			_verticalLines[0] = _width/COLUMN_COUNT;
			size_t yPos = 0;
			for(size_t rowNumber = 0; rowNumber < _insertionRow; rowNumber++)
			{
				std::vector<IView*> row = _controls.at(rowNumber);
				IView * view = row.at(0);
				view->setPosition(Point(0, static_cast<int>(yPos)));
				yPos += view->getMinSize().height;
				view->setSize(Dimension(_verticalLines[0], view->getMinSize().height));

				for(size_t columnNumber = 1; columnNumber < row.size(); columnNumber++)
				{
					IView * control = row.at(columnNumber);
					control->setPosition(Point(static_cast<int>(_verticalLines[0]), static_cast<int>(rowNumber*25)));
					control->setSize(Dimension(_width - _verticalLines[0], 20));
				}
			}
    		::RedrawWindow(_hParent, NULL, NULL, NULL);
		}

		virtual void clean()
		{
			for(size_t rowNumber = 0; rowNumber < _insertionRow; rowNumber++)
			{
				std::vector<IView*> row = _controls.at(rowNumber);

				for(size_t columnNumber = 0; columnNumber < row.size(); columnNumber++)
				{
					IView * control = row.at(columnNumber);
					control->destroy();
				}
			}
			_controls.clear();
			_insertionRow = 0;
			refresh();
		}

		virtual void setResize(const size_t width, const size_t height)
		{
			_width  = width;
			_height = height;
			refresh();
		}

		virtual void createControls(const HWND parentWindow)
		{
			_hParent = parentWindow;
			for(size_t rowNumber = 0; rowNumber < _insertionRow; rowNumber++)
			{
				std::vector<IView *> row = _controls.at(rowNumber);

				for(size_t columnNumber = 0; columnNumber < row.size(); columnNumber++)
				{
					IView * control = row.at(columnNumber);
					control->create(parentWindow);
				}
			}
			refresh();
		}
		
		virtual ~GridLayout() {};
    };

	class AbsoluteLayout : public ILayout
    {
	private:
		struct ControlWrapper
		{
			IView *   control;
			Point     point;
			Dimension dimension;

			ControlWrapper(Gui::IView * control_, Gui::Point & point_, Gui::Dimension & dimension_)
				:control(control_), point(point_), dimension(dimension_){};
		};

		std::vector<ControlWrapper> _controls;

	public:
		virtual void add(IView * view) {};

		virtual void add(IView * view, Point & point, Dimension & dimension)
		{
			ControlWrapper wrapper(view, point, dimension);
			_controls.push_back(wrapper);
		}

		virtual void setResize(const size_t width, const size_t height) 
		{
			for(size_t index = 0; index < _controls.size(); ++index)
			{
				IView * control = _controls.at(index).control;
				Point point = _controls.at(index).point;
				Dimension dimension = _controls.at(index).dimension;

				if(point.x < 0)
				{
					point.x = width - dimension.width + point.x;
				}
				if(point.y < 0)
				{
					point.y = height - dimension.height + point.y;
				}

				control->setPosition(point, dimension);
			}
		};

		virtual void createControls(IView * parentView) 
		{
			for(size_t index = 0; index < _controls.size(); ++index)
			{
				_controls.at(index).control->create(parentView);
			}
		};
		virtual void clean(void) 
		{
			for(size_t index = 0; index < _controls.size(); ++index)
			{
				_controls.at(index).control->destroy();
			}		
		};
	};
};