/*
 * Copyright (c) 2006-2012, Coderminus.com
 * All rights reserved.
 *
 * License: http://en.wikipedia.org/wiki/MIT_License
 */

#pragma once

#include "MessageProcessor.h"

namespace Gui
{
	/**
	 *	Base class for controller
	 */
	template<typename T>
	class Controller : public MessageProcessor,
 	   				   public IMouseListener,
					   public IMouseMoveListener,
					   public ICommandListener,
					   public IKeyboardListener,
					   public IMessageListener,
                       public IFocusListener, 
                       public IAsyncUpdateListener,
                       public IScrollListener,
                       public INotifyListener

	{
	protected:
		T * _view;
	public:
		Controller()
			:_view(0)
		{
			setMouseListener      (this);
			setMouseMoveListener  (this);
			setCommandListener    (this);
			setKeyboardListener   (this);
			setMessageListener    (this);
            setFocusListener      (this);
            setAsyncUpdateListener(this);
            setScrollListener     (this);
            setNotifyListener     (this);
		}

		/* IMouseListener implementation */
		virtual bool onRButtonDown       (WPARAM wParam, LPARAM lParam)	{ return false;	}
		virtual bool onRButtonUp         (WPARAM wParam, LPARAM lParam) { return false; }
		virtual bool onLButtonDown       (WPARAM wParam, LPARAM lParam) { return false; }
		virtual bool onLButtonUp         (WPARAM wParam, LPARAM lParam) { return false; }		
		virtual bool onMouseWheel        (WPARAM wParam, LPARAM lParam) { return false; }
		virtual bool onLButtonDoubleClick(WPARAM wParam, LPARAM lParam) { return false; }
		/* IKeyboardListener implementation */
		virtual bool onKeyDown           (WPARAM wParam, LPARAM lParam) { return false; }
		virtual bool onKeyUp             (WPARAM wParam, LPARAM lParam) { return false; }
		virtual bool onChar              (WPARAM wParam, LPARAM lParam) { return false; }

		virtual bool onMouseMove         (int x, int y,  WPARAM wParam) { return false; }
        virtual bool onSetFocus          (void                        ) { return false; }
        virtual bool onKillFocus         (void                        ) { return false; }

		virtual bool onCommand           (const Command & command     ) { return false; }
		virtual bool onMessage           (UINT message, WPARAM wParam, LPARAM lParam) { return false; }
		virtual bool onVScroll           (WPARAM wParam, LPARAM lParam) { return false; }
		virtual bool onHScroll           (WPARAM wParam, LPARAM lParam) { return false; }
        virtual bool onNotify            (WPARAM wParam, LPARAM lParam) { return false; }
        virtual bool onAsyncUpdate       ()
        {
            _view->refresh();
            return false;
        }

		virtual void setView(T * view)
		{
			_view = view;
			if(_view)
			{
				_view->setController(this);
			}
		}

        virtual T * getView(void) { return _view; }
	};
};
