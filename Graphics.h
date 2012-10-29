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

#include <windows.h>
#include <Winuser.h>
#include <windef.h>
#include <commctrl.h>
#include <list>
#include <string>

namespace Gui
{
    template<typename T>
    class GdiHandleGuard
    {
    private:
        T    _handle;
        bool _isOwner;
    public:
        GdiHandleGuard():_handle(0), _isOwner(true){}
        GdiHandleGuard(const T & handle):_handle(handle), _isOwner(true){}
        GdiHandleGuard(GdiHandleGuard const & other):_handle(other._handle), _isOwner(false){}
        virtual ~GdiHandleGuard(){if(_isOwner) ::DeleteObject(_handle);}
        void operator =(const T & handle){_handle = handle;}
        operator      T(                ){ return _handle;}
    };

    class Caret
    {
    private:
        HWND _hWnd;
    public:
        void create(const HWND hWnd, size_t width = 1, size_t height = 12)
        {
            _hWnd = hWnd;
            ::CreateCaret(hWnd, 0, static_cast<int>(width), static_cast<int>(height));
        }

        void set(size_t x, size_t y)
        {
            ::SetCaretPos(static_cast<int>(x), static_cast<int>(y));
        }

        void show(void)
        {
            ::ShowCaret(_hWnd);
        }
    };

	class Font
	{
	private:
		HFONT _handle;
	public:
		Font(int fontCode)
		{
			_handle = (HFONT)GetStockObject(fontCode);
		}

		Font(HFONT handle)
			:_handle(handle)
		{
		}

		virtual ~Font(void)
		{
			::DeleteObject(_handle);
		}

		operator HFONT() const 
		{
			return _handle;
		}

		static int PointsToLogical(int nPointSize)
		{
			HDC hdc      = ::GetDC(0);
			int nLogSize = - MulDiv(nPointSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
			ReleaseDC(0, hdc);

			return nLogSize;
		}

		static HFONT createPointFont(int pointSize, const std::wstring & faceName, bool isBold = false)//, HDC hdc = 0)
		{
			return ::CreateFont(PointsToLogical(pointSize), 
					0, 0, 0, 
					isBold ? FW_BOLD : 0,
					0,0,0,0,0,0,
					DEFAULT_QUALITY, //ANTIALIASED_QUALITY, //CLEARTYPE_QUALITY,
					0,
					faceName.c_str());
		}
	};

	namespace Fonts
	{
		class DEFAULT : public Font
		{
		public:
			DEFAULT():Font(DEFAULT_GUI_FONT){};
		};

        class FIXED : public Font
        {
        public:
            FIXED():Font(SYSTEM_FIXED_FONT) {};
        };
	};

    class Color
    {
    private:
        COLORREF _color;
    public:
        Color(COLORREF color):_color(color){}
        Color(int red, int green, int blue)
            :_color(RGB(red, green, blue))
        {
        }

        BYTE getRed() const
        {
            return GetRValue(_color);
        }

        BYTE getGreen() const
        {
            return GetGValue(_color);
        }
        BYTE getBlue() const
        {
            return GetBValue(_color);
        }

        operator COLORREF() { return _color;}
    };

	namespace Colors
	{
		enum Color
		{
			WINDOW        = COLOR_WINDOW,
			WINDOWTEXT    = COLOR_WINDOWTEXT,
			DIALOG        = COLOR_3DFACE,
			GREY          = COLOR_3DFACE + 1,
            MENU          = COLOR_MENU,
            MENUHILIGHT   = COLOR_MENUHILIGHT,
            HIGHLIGHT     = COLOR_HIGHLIGHT,
            HIGHLIGHTTEXT = COLOR_HIGHLIGHTTEXT
		};

        
	};

	class Pen
	{
	private:
		HPEN _hPen;
	public:
		Pen()
			:_hPen(0)
		{
		}
		Pen(HPEN hPen)
			:_hPen(hPen)
		{
		}
		Pen(const Pen & pen)
			:_hPen(pen._hPen)
		{
		}
		virtual ~Pen()
		{
			::DeleteObject(_hPen);
		}

		operator HPEN()
		{
			return _hPen;
		}

		static HPEN create(int type, int width, Colors::Color color)
		{
			return ::CreatePen(type, width, ::GetSysColor(color));
		}
	};

