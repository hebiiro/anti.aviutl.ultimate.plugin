#pragma once
#ifdef _MFC_VER

class CUxDC : public CDC
{
public:

	HPAINTBUFFER m_pb;

	CUxDC(CPaintDC& dc, BP_BUFFERFORMAT format = BPBF_COMPATIBLEBITMAP)
	{
		BP_PAINTPARAMS pp = { sizeof(pp) };
		HDC mdc = nullptr;
		m_pb = ::BeginBufferedPaint(dc, &dc.m_ps.rcPaint, format, &pp, &mdc);
		Attach(mdc);
	}

	virtual ~CUxDC()
	{
		Detach();
		::EndBufferedPaint(m_pb, TRUE);
	}

	BOOL is_valid() const
	{
		return !!m_pb;
	}
};

class CDoubleBufferPaintDC : public CDC
{
public:

	CPaintDC m_dc;
	CBitmap m_bitmap;
	CBitmap* m_oldBitmap;

	CDoubleBufferPaintDC(CWnd* wnd)
		: m_dc(wnd)
	{
		int w = m_dc.m_ps.rcPaint.right - m_dc.m_ps.rcPaint.left;
		int h = m_dc.m_ps.rcPaint.bottom - m_dc.m_ps.rcPaint.top;

		CreateCompatibleDC(&m_dc);
		m_bitmap.CreateCompatibleBitmap(&m_dc, w, h);
		m_oldBitmap = SelectObject(&m_bitmap);
	}

	~CDoubleBufferPaintDC()
	{
		int w = m_dc.m_ps.rcPaint.right - m_dc.m_ps.rcPaint.left;
		int h = m_dc.m_ps.rcPaint.bottom - m_dc.m_ps.rcPaint.top;

		m_dc.BitBlt(0, 0, w, h, this, 0, 0, SRCCOPY);

		SelectObject(m_oldBitmap);
	}
};

class COffscreenDC : public CDC
{
public:

	CBitmap m_bitmap;
	CBitmap* m_oldBitmap;
	int m_w, m_h;

	COffscreenDC()
	{
		m_oldBitmap = nullptr;
		m_w = 0;
		m_h = 0;
	}

	~COffscreenDC()
	{
		if (!GetSafeHdc())
			SelectObject(m_oldBitmap);
	}

	BOOL is_valid()
	{
		return !!m_bitmap.GetSafeHandle();
	}

	BOOL update(CWnd* wnd)
	{
		CRect rc; wnd->GetClientRect(&rc);
		int w = rc.Width();
		int h = rc.Height();
		return update(wnd, w, h);
	}

	BOOL update(CWnd* wnd, int w, int h)
	{
		CClientDC dc(wnd);

		if (!GetSafeHdc())
		{
			CreateCompatibleDC(&dc);
			m_oldBitmap = GetCurrentBitmap();
		}

		if (m_w == w && m_h == h)
			return FALSE;

		m_w = w;
		m_h = h;

		if (m_bitmap.GetSafeHandle())
		{
			SelectObject(m_oldBitmap);
			m_bitmap.DeleteObject();
		}

		m_bitmap.CreateCompatibleBitmap(&dc, w, h);
		SelectObject(&m_bitmap);

		return TRUE;
	}
};

class CDIBDC : public CDC
{
public:

	CBitmap m_bitmap;
	CBitmap* m_oldBitmap = nullptr;
	int m_w = 0, m_h = 0;
	void* m_bits = nullptr;

	CDIBDC(CDC* dc, int w, int h, int bitCount = 32)
	{
		CreateCompatibleDC(dc);

		m_w = w;
		m_h = h;
	
		BITMAPINFO bmi = {};
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = m_w;
		bmi.bmiHeader.biHeight = m_h;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = bitCount;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biSizeImage = m_w * m_h * (bitCount / 8);

		m_bitmap.Attach(::CreateDIBSection(GetSafeHdc(), &bmi, DIB_RGB_COLORS, &m_bits, nullptr, 0));
		m_oldBitmap = SelectObject(&m_bitmap);
	}

	~CDIBDC()
	{
		if (is_valid())
			SelectObject(m_oldBitmap);
	}

	BOOL is_valid()
	{
		return !!m_bitmap.GetSafeHandle();
	}
};

#endif
