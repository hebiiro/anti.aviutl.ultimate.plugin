#pragma once
#include "Painter.h"
#include "Skin/Config.h"
#include "Skin/ColorSet.h"

namespace fgo::dark::skin::figure
{
	struct Node
	{
		_bstr_t name;
		int ix, iy;
		int ox, oy;
		int left, top, right, bottom;
		int addLeft, addTop, addRight, addBottom;

		Node()
		{
			name = L"";
			ix = iy = ox = oy = 0;
			left = top = right = bottom = INT_MIN;
			addLeft = addTop = addRight = addBottom = INT_MIN;
		}
		virtual ~Node()
		{
		}

		virtual void load(const MSXML2::IXMLDOMElementPtr& element)
		{
			getPrivateProfileBSTR(element, L"name", name);

			getPrivateProfileInt(element, L"ix", ix);
			getPrivateProfileInt(element, L"iy", iy);
			getPrivateProfileInt(element, L"ox", ox);
			getPrivateProfileInt(element, L"oy", oy);

			getPrivateProfileInt(element, L"left", left);
			getPrivateProfileInt(element, L"top", top);
			getPrivateProfileInt(element, L"right", right);
			getPrivateProfileInt(element, L"bottom", bottom);

			getPrivateProfileInt(element, L"addLeft", addLeft);
			getPrivateProfileInt(element, L"addTop", addTop);
			getPrivateProfileInt(element, L"addRight", addRight);
			getPrivateProfileInt(element, L"addBottom", addBottom);
		}

		virtual void draw(HDC dc, LPRECT rc)
		{
			::InflateRect(rc, ix, iy);
			::OffsetRect(rc, ox, oy);

			if (left != INT_MIN) rc->left = left;
			if (top != INT_MIN) rc->top = top;
			if (right != INT_MIN) rc->right = right;
			if (bottom != INT_MIN) rc->bottom = bottom;

			if (addLeft != INT_MIN) rc->left += addLeft;
			if (addTop != INT_MIN) rc->top += addTop;
			if (addRight != INT_MIN) rc->right += addRight;
			if (addBottom != INT_MIN) rc->bottom += addBottom;
		}

		LPCWSTR getName() const { return name; }
	};

	struct Null : Node
	{
		Null()
		{
		}

