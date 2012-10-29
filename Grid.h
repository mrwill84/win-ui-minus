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
#include "WinUtils.h"
#include "MessageProcessor.h"
#include <vector>
#include <string>

namespace Gui { namespace Grid
{
	class IModel
	{
	public:
		struct Column
		{
			std::wstring name;
			size_t       width;
			Column(const std::wstring & name_, size_t width_)
				:name(name_), width(width_)
			{
			}
		};

		virtual ~IModel() {};

		virtual void         insertItem     (size_t row, const std::wstring & text = L""      ) = 0;
		virtual void         setItemText    (size_t row, size_t col, const std::wstring & text) = 0;
		virtual void         deleteItem     (size_t index                                     ) = 0;
		virtual std::wstring getItemText    (size_t row, size_t col                           ) = 0;
		virtual size_t       getSize        (void                                             ) = 0;
		virtual Column       getColumn      (size_t index                                     ) = 0;
		virtual size_t       getColumnsCount(void                                             ) = 0;
		virtual void         insertColumn   (size_t index, const Column & column              ) = 0;
		virtual void         deleteColumn   (size_t index                                     ) = 0;
		virtual void         cleanup        (void                                             ) = 0;
	};

    class DefaultModel : public IModel
	{
	protected:
		std::vector<Column>                     _columns;
		std::vector<std::vector<std::wstring> > _grid;
	public:
		virtual void insertItem(size_t row, const std::wstring & text = L"")
		{
			std::vector<std::wstring> line;
			for(size_t index = 0; index < _columns.size(); ++index)
			{
				index == 0?line.push_back(text):line.push_back(L"");
			}
			_grid.insert(_grid.begin() + row, line);
		}

		virtual void setItemText(size_t row, size_t col, const std::wstring & text)
		{
            if(row == _grid.size())
            {
                insertItem(_grid.size());
            }
			_grid[row][col] = text;
		}
		virtual void deleteItem(size_t index)
		{
			_grid.erase(_grid.begin() + index);
		}
		virtual std::wstring getItemText(size_t row, size_t col)
		{
			return _grid[row][col];
		}
		virtual size_t getSize(void)
		{
			return _grid.size();
		}
		virtual Column getColumn(size_t index)
		{
			return _columns[index];
		}
		virtual size_t getColumnsCount(void)
		{
			return _columns.size();
		}
		virtual void insertColumn(size_t index, const Column & column)
		{
			_columns.insert(_columns.begin() + index, column);
			for(size_t rowIndex = 0; rowIndex < getSize(); ++rowIndex)
			{
				std::vector<std::wstring> line = _grid[rowIndex];
				line.insert(line.begin() + index, L"");
				_grid[rowIndex] = line;
			}
		}
		virtual void deleteColumn(size_t index)
		{
			_columns.erase(_columns.begin() + index);
		}

		virtual void cleanup(void)
		{
			_columns.clear();
			_grid.clear();
		}

	};

    class View : public ControlBase, 
                 public Modellable<IModel>
	{
	public:
	protected:
		DefaultModel  _defaultModel;
    public:
        View(void)
        {
            _model = &_defaultModel;
        }

		virtual void create(const HWND parentWindow)
		{
			createBase(L"SysListView32", parentWindow, WS_CHILD | WS_VISIBLE | WS_VSCROLL | LVS_REPORT);
            loadFromModel();
		}

        virtual WNDPROC subclass(const HWND hWnd)
        {
            WNDPROC oldProc = ControlBase::subclass(hWnd);
            ::SendMessage(_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, _exStyle | LVS_EX_FULLROWSELECT);
            loadFromModel();
            return oldProc;
        }

		virtual void insertColumn(size_t index, const IModel::Column & column)
		{
			insertColumnView(index, column.name, column.width);
			_model->insertColumn(index, column);
		}
 
		virtual void insertItem(size_t index, const std::wstring & text = L"")
		{
			_model->insertItem(index, text);
		}

		virtual void deleteItem(size_t index)
		{
			deleteItemView(index);
			_model->deleteItem(index);
		}

		virtual size_t getSize(void)
		{
			return _model->getSize();
		}

		virtual void setItemText(size_t index, size_t column, const std::wstring & text)
		{
			ListView_SetItemText(_hWnd, static_cast<int>(index), static_cast<int>(column), const_cast<wchar_t *>(text.c_str()));
			_model->setItemText(index, column, text);
		}

        size_t getSelectionIndex(void)
        {
            return ListView_GetSelectionMark(_hWnd);
        }

        size_t getSelectedCount(void)
        {
            return ListView_GetSelectedCount(_hWnd);
        }

        size_t getNextSelectedIndex(size_t start)
        {
            return ListView_GetNextItem(_hWnd, static_cast<int>(start), LVNI_SELECTED);
        }

		virtual void setModel(IModel * model)
		{
			_model = model;
			loadFromModel();
		}

		void loadFromModel(void)
		{
			cleanupView();

			for(size_t col = 0; col < _model->getColumnsCount(); ++col)
			{
				insertColumnView(col, _model->getColumn(col).name, _model->getColumn(col).width);
			}
			for(size_t row = 0; row < _model->getSize(); ++row)
			{
				insertItemView(row, _model->getItemText(row, 0));
				for(size_t col = 0; col < _model->getColumnsCount(); ++col)
				{
					setItemText(row, col, _model->getItemText(row, col));
				}
			}
		}

		void cleanupView(void)
		{
			size_t viewItemCount = ListView_GetItemCount(_hWnd);
			for(size_t row = 0; row < viewItemCount; ++row)
			{
				deleteItemView(0);
			}
            
            int viewColumnCount = 0;
            LVCOLUMN column = {0};
            column.mask = LVCF_WIDTH;
            
            while(ListView_GetColumn(_hWnd, viewColumnCount++, &column))
                ;

            if(viewColumnCount > 0) 
            {
                viewColumnCount--;
            }
			for(int col = 0; col < viewColumnCount; ++col)
			{
				deleteColumnView(0);
			}
		}

    private:
		void insertColumnView(size_t index, const std::wstring & text, size_t width)
		{
			LV_COLUMN lvc;
			memset(&lvc, 0, sizeof(LV_COLUMN) );
			lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.fmt = LVCFMT_LEFT;

			lvc.iSubItem   = 0;
			lvc.cx         = static_cast<int>(width);
			lvc.cchTextMax = 13;
			lvc.pszText    = const_cast<wchar_t *>(text.c_str()); 
			ListView_InsertColumn(_hWnd, static_cast<int>(index), &lvc);
		}

		void insertItemView(size_t index, const std::wstring & text = L"")
		{
			LVITEM item;
			memset(&item, 0, sizeof(LVITEM));
			item.mask  = LVIF_TEXT ; 
			item.iItem = static_cast<int>(index);
			item.pszText = const_cast<wchar_t *>(text.c_str());
			ListView_InsertItem(_hWnd, &item);
		}

		void deleteItemView(size_t index)
		{
			ListView_DeleteItem(_hWnd, index);
		}

		void deleteColumnView(size_t index)
		{
			ListView_DeleteColumn(_hWnd, index);
		}
	};
}}; // namespace