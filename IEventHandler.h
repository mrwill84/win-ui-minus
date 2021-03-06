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

namespace Gui
{
	////////////////////////////////////////////////////////////////////////////////////
	template<typename T_Event>
	class IHandler
	{
	public:
		virtual void handle(const T_Event & event) = 0;
	};

	template<typename T_Info>
	class Informator
	{
	private:
		IHandler<T_Info> * _handler;

	public:
		virtual void setHandler(IHandler<T_Info> * handler)	{ _handler = handler; }

		virtual void inform(const T_Info & info)
		{
			if(_handler)
			{
				_handler->handle(info);
			}
		}
	};

	///////////////////////////////////////////////////////////////////////////////////
	class Event 
	{
	public:
		std::wstring id;
		std::wstring text;
        HWND         wnd;
		Event(const std::wstring & id_, const std::wstring & text_ = L"", HWND hWnd = 0):text(text_), id(id_), wnd(hWnd){}
	};

	//////////////////////////////////////////////////////////////////////////////////
	typedef IHandler<Event> IEventHandler;
};