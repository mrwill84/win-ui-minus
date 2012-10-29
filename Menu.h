/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include <windows.h>
#include <windowsx.h>

#include "Modellable.h"

namespace Gui
{
/**
 *	Menu item wrapper
 *	@version 1.0
 */
class MenuItem
{
protected:
	MENUITEMINFO _itemInfo;
public:
	MenuItem(const std::wstring & text, int id, HBITMAP bitmap = 0)
	{
		memset(&_itemInfo, 0, sizeof(MENUITEMINFO));
        _itemInfo.cbSize     = sizeof(MENUITEMINFO);
		_itemInfo.fMask      = MIIM_TYPE | MIIM_ID;
        if(bitmap)
        {
            _itemInfo.hbmpItem = bitmap;
        }
		_itemInfo.fType      = MFT_STRING;
		_itemInfo.wID        = id;
		_itemInfo.dwTypeData = const_cast<LPWSTR>(text.c_str());
		_itemInfo.cch        = static_cast<UINT>(text.size());	
	}

    MenuItem(const MENUITEMINFO & itemInfo)
        :_itemInfo(itemInfo)
    {
    }

	operator MENUITEMINFO * ()
	{
		return &_itemInfo;
	}
};

class MenuSeparator : public MenuItem
{
public:
	MenuSeparator()
		:MenuItem(L"-", -1) 
	{
		_itemInfo.fType = MFT_SEPARATOR;
	}
};

class SubMenuItem : public MenuItem
{
public:
	SubMenuItem(LPWSTR text, HMENU subMenu)
		:MenuItem(text, -1)
	{
		_itemInfo.fMask = MIIM_TYPE | MIIM_SUBMENU | MIIM_ID; 
		_itemInfo.hSubMenu = subMenu; 
	}
};

namespace Menu
{

struct Item 
{
    int  commandId;
    bool isEnabled;
    bool isChecked;

	//To update the menu item text during execution
	std::wstring text;
	bool  isTextNew;

    Item(int commandId_ = 0, bool isEnabled_ = true, bool isChecked_ = false)
        :commandId(commandId_), isEnabled(isEnabled_), isChecked(isChecked_),
		isTextNew(false)
    {
	}
};

class IModel
{
public:
    virtual size_t getItemsCount(void        ) const = 0;
    virtual Item   getItem      (size_t index) const = 0;
};

class View : public Modellable<IModel>
{
protected:
	HMENU      _menu;
public:
	View(void)
	{
		_menu = ::CreatePopupMenu();
	}

    View(HMENU menu)
        :_menu(menu)
    {
    }

	View(int resourceId)
	{
		_menu = ::LoadMenu(::GetModuleHandle(0), MAKEINTRESOURCE(resourceId));
	}

	operator HMENU (void)
	{
		return _menu;
	}

	void addMenuItem(MenuItem & menuItem)
	{
		int count = ::GetMenuItemCount(_menu);
		::InsertMenuItem(_menu, count, TRUE, menuItem);
	}

    void updateMenuItem(const Item & item)
    {
        MENUITEMINFO menuItem = {0};

        LONG  count = GetMenuItemCount(_menu);

        menuItem.cbSize = sizeof(MENUITEMINFO);
        menuItem.fMask = MIIM_SUBMENU|MIIM_ID ;

        for(LONG index = 0; index < count; ++index)
        {
            GetMenuItemInfo(_menu, index, TRUE, &menuItem);

            if(menuItem.hSubMenu)
            {
                View subMenu(menuItem.hSubMenu);
                subMenu.updateMenuItem(item);
            }
            else if(menuItem.wID == item.commandId)
            {
                menuItem.fMask = MIIM_STATE;
                menuItem.fState = menuItem.fState | (item.isChecked?MFS_CHECKED:MFS_UNCHECKED);
                menuItem.fState = menuItem.fState | (item.isEnabled?MFS_ENABLED:MFS_GRAYED);
                SetMenuItemInfo(_menu, index, TRUE, &menuItem);
                break;
            }
        }
    }

    void updateFromModel(void)
    {
        for(size_t index = 0; index < _model->getItemsCount(); ++index)
        {
            Item item = _model->getItem(index);
            updateMenuItem(item);
        }
    }

	int track(HWND hWnd)
	{
		return ::TrackPopupMenu(_menu, 
			             TPM_LEFTALIGN | TPM_RETURNCMD, 
						 GET_X_LPARAM(::GetMessagePos()), 
						 GET_Y_LPARAM(::GetMessagePos()),
						 0,
						 hWnd,
						 0);
	}
};

};//Menu namespace

};//Gui namespace
