#pragma once
#include "Skin/Figure/GdiNode.h"

namespace fgo::dark::skin::figure
{
	struct GdiTextNodeProp : GdiNodeProp
	{
		_bstr_t fontName = L"";
		int fontHeight = Node::InvalidInt;
		int fontWidth = Node::InvalidInt;
		int fontAngle = 0;
		int fontWeight = 0;
		BOOL fontItalic = FALSE;
		BOOL fontStrike = FALSE;
		COLORREF fillColor = Node::InvalidColor;
		COLORREF foreColor = Node::InvalidColor;
		COLORREF backColor = Node::InvalidColor;
		UINT format = Node::InvalidUint;

		void load(const MSXML2::IXMLDOMElementPtr& element)
		{
			__super::load(element);

			{
				ColorSet colorSet;
				if (S_OK == getPrivateProfileColorSet(element, L"textColorSet", colorSet))
				{
					foreColor = colorSet.textForeColor;
					backColor = colorSet.textBackColor;
				}
			}

			{
				ColorSet colorSet;
				if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
				{
					fillColor = colorSet.fillColor;
					foreColor = colorSet.textForeColor;
					backColor = colorSet.textBackColor;
				}
			}

			getPrivateProfileBSTR(element, L"fontName", fontName);
			getPrivateProfileInt(element, L"fontHeight", fontHeight);
			getPrivateProfileInt(element, L"fontWidth", fontWidth);
			getPrivateProfileInt(element, L"fontAngle", fontAngle);
			getPrivateProfileInt(element, L"fontWeight", fontWeight);
			getPrivateProfileBool(element, L"fontItalic", fontItalic);
			getPrivateProfileBool(element, L"fontStrike", fontStrike);
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
			getPrivateProfileNamedColor(element, L"foreColor", foreColor, ColorSet::Type::textForeColor);
			getPrivateProfileNamedColor(element, L"backColor", backColor, ColorSet::Type::textBackColor);
			getPrivateProfileInt(element, L"format", format);
		}
	};

	struct GdiTextNode : TextNode
	{
		GdiTextNodeProp prop;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			prop.load(element);

			__super::load(element);
		}
	};

	struct DrawText : GdiTextNode
	{
		virtual void drawText(HDC dc, LPRECT rc, LPCWSTR text, int c, UINT format) override
		{
			prop.calc(rc);

			painter::drawText(dc, text, c, rc, format, prop.fillColor, prop.foreColor);
		}

		void textOut(HDC dc, int x, int y, UINT options, LPRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			prop.calc(rc);

			painter::textOut(dc, x, y, options, rc, text, c, dx, prop.fillColor, prop.foreColor);
		}
	};

	struct DrawShadowText : GdiTextNode
	{
		virtual void drawText(HDC dc, LPRECT rc, LPCWSTR text, int c, UINT format) override
		{
			prop.calc(rc);

			if (config.getShadowMode() == Config::ShadowMode::On)
				painter::drawShadowText(dc, text, c, rc, format, prop.fillColor, prop.foreColor, prop.backColor);
			else
				painter::drawText(dc, text, c, rc, format, prop.fillColor, prop.foreColor);
		}

		virtual void textOut(HDC dc, int x, int y, UINT options, LPRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			prop.calc(rc);

			if (config.getShadowMode() == Config::ShadowMode::On)
				painter::shadowTextOut(dc, x, y, options, rc, text, c, dx, prop.fillColor, prop.foreColor, prop.backColor);
			else
				painter::textOut(dc, x, y, options, rc, text, c, dx, prop.fillColor, prop.foreColor);
		}
	};
}
