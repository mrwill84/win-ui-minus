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

#include "DefaultModel.h"
#include "../ControlBase.h"
#include "../ListPane.h"
#include "../Controls/Controls.h"

namespace Gui { namespace Properties 
{

class IEventHandler
{
public:
    virtual void onBrowseButton(void) = 0;
};

class View : public Gui::ListPane::View, public IEventHandler
{
    static const int MIN_NAME_SIZE = 30;
public:
    static const int BROWSE_BUTTON_ACTION = 11001;
    class DefaultController : public Gui::Controller<View>
    {
    private:
        int             _divTop;
        int             _divBtm;
        int             _oldDivX;
        HCURSOR         _sizeCursor;
        IEventHandler * _eventHandler;
        std::wstring    _tooltipsText;
        bool            _isTrackingMouse;

    public:
        DefaultController()
            :_eventHandler(0),
             _isTrackingMouse(false)
        {
            _sizeCursor = ::LoadCursor(NULL, IDC_SIZEWE);
        }

        virtual bool onAsyncUpdate()
        {
            _view->refresh();
            return false;
        }

        virtual bool onPaint(WPARAM wParam, LPARAM lParam)
        { 
            _view->paint();
            return true;
        }

        virtual bool onKeyDown(WPARAM wParam, LPARAM lParam)
        {
            int virtualCode = static_cast<int>(wParam);
            int state       = static_cast<int>(lParam);

            switch(virtualCode)
            {
                case VK_TAB:
                {
                    return onTab(wParam, lParam);
                    break;
                }
            }
            return false;
        }

        bool onCommand(const Gui::Command & command)
        {
            if(command.event == Gui::Command::BUTTON)
            {
                if(command.id == Gui::Properties::View::BROWSE_BUTTON_ACTION)
                {
                    if(_eventHandler)
                    {
                        _eventHandler->onBrowseButton();
                    }
                }
            }
            // HACK: this assumes this Property view will be created in a dialog,
            // and that the TAB key will kill the edit focus (WS_TABSTOP style)
            else if (command.event == EN_KILLFOCUS)
            {
                bool isTab = (HIWORD(GetKeyState(VK_TAB)) > 0);
                bool isShift = (HIWORD(GetKeyState(VK_SHIFT)) > 0);
				if (isTab)
                {
                    size_t itemCount = _view->getPropertiesModel()->getPropertiesCount();
                    int selection = static_cast<int>(_view->getPropertiesModel()->getSelection());
                    if (isShift)
                    {
                        if (--selection > static_cast<int>(itemCount))
                        {   // shift-tab back beyond item 0 - go to last item
                            selection = static_cast<int>(itemCount) - 1;
                        }
                    }
                    else
                    {
                        if (++selection >= static_cast<int>(itemCount))
                        {   // tab beyond last item - go back to first item
                            selection = 0;
                        }
                    }
                    selectItem(selection);
                }
            }
            return true;
        }

        virtual bool onSetFocus(void)
        {
            _view->activateTooltips();
            return false;
        }

