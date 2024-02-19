#pragma once
#include "Painter.h"
#include "Skin/Config.h"
#include "Skin/ColorSet.h"

namespace fgo::dark::skin::figure
{
	struct Node
	{
		inline static constexpr int InvalidColor = CLR_NONE;
		inline static constexpr int InvalidInt = INT_MIN;
		inline static constexpr int InvalidUint = UINT_MAX;
		inline static constexpr REAL InvalidReal = FLT_MIN;

		_bstr_t name;

		Node()
		{
			name = L"";
		}

		virtual ~Node()
		{
		}

		virtual void load(const MSXML2::IXMLDOMElementPtr& element)
		{
			getPrivateProfileBSTR(element, L"name", name);
		}

		virtual void draw(HDC dc, LPRECT rc)
		{
		}

		LPCWSTR getName() const { return name; }
	};

	struct IconNode : Node
	{
	};

	struct TextNode : Node
	{
		virtual void drawText(HDC dc, LPRECT rc, LPCWSTR text, int c, UINT format)
		{
			__super::draw(dc, rc);
		}

		virtual void textOut(HDC dc, int x, int y, UINT options, LPRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			__super::draw(dc, rc);
		}
	};
}
