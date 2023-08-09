#pragma once

namespace fgo
{
	struct LayeredWindowInfo
	{
		LayeredWindowInfo(UINT width, UINT height)
			: m_sourcePosition()
			, m_windowPosition()
			, m_size()
			, m_blend()
			, m_info()
		{
			m_size.cx = width;
			m_size.cy = height;

			m_info.cbSize = sizeof(UPDATELAYEREDWINDOWINFO);
			m_info.pptSrc = &m_sourcePosition;
			m_info.pptDst = &m_windowPosition;
			m_info.psize = &m_size;
			m_info.pblend = &m_blend;
			m_info.dwFlags = ULW_ALPHA;

			m_blend.SourceConstantAlpha = 255;
			m_blend.AlphaFormat = AC_SRC_ALPHA;
		}

		void Update(HWND window, HDC source, BYTE alpha)
		{
			m_info.hdcSrc = source;
			m_blend.SourceConstantAlpha = alpha;
			BOOL result = ::UpdateLayeredWindowIndirect(window, &m_info);
			DWORD error = ::GetLastError();
			MY_TRACE(_T("result = %d, error = 0x%08X\n"), result, error);
		}

		UINT GetWidth() const { return m_size.cx; }
		UINT GetHeight() const { return m_size.cy; }
		void SetWindowPosition(int x, int y)
		{
			m_windowPosition.x = x;
			m_windowPosition.y = y;
		}

	private:
		POINT m_sourcePosition;
		POINT m_windowPosition;
		SIZE m_size;
		BLENDFUNCTION m_blend;
		UPDATELAYEREDWINDOWINFO m_info;
	};
}