        virtual bool onMouseMove(int x, int y, WPARAM wParam)
        {	
	        if (::GetCapture() == _view->getHandle())
	        {
		        RECT clientRect = _view->getClientRect();
		        clientRect.left += 10; clientRect.right -= 10;
                if((x > (clientRect.left + MIN_NAME_SIZE + 16)) && (x < (clientRect.right - MIN_NAME_SIZE)))
                {
		            //move divider line to the mouse pos. if columns are
		            //currently being resized
					HDC hDC = ::GetDC(_view->getHandle());
		            //remove old divider line
		            invertLine(hDC, _oldDivX, _divTop, _oldDivX, _divBtm);
		            //draw new divider line
		            invertLine(hDC, x, _divTop, x, _divBtm);
		            _oldDivX = x;
                    ::ReleaseDC(_view->getHandle(), hDC);
                }
	        }
	        else if ((x >= _view->getDividerPos() - 5) && (x <= _view->getDividerPos() + 5))
            {
		        ::SetCursor(_sizeCursor);
            }
            else
            {
                Gui::Point pt(x, y);
                POINT point = pt;
                int index = _view->getIndexUnderMouse(pt);
                if(index != -1)
                {
                    _tooltipsText = _view->getPropertiesModel()->getProperty(index).value;

                    ::MapWindowPoints(_view->getHandle(), 0, &point, 1);
                 
                    _view->activateTooltips();
                    ::SendMessage(_view->getTooltipHandle(),
                        TTM_TRACKPOSITION,
                        0, 
                        (LPARAM)MAKELPARAM(point.x + 15,
                        point.y + 15));
                }
                else
                {
                    _view->deactivateTooltips();
                }
            }
            
            if(!_isTrackingMouse)
            {
                TRACKMOUSEEVENT TrackMouseEventStruct;
                memset(&TrackMouseEventStruct,
                    0,
                    sizeof(TrackMouseEventStruct));
                TrackMouseEventStruct.cbSize =
                    sizeof(TrackMouseEventStruct);
                TrackMouseEventStruct.dwFlags = TME_LEAVE;
                TrackMouseEventStruct.hwndTrack = _view->getHandle();
                ::TrackMouseEvent(&TrackMouseEventStruct);
                _isTrackingMouse = true;
            }
            return 0;		
        }

        virtual bool onLButtonDown(WPARAM wParam, LPARAM lParam)
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
	        
            if((x >= _view->getDividerPos() - 5) && (x <= _view->getDividerPos() + 5))
	        {
		        //if mouse clicked on divider line, then start resizing
		        ::SetCursor(_sizeCursor);

		        RECT windowRect = _view->getWindowRect();
		        windowRect.left += 10; windowRect.right -= 10;
		        //do not let mouse leave the list box boundary
		        ::ClipCursor(&windowRect);
                
		        RECT clientRect = _view->getClientRect();

		        _divTop = clientRect.top;
		        _divBtm = clientRect.bottom;
		        _oldDivX = x;

                HDC hDC = ::GetDC(_view->getHandle());
		        invertLine(hDC, _oldDivX, _divTop, _oldDivX, _divBtm);
                ::ReleaseDC(_view->getHandle(), hDC);

		        //capture the mouse
		        SetCapture(_view->getHandle());
                selectItem(-1);
	        }
            else
            {
				Gui::Point pt(LOWORD(lParam), HIWORD(lParam));
				int index = _view->getIndexUnderMouse(pt);

				selectItem(index);
            }
            return false;
        }
        virtual bool onLButtonUp(WPARAM wParam, LPARAM lParam)
        {
	        if(::GetCapture() == _view->getHandle())
	        {
    	        ::ReleaseCapture();
                int x = LOWORD(lParam);

    	        ::ClipCursor(NULL);

		        HDC hDC = GetDC(_view->getHandle());
		        invertLine(hDC, _oldDivX, _divTop, _oldDivX, _divBtm);
                ::ReleaseDC(_view->getHandle(), hDC);

                RECT clientRect;
                ::GetClientRect(_view->getHandle(), &clientRect);
		        clientRect.left += 10; clientRect.right -= 10;
                if((x > (clientRect.left + MIN_NAME_SIZE + 16)) && (x < (clientRect.right - MIN_NAME_SIZE)))
                {
		            _view->setDividerPos(x);
                }
                else if(x < (clientRect.left + MIN_NAME_SIZE + 16))
                {
                    _view->setDividerPos(clientRect.left + MIN_NAME_SIZE + 16);
                }
                else if(x > (clientRect.right - MIN_NAME_SIZE))
                {
                    _view->setDividerPos(clientRect.right - MIN_NAME_SIZE);
                }
                
		        _view->refresh();
	        }
	        else
	        {
                _view->refresh();
	        }

            return false;
        }

        virtual void setEventHandler(IEventHandler * eventHandler)
        {
            _eventHandler = eventHandler;
        }

        virtual bool onMessage(UINT message, WPARAM wParam, LPARAM lParam)
        {
            if(message == WM_MOUSELEAVE)
            {
                _view->deactivateTooltips();
                _isTrackingMouse = false;
            }
            return false;
        }

