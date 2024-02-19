#pragma once
#include "Skin/Figure/Node.h"

namespace fgo::dark::skin::figure
{
	struct GdiNodeProp
	{
		int ix = 0;
		int iy = 0;
		int ox = 0;
		int oy = 0;
		int left = Node::InvalidInt;
		int top = Node::InvalidInt;
		int right = Node::InvalidInt;
		int bottom = Node::InvalidInt;
		int width = Node::InvalidInt;
		int height = Node::InvalidInt;
		int addLeft = Node::InvalidInt;
		int addTop = Node::InvalidInt;
		int addRight = Node::InvalidInt;
		int addBottom = Node::InvalidInt;
		int addWidth = Node::InvalidInt;
		int addHeight = Node::InvalidInt;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element)
		{
			getPrivateProfileInt(element, L"ix", ix);
			getPrivateProfileInt(element, L"iy", iy);
			getPrivateProfileInt(element, L"ox", ox);
			getPrivateProfileInt(element, L"oy", oy);

			getPrivateProfileInt(element, L"left", left);
			getPrivateProfileInt(element, L"top", top);
			getPrivateProfileInt(element, L"right", right);
			getPrivateProfileInt(element, L"bottom", bottom);
			getPrivateProfileInt(element, L"width", width);
			getPrivateProfileInt(element, L"height", height);

			getPrivateProfileInt(element, L"addLeft", addLeft);
			getPrivateProfileInt(element, L"addTop", addTop);
			getPrivateProfileInt(element, L"addRight", addRight);
			getPrivateProfileInt(element, L"addBottom", addBottom);
			getPrivateProfileInt(element, L"addWidth", addWidth);
			getPrivateProfileInt(element, L"addHeight", addHeight);
		}

