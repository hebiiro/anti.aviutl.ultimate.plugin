#pragma once

namespace my
{
	struct GdiObjSelector
	{
		HDC dc = nullptr;
		HGDIOBJ obj = nullptr;

		GdiObjSelector(HDC dc, HGDIOBJ obj) : dc(dc), obj(::SelectObject(dc, obj)) {}
		~GdiObjSelector() { ::SelectObject(dc, obj); }
	};

	struct ClientDC
	{
		HWND hwnd = nullptr;
		HDC dc = nullptr;

		ClientDC(HWND hwnd) : hwnd(hwnd), dc(::GetDC(hwnd)) {}
		~ClientDC() { if (dc) ::ReleaseDC(hwnd, dc); }
		operator HDC() { return dc; }
	};

	struct WindowDC
	{
		HWND hwnd = nullptr;
		HDC dc = nullptr;

		WindowDC(HWND hwnd) : hwnd(hwnd), dc(::GetWindowDC(hwnd)) {}
		WindowDC(HWND hwnd, HRGN rgn, DWORD flags) : hwnd(hwnd), dc(::GetDCEx(hwnd, rgn, flags)) {}
		~WindowDC() { if (dc) ::ReleaseDC(hwnd, dc); }
		operator HDC() { return dc; }
	};

	struct PaintDC
	{
		PAINTSTRUCT ps = {};
		HWND hwnd = nullptr;
		HDC dc = nullptr;

		PaintDC(HWND hwnd) : hwnd(hwnd), dc(::BeginPaint(hwnd, &ps)) {}
		~PaintDC() { if (dc) ::EndPaint(hwnd, &ps); }
		operator HDC() { return dc; }
	};
#ifdef _UXTHEME_H_
	struct UxDC
	{
		BP_PAINTPARAMS pp = { sizeof(pp) };
		HDC dc = nullptr;
		HPAINTBUFFER pb = nullptr;

		UxDC(HDC dc, LPCRECT rc, BP_BUFFERFORMAT format = BPBF_COMPATIBLEBITMAP) : pb(::BeginBufferedPaint(dc, rc, format, &this->pp, &this->dc)) {}
		~UxDC() { if (pb) ::EndBufferedPaint(pb, TRUE); }
		BOOL is_valid() { return !!pb; }
		operator HDC() { return dc; }
	};
#endif // _UXTHEME_H_
	struct DoubleBufferDC
	{
		HDC orig = nullptr;
		RECT rc = {};

		HDC dc = nullptr;
		HBITMAP bitmap = nullptr;
		HBITMAP old_bitmap = nullptr;

		DoubleBufferDC(HDC orig, LPCRECT rc, BOOL copy = FALSE) : orig(orig), rc(*rc) { init(copy); }
		~DoubleBufferDC() { exit(); }
		BOOL is_valid() { return !!dc; }
		operator HDC() { return dc; }

		void init(BOOL copy)
		{
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);

			dc = ::CreateCompatibleDC(orig);
			bitmap = ::CreateCompatibleBitmap(orig, w, h);
			old_bitmap = (HBITMAP)::SelectObject(dc, bitmap);

			if (copy)
				::BitBlt(dc, 0, 0, w, h, orig, rc.left, rc.top, SRCCOPY);
		}

		void exit()
		{
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);

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
