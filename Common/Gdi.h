#pragma once

//--------------------------------------------------------------------

template<class T>
class GdiObj
{
public:

	T m_gdiobj = 0;

public:

	GdiObj(T gdiobj)
	{
		m_gdiobj = gdiobj;
	}

	~GdiObj()
	{
		::DeleteObject(m_gdiobj);
	}

	operator T()
	{
		return m_gdiobj;
	}
};

class GdiObjSelector
{
public:

	HDC m_dc = 0;
	HGDIOBJ m_gdiobj = 0;

public:

	GdiObjSelector(HDC dc, HGDIOBJ gdiobj)
	{
		m_gdiobj = ::SelectObject(dc, gdiobj);
	}

	~GdiObjSelector()
	{
		::SelectObject(m_dc, m_gdiobj);
	}
};

//--------------------------------------------------------------------

class ClientDC
{
public:

	HWND m_hwnd = 0;
	HDC m_dc = 0;

public:

	ClientDC(HWND hwnd)
	{
		m_hwnd = hwnd;
		m_dc = ::GetDC(m_hwnd);
	}

	~ClientDC()
	{
		if (m_dc)
			::ReleaseDC(m_hwnd, m_dc);
	}

	operator HDC()
	{
		return m_dc;
	}
};

class PaintDC
{
public:

	PAINTSTRUCT m_ps = {};
	HWND m_hwnd = 0;
	HDC m_dc = 0;

public:

	PaintDC(HWND hwnd)
	{
		m_hwnd = hwnd;
		m_dc = ::BeginPaint(m_hwnd, &m_ps);
	}

	~PaintDC()
	{
		if (m_dc)
			::EndPaint(m_hwnd, &m_ps);
	}

	operator HDC()
	{
		return m_dc;
	}
};
#ifdef _UXTHEME_H_
class UxDC
{
public:

	BP_PAINTPARAMS m_pp = { sizeof(m_pp) };
	HDC m_dc = 0;
	HPAINTBUFFER m_pb = 0;

public:

	UxDC(HDC dc, LPCRECT rc, BP_BUFFERFORMAT format = BPBF_COMPATIBLEBITMAP)
	{
		m_pb = ::BeginBufferedPaint(dc, rc, format, &m_pp, &m_dc);
	}

	~UxDC()
	{
		if (m_pb)
			::EndBufferedPaint(m_pb, TRUE);
	}

	BOOL isValid()
	{
		return !!m_pb;
	}

	operator HDC()
	{
		return m_dc;
	}
};
#endif // _UXTHEME_H_
class DoubleBufferDC
{
public:

	HDC m_dstDC;
	RECT m_rc;

	HDC m_dc;
	HBITMAP m_bitmap;
	HBITMAP m_oldBitmap;

	DoubleBufferDC(HDC dc, LPCRECT rc)
	{
		m_dstDC = dc;
		m_rc = *rc;

		int w = m_rc.right - m_rc.left;
		int h = m_rc.bottom - m_rc.top;

		m_dc = ::CreateCompatibleDC(dc);
		m_bitmap = ::CreateCompatibleBitmap(dc, w, h);
		m_oldBitmap = (HBITMAP)::SelectObject(m_dc, m_bitmap);
	}

	~DoubleBufferDC()
	{
		int w = m_rc.right - m_rc.left;
		int h = m_rc.bottom - m_rc.top;

		::BitBlt(m_dstDC, 0, 0, w, h, m_dc, 0, 0, SRCCOPY);

		::SelectObject(m_dc, m_oldBitmap);
		::DeleteObject(m_bitmap);
		::DeleteDC(m_dc);
	}

	BOOL isValid()
	{
		return !!m_dc;
	}

	operator HDC()
	{
		return m_dc;
	}
};

//--------------------------------------------------------------------