		virtual void calc(LPRECT rc)
		{
			::InflateRect(rc, ix, iy);
			::OffsetRect(rc, ox, oy);

			if (left != Node::InvalidInt) rc->left = left;
			if (top != Node::InvalidInt) rc->top = top;
			if (right != Node::InvalidInt) rc->right = right;
			if (bottom != Node::InvalidInt) rc->bottom = bottom;

			if (width != Node::InvalidInt)
			{
				rc->left = ::MulDiv(rc->left + rc->right - width, 1, 2);
				rc->right = rc->left + width;
			}

			if (height != Node::InvalidInt)
			{
				rc->top = ::MulDiv(rc->top + rc->bottom - height, 1, 2);
				rc->bottom = rc->top + height;
			}

			if (addLeft != Node::InvalidInt) rc->left += addLeft;
			if (addTop != Node::InvalidInt) rc->top += addTop;
			if (addRight != Node::InvalidInt) rc->right += addRight;
			if (addBottom != Node::InvalidInt) rc->bottom += addBottom;

			if (addWidth != Node::InvalidInt)
			{
				rc->left -= addWidth;
				rc->right += addWidth;
			}

			if (addHeight != Node::InvalidInt)
			{
				rc->top -= addHeight;
				rc->bottom += addHeight;
			}
		}
	};

	struct GdiNode : Node
	{
		GdiNodeProp prop;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			prop.load(element);

			__super::load(element);
		}

		virtual void draw(HDC dc, LPRECT rc) override
		{
			prop.calc(rc);

			__super::draw(dc, rc);
		}
	};

	struct Null : GdiNode
	{
	};

	struct FillRect : GdiNode
	{
		COLORREF fillColor = InvalidColor;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
		{
			__super::load(element);

			ColorSet colorSet;
			if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
			{
				fillColor = colorSet.fillColor;
			}
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
		}

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			if (fillColor != InvalidColor)
				painter::fillRect(dc, rc, fillColor);
		}
	};

	struct FrameRect : GdiNode
	{
		COLORREF edgeColor = InvalidColor;
		int edgeWidth = 0;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			if (edgeColor != InvalidColor)
				painter::frameRect(dc, rc, edgeColor, edgeWidth);
		}
	};

	struct RoundRect : GdiNode
	{
		COLORREF fillColor = InvalidColor;
		COLORREF edgeColor = InvalidColor;
		int edgeWidth = 0;
		int roundWidth = 0;
		int roundHeight = 0;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			if (config.getRoundMode() == Config::RoundMode::On)
				painter::roundRect(dc, rc, fillColor, edgeColor, edgeWidth, roundWidth, roundHeight);
			else
				painter::drawRectangle(dc, rc, fillColor, edgeColor, edgeWidth);
		}
	};

	struct DrawRectangle : GdiNode
	{
		COLORREF fillColor = InvalidColor;
		COLORREF edgeColor = InvalidColor;
		int edgeWidth = 0;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawRectangle(dc, rc, fillColor, edgeColor, edgeWidth);
		}
	};

	struct DrawAlphaRectangle : GdiNode
	{
		COLORREF fillColor = InvalidColor;
		COLORREF edgeColor = InvalidColor;
		int edgeWidth = 0;
		int alpha = 0;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawAlphaRectangle(dc, rc, fillColor, edgeColor, edgeWidth, alpha);
		}
	};

	struct DrawAlphaRoundRect : GdiNode
	{
		COLORREF fillColor = InvalidColor;
		COLORREF edgeColor = InvalidColor;
		int edgeWidth = 0;
		int roundWidth = 0;
		int roundHeight = 0;
		int alpha = 0;

		virtual void load(const MSXML2::IXMLDOMElementPtr& element) override
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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			if (config.getRoundMode() == Config::RoundMode::On)
				painter::drawAlphaRoundRect(dc, rc, fillColor, edgeColor, edgeWidth, roundWidth, roundHeight, alpha);
			else
				painter::drawAlphaRectangle(dc, rc, fillColor, edgeColor, edgeWidth, alpha);
		}
	};

	struct DrawSingleRaisedEdge : GdiNode
	{
		inline static COLORREF topLeftColor = InvalidColor;
		inline static COLORREF bottomRightColor = InvalidColor;

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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawSingleEdge(dc, rc, topLeftColor, bottomRightColor);
		}
	};

	struct DrawSingleSunkenEdge : GdiNode
	{
		inline static COLORREF topLeftColor = InvalidColor;
		inline static COLORREF bottomRightColor = InvalidColor;

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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawSingleEdge(dc, rc, topLeftColor, bottomRightColor);
		}
	};

	struct DrawDoubleRaisedEdge : GdiNode
	{
		inline static COLORREF innerTopLeftColor = InvalidColor;
		inline static COLORREF innerBottomRightColor = InvalidColor;
		inline static COLORREF outerTopLeftColor = InvalidColor;
		inline static COLORREF outerBottomRightColor = InvalidColor;

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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawDoubleEdge(dc, rc,
				innerTopLeftColor, innerBottomRightColor,
				outerTopLeftColor, outerBottomRightColor);
		}
	};

	struct DrawDoubleSunkenEdge : GdiNode
	{
		inline static COLORREF innerTopLeftColor = InvalidColor;
		inline static COLORREF innerBottomRightColor = InvalidColor;
		inline static COLORREF outerTopLeftColor = InvalidColor;
		inline static COLORREF outerBottomRightColor = InvalidColor;

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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawDoubleEdge(dc, rc,
				innerTopLeftColor, innerBottomRightColor,
				outerTopLeftColor, outerBottomRightColor);
		}
	};

	struct DrawDoubleBumpEdge : GdiNode
	{
		inline static COLORREF innerTopLeftColor = InvalidColor;
		inline static COLORREF innerBottomRightColor = InvalidColor;
		inline static COLORREF outerTopLeftColor = InvalidColor;
		inline static COLORREF outerBottomRightColor = InvalidColor;

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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawDoubleEdge(dc, rc,
				innerTopLeftColor, innerBottomRightColor,
				outerTopLeftColor, outerBottomRightColor);
		}
	};

	struct DrawDoubleEtchedEdge : GdiNode
	{
		inline static COLORREF innerTopLeftColor = InvalidColor;
		inline static COLORREF innerBottomRightColor = InvalidColor;
		inline static COLORREF outerTopLeftColor = InvalidColor;
		inline static COLORREF outerBottomRightColor = InvalidColor;

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

		virtual void draw(HDC dc, LPRECT rc) override
		{
			__super::draw(dc, rc);

			painter::drawDoubleEdge(dc, rc,
				innerTopLeftColor, innerBottomRightColor,
				outerTopLeftColor, outerBottomRightColor);
		}
	};
}