        virtual bool onNotify(WPARAM wParam, LPARAM lParam)
        {
            NMHDR * pNotify = (NMHDR*)lParam;

            NMTTDISPINFO * lpttd;
            if(pNotify->code == TTN_GETDISPINFO)
            {
                if(!_tooltipsText.empty())
                {
                    lpttd = (LPNMTTDISPINFO)pNotify;
                    ::SendMessage(pNotify->hwndFrom, TTM_SETMAXTIPWIDTH, 0, 300);
                    lpttd->lpszText = const_cast<wchar_t*>(_tooltipsText.c_str());
                }
            }

            return false;
        }

    protected:
        bool onTab(WPARAM wParam, LPARAM lParam)
        {
            selectItem(0);
            return true;
        }

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

        void invertLine(HDC hDC, int xFrom, int yFrom, int xTo, int yTo)
        {
	        int oldMode = SetROP2(hDC, R2_NOT);
            
            ::MoveToEx(hDC, xFrom, yFrom, NULL);
            ::LineTo(hDC, xTo, yTo);

	        SetROP2(hDC, oldMode);
        }
    };

protected:
    DefaultController       _defaultController;
    DefaultModel            _defaultModel;
    int                     _dividerPos;
    std::vector<Gui::Edit*> _valueEdits;
	IPropertiesModel      * _propertiesModel;
    Gui::Button             _button;
    IButtonHandler        * _buttonHandler;
    Tooltip                 _tooltip;
public:
    View()
        :_dividerPos(100),
         _propertiesModel(0),
         _button(L"...", BROWSE_BUTTON_ACTION)
    {
        _model = &_defaultModel;
		_propertiesModel = &_defaultModel;
        _defaultController.setEventHandler(this);
        _defaultController.setView(this);
    }

    ~View()
    {
        for(size_t index = 0; index < _valueEdits.size(); ++index)
        {
            delete _valueEdits[index];
        }
    }

    virtual void create(const HWND parentWnd)
    {
        Gui::ListPane::View::create(parentWnd);
        _tooltip.create(_hWnd);
        initTooltip();
        _button.create(_hWnd);
        _button.show(false);
    }

    void initTooltip()
    {
        _tooltip.addTool(this);
    }

    void activateTooltips(void)
    {
        _tooltip.activate(true);
    }

    void deactivateTooltips(void)
    {
        _tooltip.activate(false);
    }

    HWND getTooltipHandle() 
    {
        return _tooltip.getHandle();
    }
    virtual void destroy()
    {
        for(size_t index = 0; index < _valueEdits.size(); ++index)
        {
            if(::IsWindowVisible(_valueEdits[index]->getHandle()))
            {
                if(_propertiesModel)
                {
                    Property property = _propertiesModel->getProperty(index);
                    _propertiesModel->setValue(index, _valueEdits[index]->getText());
                }
            }
            _valueEdits[index]->destroy();
        }
        _button.destroy();
        Gui::ControlBase::destroy();
    }

    void addProperty(const Property & property)
    {
        _propertiesModel->addProperty(property);
    }

	IPropertiesModel * getPropertiesModel(void)
	{
		return _propertiesModel;
	}

    void setPropertiesModel(IPropertiesModel * model)
    {
        _model = model;
        _propertiesModel = model;
    }

    void clear()
    {
        if(_propertiesModel)
        {
            _propertiesModel->clear();
        }
    }

    int getDividerPos() const
    {
        return _dividerPos;
    }

    void setDividerPos(const int pos)
    {
		if(pos < (MIN_NAME_SIZE + 16))
		{
			_dividerPos = (MIN_NAME_SIZE + 16);
		}
		else if(pos > (static_cast<int>(_size.width) - MIN_NAME_SIZE))
		{
			_dividerPos = static_cast<int>(_size.width) - MIN_NAME_SIZE;
		}
		else
		{
			_dividerPos = pos;
		}
    }

