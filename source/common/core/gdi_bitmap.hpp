#pragma once

namespace apn
{
	struct GdiBitmap
	{
		GdiBitmap(UINT width, UINT height)
			: m_width(width)
			, m_height(height)
			, m_stride((width * 32 + 31) / 32 * 4)
			, m_bits(nullptr)
			, m_dc(nullptr)
			, m_bitmap(nullptr)
			, m_oldBitmap(nullptr)
		{
			m_dc = ::CreateCompatibleDC(nullptr);
			MY_TRACE_HEX(m_dc);

			BITMAPINFO bitmapInfo = {};
			bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
			bitmapInfo.bmiHeader.biWidth = width;
			bitmapInfo.bmiHeader.biHeight = 0 - height;
			bitmapInfo.bmiHeader.biPlanes = 1;
			bitmapInfo.bmiHeader.biBitCount = 32;
			bitmapInfo.bmiHeader.biCompression = BI_RGB;
			m_bitmap = ::CreateDIBSection(
				nullptr, &bitmapInfo, DIB_RGB_COLORS, &m_bits, nullptr, 0);
			MY_TRACE_HEX(m_bitmap);

			m_oldBitmap = (HBITMAP)::SelectObject(m_dc, m_bitmap);
			MY_TRACE_HEX(m_oldBitmap);
		}

		~GdiBitmap()
		{
			::SelectObject(m_dc, m_oldBitmap);
			::DeleteObject(m_bitmap);
			::DeleteDC(m_dc);
		}

		UINT GetWidth() const { return m_width; }
		UINT GetHeight() const { return m_height; }
		UINT GetStride() const { return m_stride; }
		void* GetBits() const { return m_bits; }
		HDC GetDC() const { return m_dc; }

	private:
		const UINT m_width;
		const UINT m_height;
		const UINT m_stride;
		void* m_bits;

		HDC m_dc;
		HBITMAP m_bitmap;
		HBITMAP m_oldBitmap;
	};
}
