#pragma once
#include "Skin/Theme/Pocket.h"
#include "Skin/Figure/Manager.h"

namespace fgo::dark::skin::theme
{
	struct State
	{
		COLORREF fillColor;
		COLORREF textBkColor;
		COLORREF textForeColor;
		COLORREF textBackColor;
		HBRUSH fillBrush;

		std::vector<std::shared_ptr<figure::Node>> nodes;
		std::vector<std::shared_ptr<figure::IconNode>> iconNodes;
		std::vector<std::shared_ptr<figure::TextNode>> textNodes;

		State()
		{
			fillColor = CLR_NONE;
			textBkColor = CLR_NONE;
			textForeColor = CLR_NONE;
			textBackColor = CLR_NONE;
			fillBrush = 0;
		}

		~State()
		{
			deleteFillBrush();
		}

		HBRUSH getFillBrush()
		{
			if (!fillBrush && fillColor != CLR_NONE)
				fillBrush = ::CreateSolidBrush(fillColor);
			return fillBrush;
		}

		void deleteFillBrush()
		{
			if (fillBrush)
			{
				::DeleteObject(fillBrush);
				fillBrush = 0;
			}
		}

		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			ColorSet colorSet;
			if (S_OK == getPrivateProfileColorSet(element, L"colorSet", colorSet))
			{
				fillColor = colorSet.fillColor;
				textBkColor = colorSet.textBkColor;
				textForeColor = colorSet.textForeColor;
				textBackColor = colorSet.textBackColor;
			}
			getPrivateProfileNamedColor(element, L"fillColor", fillColor, ColorSet::Type::fillColor);
			getPrivateProfileNamedColor(element, L"textBkColor", textBkColor, ColorSet::Type::textBkColor);
			getPrivateProfileNamedColor(element, L"textForeColor", textForeColor, ColorSet::Type::textForeColor);
			getPrivateProfileNamedColor(element, L"textBackColor", textBackColor, ColorSet::Type::textBackColor);

			deleteFillBrush();
			loadFigure(element);
			loadIconFigure(element);
			loadTextFigure(element);

			return S_OK;
		}

		HRESULT loadFigure(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			// <Figure>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"Figure");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t name = L"";
				getPrivateProfileBSTR(element, L"name", name);
				auto node = figure::manager.getNode(name);
				MY_TRACE(_T("%ws => 0x%08X\n"), (LPCWSTR)name, node.get());
				if (node) nodes.push_back(node);
			}

			return S_OK;
		}

		HRESULT loadIconFigure(const MSXML2::IXMLDOMElementPtr& parentElement)
		{
			MY_TRACE_FUNC("");

			// <IconFigure>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = parentElement->selectNodes(L"IconFigure");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t name = L"";
				getPrivateProfileBSTR(element, L"name", name);
				auto iconNode = figure::manager.getIconNode(name);
				MY_TRACE(_T("%ws => 0x%08X\n"), (LPCWSTR)name, iconNode.get());
				if (iconNode) iconNodes.push_back(iconNode);
			}

			return S_OK;
		}

		HRESULT loadTextFigure(const MSXML2::IXMLDOMElementPtr& parentElement)
		{
			MY_TRACE_FUNC("");

			// <TextFigure>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = parentElement->selectNodes(L"TextFigure");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t name = L"";
				getPrivateProfileBSTR(element, L"name", name);
				auto textNode = figure::manager.getTextNode(name);
				MY_TRACE(_T("%ws => 0x%08X\n"), (LPCWSTR)name, textNode.get());
				if (textNode) textNodes.push_back(textNode);
			}

			return S_OK;
		}
	};
}
