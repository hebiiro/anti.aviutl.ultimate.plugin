#pragma once

namespace my
{
	struct GdiObjSelector
	{
		HDC dc = 0;
		HGDIOBJ obj = 0;

		GdiObjSelector(HDC dc, HGDIOBJ obj) : dc(dc), obj(::SelectObject(dc, obj)) {}
		~GdiObjSelector() { ::SelectObject(dc, obj); }
	};

	struct ClientDC
	{
		HWND hwnd = 0;
		HDC dc = 0;

		ClientDC(HWND hwnd) : hwnd(hwnd), dc(::GetDC(hwnd)) {}
		~ClientDC() { if (dc) ::ReleaseDC(hwnd, dc); }
		operator HDC() { return dc; }
	};

	struct WindowDC
	{
		HWND hwnd = 0;
		HDC dc = 0;

		WindowDC(HWND hwnd) : hwnd(hwnd), dc(::GetWindowDC(hwnd)) {}
		~WindowDC() { if (dc) ::ReleaseDC(hwnd, dc); }
		operator HDC() { return dc; }
	};

	struct PaintDC
	{
		PAINTSTRUCT ps = {};
		HWND hwnd = 0;
		HDC dc = 0;

		PaintDC(HWND hwnd) : hwnd(hwnd), dc(::BeginPaint(hwnd, &ps)) {}
		~PaintDC() { if (dc) ::EndPaint(hwnd, &ps); }
		operator HDC() { return dc; }
	};
#ifdef _UXTHEME_H_
	struct UxDC
	{
		BP_PAINTPARAMS pp = { sizeof(pp) };
		HDC dc = 0;
		HPAINTBUFFER pb = 0;

		UxDC(HDC dc, LPCRECT rc, BP_BUFFERFORMAT format = BPBF_COMPATIBLEBITMAP) : pb(::BeginBufferedPaint(dc, rc, format, &this->pp, &this->dc)) {}
		~UxDC() { if (pb) ::EndBufferedPaint(pb, TRUE); }
		BOOL is_valid() { return !!pb; }
		operator HDC() { return dc; }
	};
#endif // _UXTHEME_H_
	struct DoubleBufferDC
	{
		HDC orig = 0;
		RECT rc = {};

		HDC dc = 0;
		HBITMAP bitmap = 0;
		HBITMAP old_bitmap = 0;

		DoubleBufferDC(HDC orig, LPCRECT rc, BOOL copy = FALSE) : orig(orig), rc(*rc) { init(copy); }
		~DoubleBufferDC() { exit(); }
		BOOL is_valid() { return !!dc; }
		operator HDC() { return dc; }

		void init(BOOL copy)
		{
			int w = my::get_width(rc);
			int h = my::get_height(rc);

			dc = ::CreateCompatibleDC(orig);
			bitmap = ::CreateCompatibleBitmap(orig, w, h);
			old_bitmap = (HBITMAP)::SelectObject(dc, bitmap);

			if (copy)
				::BitBlt(dc, 0, 0, w, h, orig, rc.left, rc.top, SRCCOPY);
		}

		void exit()
		{
			int w = my::get_width(rc);
			int h = my::get_height(rc);

			::BitBlt(orig, rc.left, rc.top, w, h, dc, 0, 0, SRCCOPY);

			::SelectObject(dc, old_bitmap);
			::DeleteObject(bitmap);
			::DeleteDC(dc);
		}
	};

	namespace gdi
	{
		using selector = GdiObjSelector;
	}
}
