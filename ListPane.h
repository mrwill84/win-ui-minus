/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include "ControlBase.h"
#include "Modellable.h"
#include "ListPaneIModel.h"

namespace Gui { namespace ListPane
{

class ItemView
{
public:
	virtual void draw(Gui::Graphics & graphics, const RECT & rect, const IModel::Item & item) = 0;
};

class DefaultItemView : public ItemView
{
public:
	virtual void draw(Gui::Graphics & graphics, const RECT & rect, const IModel::Item & item)
	{
		Gui::Color bkColor   = graphics.setBkColor(Gui::Color(179, 217, 255));
        graphics.fillRect(rect);
		if(item.isSelected)
		{
			Gui::Pen greyPen = Gui::Pen::create(PS_SOLID, 1, Gui::Colors::MENUHILIGHT);

			RECT itemRect = rect;
			graphics.setPen(greyPen);
			::InflateRect(&itemRect, -1, -1);
			graphics.rectangle(itemRect);
		}
	}
};

class PropertyItemView
{
public:
	virtual void draw(Gui::Graphics & graphics, const RECT & rect, const IModel::Item & item)
	{
		Gui::Color bkColor   = graphics.setBkColor(Gui::Color(179, 217, 255));
        graphics.fillRect(rect);
		if(item.isSelected)
		{
			Gui::Pen greyPen = Gui::Pen::create(PS_SOLID, 1, Gui::Colors::MENUHILIGHT);

			RECT itemRect = rect;
			graphics.setPen(greyPen);
			::InflateRect(&itemRect, -1, -1);
			graphics.rectangle(itemRect);
		}
	}
};

class DefaultModel : public IModel
{
	std::vector<Item> _items;
public:
	DefaultModel()
	{
		_items.push_back(Item());
		_items.push_back(Item());
		_items.push_back(Item());
		_items.push_back(Item());
		_items.push_back(Item());
	}

	virtual size_t getItemsCount(void) const
	{
		return _items.size();
	}

	virtual Item getItem(size_t index) const
	{
		return _items.at(index);
	}
	
	void recalculateItems(void)
	{
		for(size_t index = 0; index < _items.size(); ++index)
		{
			Item item   = _items[index];
			item.top    = static_cast<int>(index) * 20;
			item.bottom = item.top + 20;
			_items[index] = item;
		}
	}
	
	virtual void setSelection(size_t index)
	{
		for(size_t index_ = 0; index_ < _items.size(); ++index_)
		{
			if(index_ == index)
			{
				_items[index_].isSelected = true;
			}
			else
			{
				_items[index_].isSelected = false;
			}
		}
	}

    virtual size_t getSelection(void) const
    {
        return 0;
    }
};

class View : public ControlBase,
	         public Modellable<IModel>
{
public:
	class DefaultController : public Controller<View>
    {
    public:
        virtual bool onSize(unsigned long x, unsigned long y)
        {
            _view->resize();
            return true;
        }

		virtual bool onPaint(WPARAM wParam, LPARAM lParam)
        { 
            _view->paint();
            return true;
        }

        virtual bool onMouseWheel(WPARAM wParam, LPARAM lParam)
        {
            int zDelta = (int)(short)HIWORD(wParam);
            return false;
        }

        virtual bool onLButtonDown(WPARAM wParam, LPARAM lParam)
        {
            Gui::Point pt(LOWORD(lParam), HIWORD(lParam));
            int index = _view->getIndexUnderMouse(pt);

            selectItem(index);
            return false;
        }

        virtual bool onAsyncUpdate(void)
        {
            _view->resize();
            _view->refresh();
            return true;
        }
    protected:
        void selectItem(const int index)
        {
            if(index == -1)
            {
                _view->selectItem(std::wstring::npos);
            }
            else
            {
                _view->selectItem(index);
            }
            _view->refresh();
        }

    };

	DefaultController _defaultController;
	DefaultModel      _defaultModel;
	DefaultItemView   _defaultItemView;
	ItemView        * _itemView;

public:
	View()
		:_itemView(&_defaultItemView)
	{
		_defaultController.setView(this);
		_model = &_defaultModel;
		Gui::registerClass(L"Gui::ListPane::View");
	}

	virtual void create(IView * parentView)
	{
		ControlBase::createBase(L"Gui::ListPane::View", parentView);
	}

	void setItemView(ItemView * itemView)
	{
		_itemView = itemView;
	}

	virtual void resize(void)
	{
        _model->recalculateItems();
	}

    virtual Dimension getMinSize() const
    {
        size_t height = 0;
        if(_model->getItemsCount())
        {
            IModel::Item item = _model->getItem(_model->getItemsCount() - 1);
            height = item.bottom;
        }

        return Dimension(0, height);
    }

    virtual void paint()
    {
        _model->recalculateItems();

        Gui::Graphics graphics(_hWnd);
        graphics.setBkColor(Gui::Colors::WINDOW);
        graphics.fillRect(getClientRect());

        RECT itemRect;
        itemRect.left   = 0;
        itemRect.top    = 0;
        itemRect.right  = static_cast<LONG>(_size.width);
        itemRect.bottom = 20;

        Gui::Font textFont = Gui::Font::createPointFont(8, L"Serif");
        graphics.setFont(textFont);

        if(_model)
        {
            for(size_t index = 0; index < _model->getItemsCount(); ++index)
            {
                IModel::Item item = _model->getItem(index);
                itemRect.top    = item.top;
                itemRect.bottom = item.bottom;

                drawItem(graphics, itemRect, index);
            }
        }
    }

    int getIndexUnderMouse(const POINT & point) const
    {
        if(_model)
        {
            for(size_t index = 0; index < _model->getItemsCount(); ++index)
            {
                IModel::Item item = _model->getItem(index);
                if((point.y >= item.top) && (point.y < item.bottom))
                {
                    return static_cast<int>(index);
                }
            }
        }
        return -1;
    }

    virtual void selectItem(size_t index)
    {
        _model->setSelection(index);
		refresh();
    }

protected:
    virtual void drawItem(Gui::Graphics & graphics, const RECT & rect, size_t index)
    {
		_itemView->draw(graphics, rect, _model->getItem(index));
    }
};

}};//namespace
