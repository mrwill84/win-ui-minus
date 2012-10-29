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

#include "../ControlBase.h"
#include "../Graphics.h"
#include "../Scrollable.h"
#include "../Modellable.h"
#include "../Menu.h"
#include "../IScrollable.h"

#include "DefaultModel.h"

namespace Gui { namespace Graph 
{

class View : public ControlBase, 
             public Modellable<IModel>
{
public:
    class DefaultController : public Controller<View>
    {
    protected:
        HCURSOR _dragCursor;
        int     _draggedNodeIndex;
        int     _selectedNode;
    public:
        DefaultController()
            :_draggedNodeIndex(-1), _selectedNode(-1)
        {
        }

	    virtual void setView(View * view)
	    {
		    Gui::Controller<View>::setView(view);
            _dragCursor = ::LoadCursor(NULL, IDC_SIZEALL);
	    }

	    virtual bool onPaint(WPARAM wParam, LPARAM lParam)
        { 
            _view->paint();
            return true;
        }

        virtual bool onLButtonDown(WPARAM wParam, LPARAM lParam)
	    {
		    ::ReleaseCapture();
            Point pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            int index = _view->getNodeUnderMouse(pt);

            selectNode(index);

            if(::DragDetect(_view->getHandle(), pt) && ((_draggedNodeIndex = _view->getNodeUnderMouse(pt)) != -1))
            {
                _view->beginDrag(pt.x, pt.y, _draggedNodeIndex);
                ::SetCapture(_view->getHandle());
                ::SetCursor(_dragCursor);
            }
            
		    return true;
	    }

        virtual bool onLButtonUp(WPARAM wParam, LPARAM lParam)
	    {
		    ::ReleaseCapture();
            if(_draggedNodeIndex != -1)
            {
                POINT pt;
                pt.x = (static_cast<short>(LOWORD(lParam)) > 0)?LOWORD(lParam):0;
                pt.y = (static_cast<short>(HIWORD(lParam)) > 0)?HIWORD(lParam):0;

                _view->setNodePoint(_draggedNodeIndex, Point(pt.x - 16, pt.y - 16));
                _view->endDrag();
                _view->refresh();
            }
            _draggedNodeIndex = -1;
            return true;
        }

        virtual bool onMouseMove(int x, int y, WPARAM wParam)
	    {
		    if((wParam & MK_LBUTTON) && ::GetCapture() == _view->getHandle())
		    {
                _view->moveDrag(x, y);
		    }
            else
            {
                Point pt;
                pt.x = x;
                pt.y = y;
                _view->setNodeUnderMouse(pt);
            }
		    return true;
	    }

        virtual bool onKeyDown(WPARAM wParam, LPARAM lParam) 
        { 
            int virtualCode = static_cast<int>(wParam);
            int state       = static_cast<int>(lParam);

            switch(virtualCode)
            {
                case VK_UP:
                {
                    return onKeyUp();
                }
                case VK_DOWN:
                {
                    return onKeyDown();
                }
                case VK_LEFT:
                {
                    return onKeyLeft();
                }
                case VK_RIGHT:
                {
                    return onKeyRight();
                }
            }

            return false; 
        }

        void selectNode(const int index)
        {
            if(index == -1)
            {
                _view->getModel()->selectNode(std::wstring::npos);
            }
            else
            {
                _view->getModel()->selectNode(index);
            }
            _selectedNode = index;
            _view->refresh();
        }
    protected:
        bool onKeyUp()
        {
            _view->getModel()->shiftSelectedNodeUp();
            _view->refresh();
            return false;
        }
        bool onKeyDown()
        {
            _view->getModel()->shiftSelectedNodeDown();
            return false;
        }
        bool onKeyLeft()
        {
            _view->getModel()->shiftSelectedNodeLeft();
            return false;
        }
        bool onKeyRight()
        {
            _view->getModel()->shiftSelectedNodeRight();
            return false;
        }

    };

protected:
    ImageList              _imageList;
    std::map<HBITMAP, int> _bitmapToIndex;
    DefaultController      _defaultController;
    DefaultModel           _defaultModel;
    int                    _nodeUnderMouse;
    Gui::IScrollable     * _scrollable;

public:
    View()
        :_imageList(32, 32), 
         _nodeUnderMouse(-1),
         _scrollable(0)
    { 
        Gui::registerClass(L"Gui::Graph::View"); 
        _model = &_defaultModel;
        _defaultController.setView(this);
    }

    virtual void create(const HWND parentWindow) 
    {
        createBase(L"Gui::Graph::View", parentWindow);
    }

	virtual void setPosition(const Point & point, const Dimension & size = Dimension())
	{
		Dimension correctedSize = size;
		if(correctedSize.height < getMinHeight())
		{
			correctedSize.height = getMinHeight();
		}
		if(correctedSize.width < getMinWidth())
		{
			correctedSize.width = getMinWidth();
		}
		
		ControlBase::setPosition(point, correctedSize);
	}

    virtual size_t getMinHeight() const
    {
        size_t height = 0;
        for(size_t index = 0; index < _model->getNodesCount(); ++index)
        {
            IModel::Node node = _model->getNode(index);
            if((node.point.y + 64) > static_cast<int>(height))
            {
                height = node.point.y + 64;
            }
        }
        return height;
    }
	
	virtual size_t getMinWidth() const
    {
        size_t width = 0;
        for(size_t index = 0; index < _model->getNodesCount(); ++index)
        {
            IModel::Node node = _model->getNode(index);
            if((node.point.x + 64) > static_cast<int>(width))
            {
                width = node.point.x + 64;
            }
        }
        return width;
    }

	virtual Dimension getMinSize(void) const
	{
		return Dimension(getMinWidth(), getMinHeight());
	}

    virtual void handleVerticalScroll(int scrollPos)
    {
    }

    virtual void paint()
    {
        Gui::Graphics graphics(_hWnd);
        graphics.setBkColor(Gui::Colors::WINDOW);
		graphics.fillRect(getClientRect());

        Gui::Font textFont = Gui::Font::createPointFont(8, L"Serif");
        graphics.setFont(textFont);

        for(size_t index = 0; index < _model->getNodesCount(); ++index)
        {
            IModel::Node node = getNodeForDrawing(index);
            addBitmap(node);
            drawBitmap(graphics, node);
            drawText(graphics, node);
        }
        
        Gui::Pen blackPen = Gui::Pen::create(PS_SOLID, 1, Gui::Colors::WINDOWTEXT);
        graphics.setPen(blackPen);
        for(size_t linkIndex = 0; linkIndex < _model->getLinksCount(); ++linkIndex)
        {
            IModel::Link & link = _model->getLink(linkIndex);
            drawLink(graphics, link);
        }
    }

    void beginDrag(int x, int y, size_t nodeIndex)
    {
        ImageList_BeginDrag(_imageList, _bitmapToIndex[_model->getNode(nodeIndex).bitmap], 0, 0);
        ImageList_DragEnter(_hWnd, x, y);
    }

    void moveDrag(int x, int y)
    {
        ImageList_DragMove(x - 16, y - 16);
    }

    void endDrag()
    {
        ImageList_DragLeave(0);
        ImageList_EndDrag();
        if(_scrollable)
        {
            _scrollable->resize();
        }
    }

    void setNodeUnderMouse(const Point & pt)
    {
        int nodeIndex = getNodeUnderMouse(pt);

        if(_nodeUnderMouse != nodeIndex)
        {     
            RECT rect;
            if(_nodeUnderMouse != -1)
            {
                rect = getNodeRect(_nodeUnderMouse);
                _nodeUnderMouse = nodeIndex;
                refresh(rect);
            }
            
            _nodeUnderMouse = nodeIndex;

            if(_nodeUnderMouse != -1)
            {
                rect = getNodeRect(_nodeUnderMouse);
                refresh(rect);
            }
        }
    }

    void refreshNode(size_t index)
    {
        RECT rect = getNodeRect(index);
        refresh(rect);
    }

    void setNodePoint(size_t index, const Point & point)
    {
        IModel::Node node = _model->getNode(index);
        node.point.x = point.x;
        node.point.y = point.y;// + _verticalScrollPos;
        _model->setNode(index, node);
    }

    int getNodeUnderMouse(const Point & point)
    {
        POINT pt = point;
        for(size_t index = 0; index < _model->getNodesCount(); ++index)
        {
            IModel::Node node = getNodeForDrawing(index);
            if(node.hasPoint(pt))
            {
                return static_cast<int>(index);
            }
        }
        return -1;
    }

    virtual void setScrollable(Gui::IScrollable * scrollable)
    {
        _scrollable = scrollable;
    }

protected:
    void addBitmap(const IModel::Node & node)
    {
        std::map<HBITMAP, int>::iterator it = _bitmapToIndex.find(node.bitmap);
        if(it == _bitmapToIndex.end())
        {
            _bitmapToIndex[node.bitmap] = _imageList.addImage(node.bitmap);
        }
    }

    RECT getNodeRect(size_t index)
    {
        IModel::Node node = getNodeForDrawing(index);
        RECT rect;
        rect.left   = node.point.x -  2;
        rect.top    = node.point.y -  2;
        rect.right  = node.point.x + 32;
        rect.bottom = node.point.y + 54;
        return rect;
    }

    virtual void drawLink(Graphics & graphics, const IModel::Link & link)
    {
        IModel::Node sourceNode = getNodeForDrawing(link.source);
        IModel::Node targetNode = getNodeForDrawing(link.target);

        graphics.line(sourceNode.point.x + 32, sourceNode.point.y + 16, 
                      targetNode.point.x     , targetNode.point.y + 16);
    }

    virtual void drawBitmap(Graphics & graphics, const IModel::Node & node)
    {
        Point point = node.point;
        if(_nodeUnderMouse == node.id)
        {
            point.x += -2;
            point.y += -2;
        }
        _imageList.drawImage(
            graphics, 
            _bitmapToIndex[node.bitmap], 
            point.x, 
            point.y, 
            32, 32);
    }

    virtual void drawText(Graphics & graphics, const IModel::Node & node)
    {
        graphics.text(
            node.point.x, 
            node.point.y + 33, 
            node.name);
    }
    
    IModel::Node getNodeForDrawing(const size_t index)
    {
        return _model->getNode(index);
    }

};
}}; //namespace