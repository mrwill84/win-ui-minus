/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include "../ControlBase.h"

#include <commctrl.h>
#include <vector>
#include <map>

#include "../ILayout.h"
#include "../WinUtils.h"

namespace Gui
{
    class Action
    {
    public:
        int command;
    public:
        Action(WPARAM wParam, LPARAM lParam)
          :command(-1)
        {
            if(HIWORD(wParam) == BN_CLICKED)
            {
                command = 0;
            }
        }
    };
    
    class Tooltip : public ControlBase
    {
    protected:
        TOOLINFO _toolinfo;
    public:
        Tooltip()
            :ControlBase()
        {}
        virtual void create(const HWND parentWindow)
        {
            createBase(
                TOOLTIPS_CLASS, 
                parentWindow, 
                WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, L"", WS_EX_TOPMOST);
            ::SetWindowPos(_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }

        virtual void addTool(IView * view, const wchar_t * textOrCallback = LPSTR_TEXTCALLBACK)
        {
            /* INITIALIZE MEMBERS OF THE TOOLINFO STRUCTURE */
            _toolinfo.cbSize      = sizeof(TOOLINFO);
            _toolinfo.uFlags      = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE /*TTF_SUBCLASS*/;
            _toolinfo.hwnd        = view->getHandle();
            _toolinfo.hinst       = ::GetModuleHandle(0);
            _toolinfo.uId         = (UINT)(view->getHandle());
            _toolinfo.lpszText    = const_cast<wchar_t*>(textOrCallback);
            // ToolTip control will cover the whole window
            _toolinfo.rect.left   = 0;//rect.left;    
            _toolinfo.rect.top    = 0;//rect.top;
            _toolinfo.rect.right  = 0;//rect.right;
            _toolinfo.rect.bottom = 0;//rect.bottom;

            /* SEND AN ADDTOOL MESSAGE TO THE TOOLTIP CONTROL WINDOW */
            ::SendMessage(_hWnd, TTM_ADDTOOL, 0, (LPARAM)&_toolinfo);
        }

        virtual void activate(bool isActive)
        {
            ::SendMessage(_hWnd, TTM_TRACKACTIVATE,(WPARAM)isActive?TRUE:FALSE, (LPARAM)&_toolinfo);
        }
    };
    
    class Button : public ControlBase
    {
	private:
		int _actionId;
    public:
		Button(const std::wstring & text, int actionId = 0)
			:ControlBase(), _actionId(actionId)
        {
			_text = text;
        }

        #pragma warning( push )
        #pragma warning( disable : 4312 4311)
		virtual void create(const HWND parentWindow)
		{
            _hWnd = ::CreateWindow(WC_BUTTON, _text.c_str(), BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD,
                                   0, 0, 40, 20, parentWindow, reinterpret_cast<HMENU>(_actionId), ::GetModuleHandle(0), 0);
			SetWindowFont(_hWnd, GetStockFont(DEFAULT_GUI_FONT), FALSE);
		}
        #pragma warning( pop ) 
    };

	class Edit : public ControlBase
	{
    public:
        class DefaultController : public Controller<Edit>
        {
        public:
        };

    protected:
        DefaultController _defaultController;
    public:
		Edit(const std::wstring & text = L"") 
            :ControlBase()
        {
            _defaultController.setView(this);
            setText(text);
        }
		Edit(HWND hWnd) 
            :ControlBase()
        { _hWnd = hWnd;}

		virtual void create(const HWND parentWindow)
		{
			createBase(WC_EDIT, parentWindow, WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL);
		}

		virtual std::wstring & getText(void)
		{
			int maxCount = ::GetWindowTextLength(_hWnd);
			std::auto_ptr<wchar_t> buff(new wchar_t[maxCount + 1]);
			::GetWindowText(_hWnd, buff.get(), maxCount + 1);
			_text = buff.get();
			return _text;
		}

		virtual void selectText(size_t begin, size_t end)
		{
			::SendMessage(_hWnd, EM_SETSEL, static_cast<int>(begin), static_cast<int>(end));
		}
	};


	class ComboBox : public ControlBase
	{
	public:
		ComboBox(const std::wstring & text)
            :ControlBase()
        {setText(text);}

        virtual void addString(const std::wstring & text)
		{
			ComboBox_AddString(_hWnd, text.c_str());
		}

		virtual void create(const HWND parentWindow)
		{
			createBase(
                WC_COMBOBOX, 
                parentWindow, 
                CBS_DROPDOWNLIST | CBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_CHILD | WS_VISIBLE);
		}

        virtual void cleanup(void)
        {
            ComboBox_ResetContent(_hWnd);
        }

        virtual void setCurrentSelection(int currentSelection)
        {
            ComboBox_SetCurSel(_hWnd, currentSelection);
        }
	};

	class Label : public ControlBase
	{
	public:
        Label(const std::wstring & text = L""):ControlBase() {setText(text);}

		virtual void create(const HWND parentWindow)
		{
			createBase(WC_STATIC, parentWindow, WS_CHILD|WS_VISIBLE|SS_LEFT);
		}
	};