		~Null() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);
		}
	};

	struct FillRect : Node
	{
		COLORREF fillColor;

		FillRect()
		{
			fillColor = CLR_NONE;
		}

		~FillRect() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);

			ColorSet colorSet;
			if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
			{
				fillColor = colorSet.fillColor;
			}
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			if (fillColor != CLR_NONE)
				painter::fillRect(dc, rc, fillColor);
		}
	};

	struct FrameRect : Node
	{
		COLORREF edgeColor;
		int edgeWidth;

		FrameRect()
		{
			edgeColor = CLR_NONE;
			edgeWidth = 0;
		}

		~FrameRect() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);

			ColorSet colorSet;
			if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
			{
				edgeColor = colorSet.edgeColor;
			}
			getPrivateProfileNamedColor(element, L"edgeColor", edgeColor, ColorSet::Type::edgeColor);
			getPrivateProfileInt(element, L"edgeWidth", edgeWidth);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			if (edgeColor != CLR_NONE)
				painter::frameRect(dc, rc, edgeColor, edgeWidth);
		}
	};

	struct RoundRect : Node
	{
		COLORREF fillColor;
		COLORREF edgeColor;
		int edgeWidth;
		int roundWidth;
		int roundHeight;

		RoundRect()
		{
			fillColor = CLR_NONE;
			edgeColor = CLR_NONE;
			edgeWidth = 0;
			roundWidth = 0;
			roundHeight = 0;
		}

		~RoundRect() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);

			ColorSet colorSet;
			if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
			{
				fillColor = colorSet.fillColor;
				edgeColor = colorSet.edgeColor;
			}
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
			getPrivateProfileNamedColor(element, L"edgeColor", edgeColor, ColorSet::Type::edgeColor);
			getPrivateProfileInt(element, L"edgeWidth", edgeWidth);
			getPrivateProfileInt(element, L"roundWidth", roundWidth);
			getPrivateProfileInt(element, L"roundHeight", roundHeight);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			if (config.getRoundMode() == Config::RoundMode::On)
				painter::roundRect(dc, rc, fillColor, edgeColor, edgeWidth, roundWidth, roundHeight);
			else
				painter::drawRectangle(dc, rc, fillColor, edgeColor, edgeWidth);
		}
	};

	struct DrawRectangle : Node
	{
		COLORREF fillColor;
		COLORREF edgeColor;
		int edgeWidth;

		DrawRectangle()
		{
			fillColor = CLR_NONE;
			edgeColor = CLR_NONE;
			edgeWidth = 0;
		}

		~DrawRectangle() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);

			ColorSet colorSet;
			if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
			{
				fillColor = colorSet.fillColor;
				edgeColor = colorSet.edgeColor;
			}
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
			getPrivateProfileNamedColor(element, L"edgeColor", edgeColor, ColorSet::Type::edgeColor);
			getPrivateProfileInt(element, L"edgeWidth", edgeWidth);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawRectangle(dc, rc, fillColor, edgeColor, edgeWidth);
		}
	};

	struct DrawAlphaRectangle : Node
	{
		COLORREF fillColor = CLR_NONE;
		COLORREF edgeColor = CLR_NONE;
		int edgeWidth = 0;
		int alpha = 0;

		DrawAlphaRectangle()
		{
		}

		~DrawAlphaRectangle() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);

			ColorSet colorSet;
			if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
			{
				fillColor = colorSet.fillColor;
				edgeColor = colorSet.edgeColor;
			}
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
			getPrivateProfileNamedColor(element, L"edgeColor", edgeColor, ColorSet::Type::edgeColor);
			getPrivateProfileInt(element, L"edgeWidth", edgeWidth);
			getPrivateProfileInt(element, L"alpha", alpha);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawAlphaRectangle(dc, rc, fillColor, edgeColor, edgeWidth, alpha);
		}
	};

	struct DrawAlphaRoundRect : Node
	{
		COLORREF fillColor = CLR_NONE;
		COLORREF edgeColor = CLR_NONE;
		int edgeWidth = 0;
		int roundWidth = 0;
		int roundHeight = 0;
		int alpha = 0;

		DrawAlphaRoundRect()
		{
		}

		~DrawAlphaRoundRect() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);

			ColorSet colorSet;
			if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
			{
				fillColor = colorSet.fillColor;
				edgeColor = colorSet.edgeColor;
			}
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
			getPrivateProfileNamedColor(element, L"edgeColor", edgeColor, ColorSet::Type::edgeColor);
			getPrivateProfileInt(element, L"edgeWidth", edgeWidth);
			getPrivateProfileInt(element, L"roundWidth", roundWidth);
			getPrivateProfileInt(element, L"roundHeight", roundHeight);
			getPrivateProfileInt(element, L"alpha", alpha);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			if (config.getRoundMode() == Config::RoundMode::On)
				painter::drawAlphaRoundRect(dc, rc, fillColor, edgeColor, edgeWidth, roundWidth, roundHeight, alpha);
			else
				painter::drawAlphaRectangle(dc, rc, fillColor, edgeColor, edgeWidth, alpha);
		}
	};

	struct DrawSingleRaisedEdge : Node
	{
		inline static COLORREF topLeftColor = CLR_NONE;
		inline static COLORREF bottomRightColor = CLR_NONE;

		inline static void loadStatic(const MSXML2::IXMLDOMElementPtr& element)
		{
			// <DrawSingleRaisedEdge>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"DrawSingleRaisedEdge");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				getPrivateProfileNamedColor(element, L"topLeftColor", topLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"bottomRightColor", bottomRightColor, ColorSet::Type::edgeColor);
			}
		}

		DrawSingleRaisedEdge()
		{
		}

		~DrawSingleRaisedEdge() override
		{
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawSingleEdge(dc, rc, topLeftColor, bottomRightColor);
		}
	};

	struct DrawSingleSunkenEdge : Node
	{
		inline static COLORREF topLeftColor = CLR_NONE;
		inline static COLORREF bottomRightColor = CLR_NONE;

		inline static void loadStatic(const MSXML2::IXMLDOMElementPtr& element)
		{
			// <DrawSingleSunkenEdge>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"DrawSingleSunkenEdge");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				getPrivateProfileNamedColor(element, L"topLeftColor", topLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"bottomRightColor", bottomRightColor, ColorSet::Type::edgeColor);
			}
		}

		DrawSingleSunkenEdge()
		{
		}

		~DrawSingleSunkenEdge() override
		{
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawSingleEdge(dc, rc, topLeftColor, bottomRightColor);
		}
	};

	struct DrawDoubleRaisedEdge : Node
	{
		inline static COLORREF innerTopLeftColor = CLR_NONE;
		inline static COLORREF innerBottomRightColor = CLR_NONE;
		inline static COLORREF outerTopLeftColor = CLR_NONE;
		inline static COLORREF outerBottomRightColor = CLR_NONE;

		inline static void loadStatic(const MSXML2::IXMLDOMElementPtr& element)
		{
			// <DrawDoubleRaisedEdge>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"DrawDoubleRaisedEdge");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				getPrivateProfileNamedColor(element, L"innerTopLeftColor", innerTopLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"innerBottomRightColor", innerBottomRightColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"outerTopLeftColor", outerTopLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"outerBottomRightColor", outerBottomRightColor, ColorSet::Type::edgeColor);
			}
		}

		DrawDoubleRaisedEdge()
		{
		}

		~DrawDoubleRaisedEdge() override
		{
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawDoubleEdge(dc, rc,
				innerTopLeftColor, innerBottomRightColor,
				outerTopLeftColor, outerBottomRightColor);
		}
	};

	struct DrawDoubleSunkenEdge : Node
	{
		inline static COLORREF innerTopLeftColor = CLR_NONE;
		inline static COLORREF innerBottomRightColor = CLR_NONE;
		inline static COLORREF outerTopLeftColor = CLR_NONE;
		inline static COLORREF outerBottomRightColor = CLR_NONE;

		inline static void loadStatic(const MSXML2::IXMLDOMElementPtr& element)
		{
			// <DrawDoubleSunkenEdge>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"DrawDoubleSunkenEdge");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				getPrivateProfileNamedColor(element, L"innerTopLeftColor", innerTopLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"innerBottomRightColor", innerBottomRightColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"outerTopLeftColor", outerTopLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"outerBottomRightColor", outerBottomRightColor, ColorSet::Type::edgeColor);
			}
		}

		DrawDoubleSunkenEdge()
		{
		}

		~DrawDoubleSunkenEdge() override
		{
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawDoubleEdge(dc, rc,
				innerTopLeftColor, innerBottomRightColor,
				outerTopLeftColor, outerBottomRightColor);
		}
	};

	struct DrawDoubleBumpEdge : Node
	{
		inline static COLORREF innerTopLeftColor = CLR_NONE;
		inline static COLORREF innerBottomRightColor = CLR_NONE;
		inline static COLORREF outerTopLeftColor = CLR_NONE;
		inline static COLORREF outerBottomRightColor = CLR_NONE;

		inline static void loadStatic(const MSXML2::IXMLDOMElementPtr& element)
		{
			// <DrawDoubleBumpEdge>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"DrawDoubleBumpEdge");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				getPrivateProfileNamedColor(element, L"innerTopLeftColor", innerTopLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"innerBottomRightColor", innerBottomRightColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"outerTopLeftColor", outerTopLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"outerBottomRightColor", outerBottomRightColor, ColorSet::Type::edgeColor);
			}
		}

		DrawDoubleBumpEdge()
		{
		}

		~DrawDoubleBumpEdge() override
		{
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawDoubleEdge(dc, rc,
				innerTopLeftColor, innerBottomRightColor,
				outerTopLeftColor, outerBottomRightColor);
		}
	};

	struct DrawDoubleEtchedEdge : Node
	{
		inline static COLORREF innerTopLeftColor = CLR_NONE;
		inline static COLORREF innerBottomRightColor = CLR_NONE;
		inline static COLORREF outerTopLeftColor = CLR_NONE;
		inline static COLORREF outerBottomRightColor = CLR_NONE;

		inline static void loadStatic(const MSXML2::IXMLDOMElementPtr& element)
		{
			// <DrawDoubleEtchedEdge>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"DrawDoubleEtchedEdge");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				getPrivateProfileNamedColor(element, L"innerTopLeftColor", innerTopLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"innerBottomRightColor", innerBottomRightColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"outerTopLeftColor", outerTopLeftColor, ColorSet::Type::edgeColor);
				getPrivateProfileNamedColor(element, L"outerBottomRightColor", outerBottomRightColor, ColorSet::Type::edgeColor);
			}
		}

		DrawDoubleEtchedEdge()
		{
		}

		~DrawDoubleEtchedEdge() override
		{
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawDoubleEdge(dc, rc,
				innerTopLeftColor, innerBottomRightColor,
				outerTopLeftColor, outerBottomRightColor);
		}
	};

	struct TextNode : Node
	{
		_bstr_t fontName = L"";
		int fontWeight = 0;
		BOOL fontItalic = FALSE;
		BOOL fontStroke = FALSE;
		COLORREF fillColor = CLR_NONE;
		COLORREF foreColor = CLR_NONE;
		COLORREF backColor = CLR_NONE;
		UINT format = UINT_MAX;

		TextNode()
		{
		}

		~TextNode() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
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
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
			getPrivateProfileNamedColor(element, L"foreColor", foreColor, ColorSet::Type::textForeColor);
			getPrivateProfileNamedColor(element, L"backColor", backColor, ColorSet::Type::textBackColor);
			getPrivateProfileInt(element, L"format", format);
		}

		virtual void drawText(HDC dc, LPRECT rc, LPCWSTR text, int c, UINT format)
		{
			__super::draw(dc, rc);

			if (config.getShadowMode() == Config::ShadowMode::On)
				painter::drawShadowText(dc, text, c, rc, format, fillColor, foreColor, backColor);
			else
				painter::drawText(dc, text, c, rc, format, fillColor, foreColor);
		}

		virtual void textOut(HDC dc, int x, int y, UINT options, LPRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			__super::draw(dc, rc);

			if (config.getShadowMode() == Config::ShadowMode::On)
				painter::shadowTextOut(dc, x, y, options, rc, text, c, dx, fillColor, foreColor, backColor);
			else
				painter::textOut(dc, x, y, options, rc, text, c, dx, fillColor, foreColor);
		}
	};

	struct DrawText : TextNode
	{
		DrawText()
		{
		}

		~DrawText() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);
		}

		void drawText(HDC dc, LPRECT rc, LPCWSTR text, int c, UINT format) override
		{
			__super::draw(dc, rc);

			painter::drawText(dc, text, c, rc, format, fillColor, foreColor);
		}

		void textOut(HDC dc, int x, int y, UINT options, LPRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			__super::draw(dc, rc);

			painter::textOut(dc, x, y, options, rc, text, c, dx, fillColor, foreColor);
		}
	};

	struct IconNode : TextNode
	{
		_bstr_t text = L"";

		IconNode()
		{
		}

		~IconNode() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);

			getPrivateProfileBSTR(element, L"text", text);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			int length = ::lstrlenW(text);

			RECT rc2 = *rc;
			int w = rc2.right - rc2.left;
			int h = rc2.bottom - rc2.top;
			int size = std::min<int>(w, h);

			UINT format = DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE;

			HFONT font = ::CreateFontW(-size, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, fontName);
			HFONT oldFont = (HFONT)::SelectObject(dc, font);
			int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
			COLORREF oldTextColor = ::GetTextColor(dc);

			if (config.getShadowMode() == Config::ShadowMode::On)
			{
				if (backColor != CLR_NONE)
				{
					::OffsetRect(&rc2, +1, +1);
					::SetTextColor(dc, backColor);
					for (int i = 0; i < length; i++) ::DrawTextW(dc, (BSTR)text + i, 1, &rc2, format);
					::OffsetRect(&rc2, -1, -1);
				}
			}

			if (foreColor != CLR_NONE)
			{
				::SetTextColor(dc, foreColor);
				for (int i = 0; i < length; i++) ::DrawTextW(dc, (BSTR)text + i, 1, &rc2, format);
			}

			::SetTextColor(dc, oldTextColor);
			::SetBkMode(dc, oldBkMode);
			::SelectObject(dc, oldFont);
			::DeleteObject(font);
		}
	};

	struct DrawIcon : IconNode
	{
		DrawIcon()
		{
		}

		~DrawIcon() override
		{
		}

		void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);
		}

		void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			int length = ::lstrlenW(text);

			RECT rc2 = *rc;
			int w = rc2.right - rc2.left;
			int h = rc2.bottom - rc2.top;
			int size = std::min<int>(w, h);

			UINT format = DT_NOCLIP | DT_CENTER | DT_VCENTER | DT_SINGLELINE;

			HFONT font = ::CreateFontW(-size, 0, 0, 0, 0, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, fontName);
			HFONT oldFont = (HFONT)::SelectObject(dc, font);
			int oldBkMode = ::SetBkMode(dc, TRANSPARENT);
			COLORREF oldTextColor = ::SetTextColor(dc, foreColor);

			for (int i = 0; i < length; i++) ::DrawTextW(dc, (BSTR)text + i, 1, &rc2, format);

			::SetTextColor(dc, oldTextColor);
			::SetBkMode(dc, oldBkMode);
			::SelectObject(dc, oldFont);
			::DeleteObject(font);
		}
	};
}
