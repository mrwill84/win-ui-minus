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