	/////////////////////////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////////////////////
	/**
	 *	List Box wrapper
	 */
	class List : public ControlBase
	{
	public:
		class IModel
		{
		public:
			virtual size_t       getSize     ()                          = 0;
			virtual void         addString   (const std::wstring & text) = 0;
			virtual std::wstring getString   (size_t index)              = 0;
			virtual void         deleteString(size_t index)              = 0;
		};

		class DefaultModel : public IModel, public std::vector<std::wstring>
		{
		public:
			virtual size_t       getSize     ()                         {return size();}
			virtual void         addString   (const std::wstring & text){push_back(text);}
			virtual std::wstring getString   (size_t index)             
			{
				if((index < 0) || (index >= size())) return L"";
				return operator[](index);
			}
			virtual void         deleteString(size_t index)	            {erase(begin() + index);}
		};

	protected:
		IModel     * _model;
		DefaultModel _defaultModel;

	public:
        List(IModel * model = 0)
			:ControlBase(), _model(model)
        {
			if(!_model)
			{
				_model = &_defaultModel;
			}
        }

		void setModel(IModel * model)
		{
			_model = model;
			loadFromModel();
		}

		virtual void create(const HWND parentWindow)
		{
			createBase(WC_LISTBOX, parentWindow, WS_CHILD | WS_VISIBLE | WS_VSCROLL |LBS_NOTIFY);
			loadFromModel();
		}

		IModel * getModel(void)
		{
			return _model;
		}

		void addString(const std::wstring & text)
		{
			_model->addString(text);
			ListBox_AddString(_hWnd, text.c_str());
		}

		int getSelectedIndex()
		{
			return ListBox_GetCurSel(_hWnd);
		}

		void loadFromModel(void)
		{
            while(ListBox_GetCount(_hWnd))
            {
                ListBox_DeleteString(_hWnd, 0);
            }

			for(size_t index = 0; index < _model->getSize(); ++index)
			{
				ListBox_AddString(_hWnd, _model->getString(index).c_str());
			}
		}
	};

	
	/**
	 *	Statusbar wrapper
	 */
	class Statusbar : public ControlBase
	{
    public:
        Statusbar():ControlBase(){}

		virtual void create(const HWND parentWindow)
		{
			createBase(STATUSCLASSNAME, parentWindow);
		}
	};

	/**
	 *	Tree control wrapper
	 */
    namespace Tree
    {
        struct Node
        {
            std::wstring name;
            HTREEITEM    handle;//temporary for win32
            Node(const std::wstring & name_, HTREEITEM handle_ = TVI_ROOT)
                :name(name_), handle(handle_) {}
            static Node Root()
            {
                Node rootNode(L"");
                return rootNode;
            }
        };

        class IModel
        {
        public:
            virtual size_t getChildNodesCount(const Node & node) const = 0;
            virtual Node   getChildNode      (size_t index     ) const = 0;
            virtual void   insertNode        (const Node & node, 
                                              const Node & parent)     = 0;
        };

        class DefaultModel : public IModel
        {
        private:
            std::vector<Node> _nodes;
        public:
            virtual size_t getChildNodesCount(const Node & node) const
            {
                return _nodes.size();
            }

            virtual Node getChildNode(size_t index) const
            {
                return _nodes[index];
            }

            virtual void insertNode(const Node & node, const Node & parent)
            {
                _nodes.push_back(node);
            }
        };

        class View : public ControlBase
	    {
        private:
            IModel * _model;
        public:
            View()
                :ControlBase(), _model(0)
            {}

		    virtual void create(const HWND parentWindow)
		    {
			    createBase(WC_TREEVIEW, parentWindow);
                if(_model)
                {
                    loadFromModel();
                }
		    }

            void setModel(IModel * model)
            {
                _model = model;
                if(_hWnd)
                {
                    loadFromModel();
                }
            }

            void loadFromModel(void)
            {
                size_t size = _model->getChildNodesCount(Node::Root());
                for(size_t index = 0; index < size; ++index)
                {
                    insertItem(_model->getChildNode(index).name, Node::Root());
                }
            }

            void insertItem(const std::wstring & text, const Node & parentNode)
		    {
			    TVINSERTSTRUCT item = {0};
			    item.hParent      = parentNode.handle;
			    item.hInsertAfter = TVI_LAST;
			    item.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_CHILDREN;
			    item.item.iImage  = 0;
			    item.item.pszText = const_cast<wchar_t*>(text.c_str());

			    TreeView_InsertItem(_hWnd, &item);
		    }
	    };

        class DefaultController : public Controller<View>
        {
        protected:
            IModel     * _model;
            DefaultModel _defaultModel;
        public:
	        virtual void setView(View * view)
	        {
                Controller<View>::setView(view);
                _view->setModel(&_defaultModel);
	        }

            virtual void insertNode(const Node & node, const Node & parentNode)
            {
                _model->insertNode(node, parentNode);
                _view->insertItem(node.name, parentNode);
            }
        };
   };//namespace Tree
}