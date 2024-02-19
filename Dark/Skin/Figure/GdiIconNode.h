#pragma once
#include "Skin/Figure/GdiTextNode.h"

namespace fgo::dark::skin::figure
{
	struct GdiIconNodeProp : GdiTextNodeProp
	{
		_bstr_t text = L"";

		void load(const MSXML2::IXMLDOMElementPtr& element)
		{
			__super::load(element);

			getPrivateProfileBSTR(element, L"text", text);
		}
	};

	struct GdiIconNode : IconNode
	{
		GdiIconNodeProp prop;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			prop.load(element);

			__super::load(element);
		}

		virtual void draw(HDC dc, LPRECT rc) override
		{
			prop.calc(rc);

			int length = ::lstrlenW(prop.text);

			int w = rc->right - rc->left;
			int h = rc->bottom - rc->top;
			int size = std::min<int>(w, h);

			int fontHeight = size;
			if (prop.fontHeight != InvalidInt)
				fontHeight = prop.fontHeight;

			int fontWidth = 0;
			if (prop.fontWidth != InvalidInt)
				fontWidth = prop.fontWidth;

			UINT format = DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE;
			if (prop.format != InvalidUint)
				format = prop.format;

			HFONT font = ::CreateFontW(fontHeight, fontWidth, prop.fontAngle, prop.fontAngle,
				prop.fontWeight, prop.fontItalic, FALSE, prop.fontStrike, DEFAULT_CHARSET, 0, 0, 0, 0, prop.fontName);
			HFONT oldFont = (HFONT)::SelectObject(dc, font);
			int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
			COLORREF oldTextColor = ::SetTextColor(dc, prop.foreColor);

			drawText(dc, prop.text, length, rc, format);

			::SetTextColor(dc, oldTextColor);
			::SetBkMode(dc, oldBkMode);
			::SelectObject(dc, oldFont);
			::DeleteObject(font);
		}

		virtual void drawText(HDC dc, LPCWSTR text, int length, LPRECT rc, UINT format) = 0;
	};

	struct DrawIcon : GdiIconNode
	{
		virtual void drawText(HDC dc, LPCWSTR text, int length, LPRECT rc, UINT format) override
		{
			for (int i = 0; i < length; i++) ::DrawTextW(dc, text + i, 1, rc, format);
		}
	};

	struct DrawShadowIcon : GdiIconNode
	{
		virtual void drawText(HDC dc, LPCWSTR text, int length, LPRECT rc, UINT format) override
		{
			if (config.getShadowMode() == Config::ShadowMode::On)
			{
				if (prop.backColor != InvalidColor)
				{
					::OffsetRect(rc, +1, +1);
					::SetTextColor(dc, prop.backColor);
					for (int i = 0; i < length; i++) ::DrawTextW(dc, text + i, 1, rc, format);
					::OffsetRect(rc, -1, -1);
				}
			}

			if (prop.foreColor != InvalidColor)
			{
				::SetTextColor(dc, prop.foreColor);
				for (int i = 0; i < length; i++) ::DrawTextW(dc, text + i, 1, rc, format);
			}
		}
	};
}
