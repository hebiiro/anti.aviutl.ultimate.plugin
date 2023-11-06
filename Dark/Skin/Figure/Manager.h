#pragma once
#include "Skin/Figure/Node.h"

namespace fgo::dark::skin::figure
{
	inline struct Manager
	{
		std::map<_bstr_t, std::shared_ptr<Node>> nodes;
		std::map<_bstr_t, std::shared_ptr<IconNode>> iconNodes;
		std::map<_bstr_t, std::shared_ptr<TextNode>> textNodes;

		void clear()
		{
			MY_TRACE_FUNC("");

			nodes.clear();
			iconNodes.clear();
			textNodes.clear();
		}

		std::shared_ptr<Node> getNode(LPCWSTR name)
		{
			return nodes[name];
		}

		std::shared_ptr<IconNode> getIconNode(LPCWSTR name)
		{
			return iconNodes[name];
		}

		std::shared_ptr<TextNode> getTextNode(LPCWSTR name)
		{
			return textNodes[name];
		}

		//
		// <Figures>を読み込みます。
		//
		HRESULT loadFigures(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"Figures");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				loadNode<Null>(element, L"Null");
				loadNode<FillRect>(element, L"FillRect");
				loadNode<FrameRect>(element, L"FrameRect");
				loadNode<RoundRect>(element, L"RoundRect");
				loadNode<DrawRectangle>(element, L"DrawRectangle");
				loadNode<DrawAlphaRectangle>(element, L"DrawAlphaRectangle");
				loadNode<DrawAlphaRoundRect>(element, L"DrawAlphaRoundRect");
				loadNode<DrawSingleRaisedEdge>(element, L"DrawSingleRaisedEdge");
				loadNode<DrawSingleSunkenEdge>(element, L"DrawSingleSunkenEdge");
				loadNode<DrawDoubleRaisedEdge>(element, L"DrawDoubleRaisedEdge");
				loadNode<DrawDoubleSunkenEdge>(element, L"DrawDoubleSunkenEdge");
				loadNode<DrawDoubleBumpEdge>(element, L"DrawDoubleBumpEdge");
				loadNode<DrawDoubleEtchedEdge>(element, L"DrawDoubleEtchedEdge");
				loadTextNode<DrawText>(element, L"DrawText");
				loadTextNode<DrawText>(element, L"DrawShadowText");
				loadIconNode<DrawIcon>(element, L"DrawIcon");
				loadIconNode<DrawIcon>(element, L"DrawShadowIcon");
			}

			return S_OK;
		}

		template<class T>
		HRESULT loadNode(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR tagName)
		{
			MY_TRACE_FUNC("%ws", tagName);

			MSXML2::IXMLDOMNodeListPtr nodeList = element->getElementsByTagName(tagName);
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				auto node = std::make_shared<T>();
				node->load(element);
				if (nodes.contains(node->getName())) continue; // すでに同じ名前が使われている場合はコレクションに追加しません。
				nodes[node->getName()] = node;
				MY_TRACE(_T("new node = %ws, %ws\n"), tagName, node->getName());
			}

			return S_OK;
		}

		template<class T>
		HRESULT loadIconNode(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR tagName)
		{
			MY_TRACE_FUNC("%ws", tagName);

			MSXML2::IXMLDOMNodeListPtr nodeList = element->getElementsByTagName(tagName);
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				auto node = std::make_shared<T>();
				node->load(element);
				if (iconNodes.contains(node->getName())) continue; // すでに同じ名前が使われている場合はコレクションに追加しません。
				iconNodes[node->getName()] = node;
				MY_TRACE(_T("new icon node = %ws, %ws\n"), tagName, node->getName());
			}

			return S_OK;
		}

		template<class T>
		HRESULT loadTextNode(const MSXML2::IXMLDOMElementPtr& element, LPCWSTR tagName)
		{
			MY_TRACE_FUNC("%ws", tagName);

			MSXML2::IXMLDOMNodeListPtr nodeList = element->getElementsByTagName(tagName);
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				auto node = std::make_shared<T>();
				node->load(element);
				if (textNodes.contains(node->getName())) continue; // すでに同じ名前が使われている場合はコレクションに追加しません。
				textNodes[node->getName()] = node;
				MY_TRACE(_T("new text node = %ws, %ws\n"), tagName, node->getName());
			}

			return S_OK;
		}
	} manager;
}