    class Image : public GdiHandleGuard<HBITMAP>
	{
	//private:
    //    HBITMAP _bitmap;
	public:
		//Image           (                ):_bitmap(0){}
		//Image           (LPCWSTR resource):_bitmap(::LoadBitmap(::GetModuleHandle(0), resource)){}
        
        //Image(const Image & other)
        //    :_bitmap(other._bitmap)
        //{
        //}

        //~Image()
        //{
        //    ::DeleteObject(_bitmap);
        //}
		//operator HBITMAP(                ) const { return _bitmap;}
		//operator HGDIOBJ(                ) const { return _bitmap;}
		//void     load   (LPCWSTR resource){ _bitmap = ::LoadBitmap(::GetModuleHandle(0), resource);}
	};

    class Icon
    {
        HICON _icon;
    public:
        Icon(          ):_icon(0)   {}
   		Icon(HICON icon):_icon(icon){}

   		operator HICON() const {return _icon;}
    };

    class ImageList
	{
	private:
		HIMAGELIST _imageList;
	public:
		ImageList(int cx = 16, int cy = 16):_imageList(0) 
        {
            _imageList = ImageList_Create(cx, cy, ILC_MASK|ILC_COLOR32, 1, 10);
        }
        
        void drawImage(const HDC & hdc, int index, int x, int y, int cx, int cy, UINT style = ILD_TRANSPARENT)
        {
            ImageList_DrawEx(_imageList, index, hdc, x, y, cx, cy, CLR_NONE, CLR_NONE, style);
        }

        int addImage(const HBITMAP & bitmap)
        {
            return ImageList_AddMasked(_imageList, bitmap, RGB(0, 0, 0));
        }

        operator HIMAGELIST() const { return _imageList;}
	};

	class Graphics
	{
	private:
		HWND        _hwnd;
		HDC         _hdc;
		PAINTSTRUCT _paint;
		TEXTMETRIC  _tm;
		bool        _isOwn;
	public:
		Graphics(HWND windowHandle)
			:_hwnd(windowHandle), _isOwn(true)
		{
			_hdc = ::BeginPaint(windowHandle, &_paint);
		}

		Graphics(HDC hdc):_hdc(hdc), _isOwn(false){}

		virtual ~Graphics()
		{
			if(!_isOwn)
			{
				::DeleteDC(_hdc);
			}
			else
			{
				::EndPaint(_hwnd, &_paint);
			}
		}

		operator HDC() const { return _hdc; }

		unsigned long getTop(void) const { return _paint.rcPaint.top; }

		unsigned long getBottom(void) const {return _paint.rcPaint.bottom;}

		int getFontHeight(void)
		{
			return _tm.tmHeight;
		}

		int getFontWidth(void)
		{
			return _tm.tmAveCharWidth;
		}

        Dimension getTextDimension(const std::wstring & text)
        {
            SIZE size;
            GetTextExtentPoint32(_hdc, text.c_str(), static_cast<int>(text.size()), &size);
            return Dimension(size.cx, size.cy);
        }

		void setBrush(HBRUSH brush)
		{
			::SelectObject(_hdc, brush);
		}

		void setPen(Pen & pen)
		{
			::SelectObject(_hdc, pen);
		}

		void line( int x1, int y1, int x2, int y2 )
        {
			::MoveToEx(_hdc, x1, y1, 0);
			::LineTo  (_hdc, x2, y2);
		}

		void rectangle(int x, int y, int width, int height)
		{
			::Rectangle(_hdc, x, y, width, height);
		}

		void rectangle(RECT rect)
		{
			rectangle(rect.left, rect.top, rect.right, rect.bottom);
		}

		void text(int x, int y, const std::wstring & buf)
		{
			::TextOut( _hdc, x, y, buf.c_str(), static_cast<int>(buf.size()));
		}

		void bitmap(int x, int y, int dx, int dy, const HBITMAP & image)
		{
			Graphics compatible = ::CreateCompatibleDC(_hdc);
			HGDIOBJ old = ::SelectObject(compatible, image);

            ::BitBlt(_hdc, x, y, dx, dy, compatible, 0, 0, SRCCOPY);
			::SelectObject(compatible, old);
		}