	virtual void setPosition(const Point & point, const Dimension & size = Dimension())
	{
        selectItem(std::wstring::npos);
		Dimension correctedSize = size;
		if(correctedSize.height < getMinSize().height)
		{
			correctedSize.height = getMinSize().height;
		}
		if(_size.width)
		{
			int dx = (static_cast<int>(size.width) - static_cast<int>(_size.width))/2;
			if(dx)
			{
				setDividerPos(_dividerPos + dx);
				refresh();
			}
		}
		ControlBase::setPosition(point, correctedSize);
	}

    virtual void saveToModel()
    {
        for(size_t index = 0; index < _valueEdits.size(); ++index)
        {
            if(::IsWindowVisible(_valueEdits[index]->getHandle()))
            {
                Property property = _propertiesModel->getProperty(index);
                _propertiesModel->setValue(index, _valueEdits[index]->getText());
            }
        }
    }

    virtual void selectItem(size_t index)
    {
        ListPane::View::selectItem(index);
        for(size_t index_ = 0; index_ < _propertiesModel->getPropertiesCount(); ++index_)
        {
            if(index_ == _valueEdits.size())
            {
                Gui::Edit * edit = new Gui::Edit();
                edit->create(_hWnd);
                edit->show(false);
                _valueEdits.push_back(edit);
            }

            if(index == index_)
            {
                Property property = _propertiesModel->getProperty(index_);
                if(property.isButton)
                {
                    _button.setPosition(Gui::Point(static_cast<int>(_size.width) - 20, property.listItem.top - 2), 
                        Gui::Dimension(
                        20, 
                        property.listItem.bottom - property.listItem.top + 1));
                    _button.show(true);
                    _button.setFocus();
                }
                else
                {
                    _button.show(false);
                    _valueEdits[index_]->setText(property.value);
                    _valueEdits[index_]->setPosition(Gui::Point(_dividerPos + 2, property.listItem.top), 
                        Gui::Dimension(
                        _size.width - _dividerPos - 2, 
                        property.listItem.bottom - property.listItem.top - 2));
                    _valueEdits[index_]->show(true);
                    _valueEdits[index_]->setFocus();
                }
            }
            else
            {
                if(_valueEdits[index_]->isWindow())
                {
                    _propertiesModel->setValue(index_, _valueEdits[index_]->getText());
                }
                _valueEdits[index_]->show(false);
            }
        }
        if(index == std::wstring::npos)
        {
            _button.show(false);
        }
    }


    virtual void onBrowseButton()
    {
        if(_buttonHandler)
        {
            _buttonHandler->onButtonPressed();
        }
    }

    virtual void setButtonHandler(IButtonHandler * buttonHandler)
    {
        _buttonHandler = buttonHandler;
    }
protected:
    virtual void drawItem(Gui::Graphics & graphics, const RECT & rect, size_t index)
    {
        Gui::Font textFont = Gui::Font::createPointFont(8, L"Serif");
        graphics.setFont(textFont);

		Property property = _propertiesModel->getProperty(index);
        RECT nameRect    = rect;
        nameRect.right   = nameRect.left + _dividerPos;
		nameRect.left    = 16;
        nameRect.bottom -= 1;
  
		RECT barRect = rect;
		barRect.right = barRect.left + 16;

		Gui::GdiHandleGuard<HBRUSH> brush = ::CreateSolidBrush(RGB(190, 190, 192)); 
        ::SetBkMode(graphics, TRANSPARENT);

        ::FillRect(graphics, &barRect, brush);

        ::DrawEdge(graphics, &nameRect, EDGE_SUNKEN, BF_BOTTOMRIGHT);

        RECT valueRect = rect;
        valueRect.left = nameRect.right + 1;
        ::DrawEdge(graphics, &valueRect, EDGE_SUNKEN, BF_BOTTOM);

		std::wstring name  = property.name;
        std::wstring value = property.value;

        if(property.listItem.isSelected)
        {
            graphics.setBkColor(Colors::MENUHILIGHT);
            nameRect.right -= 2;
            graphics.fillRect(nameRect);
        }
        nameRect.right -= 1;
        ::DrawText(graphics, name.c_str (), static_cast<int>(name.size ()), &nameRect , DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
        ::DrawText(graphics, value.c_str(), static_cast<int>(value.size()), &valueRect, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
    }
};

}};//namespace