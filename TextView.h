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
#include "Graphics.h"
#include "WinUtils.h"
#include "MessageProcessor.h"
#include <vector>

template <typename T>
class Iteratable
{
public:
	virtual bool hasMore(void) = 0;
	virtual T    getNext(void) = 0;
	virtual void reset  (void) = 0;
};

struct Instruction
{
	enum Type
	{
		TEXT,
		NEXT_LINE
	};
	Instruction()
		:font(Gui::Fonts::DEFAULT())
	{
	}

	Type               type;
	std::wstring       text;
	Gui::Colors::Color color;
	Gui::Font          font;
};

typedef Iteratable<Instruction> ITextModel;

class TextModel : public ITextModel
{
private:
	typedef std::list<Instruction> Instructions;
	Instructions           _instructions;
	Instructions::iterator _it;
public:
	virtual bool hasMore(void)
	{
		return (_it != _instructions.end());
	}

	virtual Instruction getNext(void)
	{
		Instruction instruction = (*_it);
		++_it;
		return instruction;
	}

	virtual void reset(void)
	{
		_it = _instructions.begin();
	}

	//void initialize(X::Element & card)
	//{
	//	ContainerIterator<X::Property> it(card);
	//	while(!it.isLast())
	//	{
	//		X::Property property = (*it);
	//		Instruction instruction;
	//		instruction.type  = Instruction::TEXT;
	//		instruction.color = Gui::Colors::WINDOWTEXT;
	//		instruction.font  = Gui::Font::createPointFont(8, L"ARIAL", true);
	//		instruction.text = property.name;
	//		_instructions.push_back(instruction);

	//		instruction.font  = Gui::Fonts::DEFAULT();
	//		instruction.text = L" :  " + property.value;
	//		_instructions.push_back(instruction);

	//		instruction.type = Instruction::NEXT_LINE;
	//		_instructions.push_back(instruction);
	//		++it;
	//	}
	//	reset();
	//}
};


namespace Gui
{

	/**
	 *	Text View generic control
	 */
	class TextView : public ControlBase,
		             public Controller<TextView>
	{
	public:
		struct Char
		{
			Font          font;
			wchar_t       character;
			Colors::Color color;

			Char(const wchar_t ch)
				:font(0),
				 character(ch),
				 color(Colors::WINDOWTEXT)
			{
			}
		};

		/** Simple interface for text document */
		class IDocument
		{
		public:
			virtual void   getLine           (int lineNo, std::vector<Char> & characters) = 0;
			virtual int    getLineCount      (void) = 0;
			virtual size_t getLongestLineSize(    ) = 0;
			virtual ~IDocument() {};
		};
	private:
		IDocument   * _document;
		unsigned long _vScrollPos;
		unsigned long _hScrollPos;
		unsigned long _vScrollMax;
		unsigned long _hScrollMax;
		int           _windowColumnCount;
		int           _windowLineCount;
		int           _lineHeight;
		int           _fontWidth;
		Gui::Font     _courierFont;

	public:
		TextView()
			:_document(0),
			 _vScrollPos(0),
			 _hScrollPos(0),
			 _vScrollMax(0),
			 _hScrollMax(0),
			 _windowColumnCount(0),
			 _windowLineCount(0),
			 _lineHeight(0),
			 _fontWidth(0),
			 _courierFont(Gui::Font::createPointFont(10, L"Courier New"))
        {
			Gui::registerClass(L"Gui::TextView");
            setView(this);
        }

		virtual void create(const HWND parentWindow)
		{
			createBase(L"Gui::TextView", parentWindow, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL);
			//setupScrollbars();
		}

		void setDocument(IDocument * document)
		{
			_document = document;
		}

		virtual void setSize(int width, int height)
		{
			ControlBase::setSize(Dimension(width, height));
			::MoveWindow(_hWnd, 0, 0, width, height, TRUE);
			//setupScrollbars();
		}
		virtual bool onPaint(WPARAM wParam, LPARAM lParam)
		{
			Graphics graphics(_hWnd);
			graphics.setBkColor(Colors::WINDOW);
			graphics.fillRect(getClientRect());
			graphics.setFont(_courierFont);
			if(_document)
			{
				size_t firstLineNo = _vScrollPos + getClientRect().top  / graphics.getFontHeight();
				size_t lastLineNo  = (_document->getLineCount() < getMaxLines(graphics))?
					                        _document->getLineCount():getMaxLines(graphics);
				std::vector<Char> text;

				for(size_t lineNo = firstLineNo; lineNo < lastLineNo; lineNo++)
				{
					_document->getLine(lineNo, text);
					RECT rect;
					rect.left = 0;
					rect.top  = (lineNo - firstLineNo) * graphics.getFontHeight();
					rect.bottom = rect.top + graphics.getFontHeight();
					rect.right  = getClientRect().right;
					paintLine(graphics, rect, text);
				}
			}
			return true;
		}

		virtual bool onMessage(UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
				case WM_VSCROLL:
				{
					//onVScroll(LOWORD(wParam), HIWORD(wParam));
					break;
				}
				case WM_HSCROLL:
				{
					//onHScroll(LOWORD(wParam), HIWORD(wParam));
					break;
				}
				default:
					return false;
			}
			return true;
		}

	private:
		void repositionCaret(void)
		{
		}

		void paintLine(Graphics & graphics, RECT & rect, std::vector<Char> & text)
		{
			//graphics.fillRect(rect);
			size_t x = rect.left;
			for(size_t i = 0; i < text.size(); i++)
			{
				Char character = text.at(i);
				//graphics.setFont(character.font);
				//graphics.setTextColor(character.color);
				int charWidth = 0;
				::GetCharWidth32(graphics, character.character, character.character, &charWidth);
				if(character.character == L'\t')
				{
					x += 4*charWidth;//TABWITH;
				}
				else if(character.character >= L' ')
				{
					wchar_t buff[2] = {character.character, '\0'};
					graphics.text(x, rect.top, buff);
					x += charWidth;
				}
				if(x >= rect.right)
				{
					break;
				}
			}
		}

		int getMaxLines(Graphics & graphics)
		{
			return _size.height / graphics.getFontHeight();
		}
private:
	void paintCard(Gui::Graphics & graphics)
	{
		//Gui::Point point(10, 10);
		//if(_model)
		//{
		//	_model->reset();
		//	while(_model->hasMore())
		//	{
		//		Instruction instruction = _model->getNext();
		//		point = processInstruction(graphics, instruction, point);
		//	}
		//}
	}

	Gui::Point processInstruction(Gui::Graphics & graphics, const Instruction & instruction, const Gui::Point & point)
	{
		Gui::Point updatedPoint = point;
		if(instruction.type == Instruction::TEXT)
		{
			graphics.setTextColor(instruction.color);
			graphics.setFont(instruction.font);
			graphics.text(updatedPoint.x     , updatedPoint.y, instruction.text);
			SIZE sz;
			::GetTextExtentPoint32(graphics, instruction.text.c_str(), instruction.text.size(), &sz);
			updatedPoint.x += sz.cx + 4;
		}
		if(instruction.type == Instruction::NEXT_LINE)
		{
			updatedPoint.y += 20;
			updatedPoint.x  = 10;
		}
		return updatedPoint;
	}

	};
}; // namespace
