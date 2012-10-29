#pragma once

#include "../IView.h"

class MockView : public Gui::IView
{
protected:
	Gui::Dimension _size;
	Gui::Point     _position;
	bool           _isWindow;
public:
	virtual void create(const HWND parentWnd)
	{
		_isWindow = true;
	}

	virtual void destroy(void)
	{
		_isWindow = false;
	}

	virtual HWND getHandle(void) const
	{
		return 0;
	}
    
	virtual std::wstring getText(void) const
	{
		return L"MockView";
	}

    virtual void setPosition(const Gui::Point & point, const Gui::Dimension & size = Gui::Dimension())
	{
		_position = point;
		_size     = size;
	}

    virtual void setSize(const Gui::Dimension & size)
	{
		_size = size;
	}

    virtual void refresh(const Gui::Rect & rect = Gui::Rect::Null())
	{
	}

	virtual void show(bool isShow)
    {
    }
	
    virtual Gui::Dimension getDefaultSize(void) const
    {
        return Gui::Dimension();
    }

	virtual Gui::Dimension getMinSize(void) const
    {
        return Gui::Dimension();
    }

	virtual Gui::Dimension getSize(void) const
	{
		return _size;
	}

	virtual Gui::Point getPosition(void) const
	{
		return _position;
	}

	virtual bool isWindow(void) const
	{
		return _isWindow;
	}

	virtual Gui::Rect getRect(void) const
	{
		return Gui::Rect();
	}
};