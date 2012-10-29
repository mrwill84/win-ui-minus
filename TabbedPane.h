/*
 * Copyright (c) 2006-2007, Andry Korolyuk
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice, 
 *     this list of conditions and the following disclaimer.
 *   * Neither the name of the Andry Korolyuk nor the names of its contributors 
 *     may be used to endorse or promote products derived from this software 
 *     without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
 * THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "ControlBase.h"
#include <map>
#include <vector>

namespace Gui 
{
    class TabView : public ControlBase
	{
    private:
        ImageList * _imageList;
	public:
		TabView()
        {
        }

		virtual void create(const HWND parentWindow)
		{
			createBase(WC_TABCONTROL, parentWindow);
            TabCtrl_SetImageList(_hWnd, *_imageList);
		}

        virtual void setImageList(ImageList * imageList)
        {
            _imageList = imageList;
        }

        virtual void addTab(const std::wstring & text)
        {
            TCITEM item = {0};
            item.mask = TCIF_TEXT;
            item.pszText = const_cast<wchar_t*>(text.c_str());

            TabCtrl_InsertItem(_hWnd, TabCtrl_GetItemCount(_hWnd), &item);
        }

        virtual void removeTab(const size_t index)
        {
            TabCtrl_DeleteItem(_hWnd, index);
        }

        virtual int getSelectedTab()
        {
            return TabCtrl_GetCurSel(_hWnd);
        }

        virtual void setTabText(const size_t index, const std::wstring & text)
        {
            TCITEM item = {0};
            item.mask = TCIF_TEXT;
            item.pszText = const_cast<wchar_t*>(text.c_str());
            TabCtrl_SetItem(_hWnd, index, &item);
        }

        virtual void setTabImage(const size_t tabIndex, int imageIndex)
        {
            TCITEM item = {0};
            item.mask     = TCIF_IMAGE;
            item.iImage   = imageIndex;
            TabCtrl_SetItem(_hWnd, tabIndex, &item);
        }
    };

    namespace TabbedPane 
    {
	
    class View : public ControlBase
	{
        const static int TAB_HEIGHT = 20;
    public:
        class DefaultController : public Controller<View>
        {
        public:
            virtual bool onMessage(UINT message, WPARAM wParam, LPARAM lParam)
            {
                if(message == WM_NOTIFY)
                {
                    return onNotify(wParam, lParam);
                }
                return false;
            }

            virtual bool onSize(unsigned long x, unsigned long y)
            {
                _view->resize();
                return false;
            }
        protected:
            bool onNotify(WPARAM wParam, LPARAM lParam)
            {
                NMHDR * pHdr = (NMHDR*)lParam;
                if(pHdr->code == TCN_SELCHANGE)
                {
                    _view->resetSelectedTab();
                }

                return false;
            }
        };

    protected:
        TabView             _tabView;
        std::vector<IView*> _tabs;
        ImageList           _imageList;
        std::map<std::wstring, int>
                            _imageIdMap;
        DefaultController   _defaultController;
	public:
		View()
		{
            Gui::registerClass(L"Gui::TabbedPane::View");
            _defaultController.setView(this);
            _tabView.setImageList(&_imageList);
		}

		virtual void create(const HWND parentWindow)
		{
            createBase(L"Gui::TabbedPane::View", parentWindow);
            _tabView.create(_hWnd);
			
            for(size_t index = 0; index < _tabs.size(); ++index)
            {
                _tabs[index]->create(_hWnd);
                _tabView.addTab(_tabs[index]->getText());
            }
		}

        virtual void destroy()
        {
            for(size_t index = 0; index < _tabs.size(); ++index)
            {
                _tabs[index]->destroy();
            }
            _tabView.destroy();
            ControlBase::destroy();
        }

        virtual void resize()
        {
            _tabView.setPosition(Point(0, 0), Dimension(_size.width, TAB_HEIGHT));
            int selectedTab = _tabView.getSelectedTab();
            if(selectedTab != -1)
            {
                _tabs[selectedTab]->setPosition(
                    Point(0, TAB_HEIGHT), 
                    Dimension(_size.width, _size.height - TAB_HEIGHT)
                    );
            }
        }

        virtual void resetSelectedTab()
        {
            int selected = _tabView.getSelectedTab();
            for(size_t index = 0; index < _tabs.size(); ++index)
            {
                if(index == selected)
                {
                    _tabs[index]->show(true);
                    _tabs[index]->refresh();
                }
                else
                {
                    _tabs[index]->show(false);
                }
            }
            resize();
        }

        virtual Dimension getDefaultSize() const 
        { 
            if(_tabs.size())
            {
                return _tabs[0]->getDefaultSize();
            }
            return Dimension();
        }

        virtual void addTab(IView * tab)
        {
            _tabs.push_back(tab);
            _tabView.addTab(tab->getText());
            tab->setPosition(Point(0, 20), Dimension(_size.width, _size.height - 20));
            if(_tabs.size() > 1)
            {
                tab->show(false);
            }
            refresh();
        }

        virtual size_t getTabCount(void)
        {
            return _tabs.size();
        }

        virtual void removeTab(const size_t & index)
        {
            _tabView.removeTab(index);
            _tabs.erase(_tabs.begin() + index);
        }

        IView * getTab(size_t index)
        {
            return _tabs[index];
        }

        virtual void addImage(const std::wstring & imageId, const HBITMAP & bitmap)
        {
            int index = _imageList.addImage(bitmap);
            _imageIdMap[imageId] = index;
        }

        virtual void setTabImage(const size_t index, const std::wstring & imageId)
        {
            _tabView.setTabImage(index, _imageIdMap[imageId]);
        }

        virtual void refresh()
        {
            for(size_t index = 0; index < _tabs.size(); ++index)
            {
                _tabView.setTabText(index, _tabs[index]->getText());
            }
            ControlBase::refresh();
        }
    };	
    }//namespace
};