        void bitmap(int x, int y, int dx, int dy, const HBITMAP & image, COLORREF mask)
        {
            GdiHandleGuard<HBITMAP> hMask = createMaskBitmap(image, mask);

			Graphics compatible = ::CreateCompatibleDC(_hdc);
            
            HGDIOBJ old = ::SelectObject(compatible, hMask);
            ::BitBlt(_hdc, x, y, dx, dy, compatible, 0, 0, SRCAND);

            ::SelectObject(compatible, image);
            ::BitBlt(_hdc, x, y, dx, dy, compatible, 0, 0, SRCPAINT);
        }

        HBITMAP createMaskBitmap(HBITMAP hbmColour, COLORREF crTransparent)
        {
            HDC hdcMem, hdcMem2;
            HBITMAP hbmMask;
            BITMAP bm;

            // Create monochrome (1 bit) mask bitmap.  

            ::GetObject(hbmColour, sizeof(BITMAP), &bm);
            hbmMask = ::CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

            // Get some HDCs that are compatible with the display driver

            hdcMem = ::CreateCompatibleDC(0);
            hdcMem2 = ::CreateCompatibleDC(0);

            ::SelectObject(hdcMem, hbmColour);
            ::SelectObject(hdcMem2, hbmMask);

            // Set the background colour of the colour image to the colour
            // you want to be transparent.
            ::SetBkColor(hdcMem, crTransparent);

            // Copy the bits from the colour image to the B+W mask... everything
            // with the background colour ends up white while everythig else ends up
            // black...Just what we wanted.

            ::BitBlt(hdcMem2, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);

            // Take our new mask and use it to turn the transparent colour in our
            // original colour image to black so the transparency effect will
            // work right.
            ::BitBlt(hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem2, 0, 0, SRCINVERT);

            // Clean up.

            DeleteDC(hdcMem);
            DeleteDC(hdcMem2);

            return hbmMask;
        }

		void fillRect(const RECT & rect)
		{
			::ExtTextOut(_hdc, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);
		}

		void setFont(Font font)
		{
			::SelectObject(_hdc, font);
			::GetTextMetrics(_hdc, &_tm);
		}

        Color setBkColor(Color & color)
		{
            return Color(::SetBkColor(_hdc, color));
		}

        Color setBkColor(Colors::Color color)
		{
            return Color(::SetBkColor(_hdc, GetSysColor(color)));
		}

        void setBkMode(int mode)
        {
            ::SetBkMode(_hdc, mode);
        }

		Color setTextColor(Color & color)
		{
			return ::SetTextColor(_hdc, color);
		}

        Color setTextColor(Colors::Color color)
		{
			return ::SetTextColor(_hdc, GetSysColor(color));
		}

		static HBRUSH createBrush(int type, Colors::Color color)
		{
			LOGBRUSH logBrush = {0};
			logBrush.lbStyle = type;
			logBrush.lbColor = ::GetSysColor(color);
			return ::CreateBrushIndirect(&logBrush);
		}

        void gradientFill(const RECT & rect, const Color & color1, const Color & color2)
        {
			TRIVERTEX vertexes[2];
			GRADIENT_RECT gradientRects = {0};

			vertexes[0].x     = rect.left;
			vertexes[0].y     = rect.top;
			//vertexes[0].Red   = color1.getRed();
			//vertexes[0].Green = color1.getGreen();
			//vertexes[0].Blue  = color1.getBlue();
			vertexes[0].Red   = 0xC000;
			vertexes[0].Green = 0xC000;
			vertexes[0].Blue  = 0xC000;

			vertexes[1].x     = rect.right; // - rect.left + 1;
			vertexes[1].y     = rect.bottom;// - rect.top  + 1;
			//vertexes[1].Red   = color2.getRed();
			//vertexes[1].Green = color2.getGreen();
			//vertexes[1].Blue  = color2.getBlue();
			vertexes[1].Red   = 0xFA00;
			vertexes[1].Green = 0xFD00;
			vertexes[1].Blue  = 0xF220;

			gradientRects.UpperLeft  = 1;
			gradientRects.LowerRight = 0;

			::GradientFill(_hdc, vertexes, 2, &gradientRects, 1, GRADIENT_FILL_RECT_V);
        }
	};
};