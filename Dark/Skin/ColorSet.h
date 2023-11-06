#pragma once

namespace fgo::dark::skin
{
	struct ColorSet
	{
		COLORREF fillColor;
		COLORREF edgeColor;
		COLORREF textBkColor;
		COLORREF textForeColor;
		COLORREF textBackColor;

		struct Type
		{
			static const int fillColor = 0;
			static const int edgeColor = 1;
			static const int textBkColor = 2;
			static const int textForeColor = 3;
			static const int textBackColor = 4;
		};
	};

	inline struct ColorSetManager
	{
		std::map<_bstr_t, ColorSet> collection;

		inline HRESULT WINAPI getPrivateProfileNamedColor(
			const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, COLORREF& outValue, int type)
		{
			_variant_t var = element->getAttribute(name);
			if (var.vt == VT_NULL) return S_FALSE;
			_bstr_t varBSTR(var);
			auto it = collection.find(varBSTR);
			if (it != collection.end())
			{
				switch (type)
				{
				case ColorSet::Type::fillColor: outValue = it->second.fillColor; return S_OK;
				case ColorSet::Type::edgeColor: outValue = it->second.edgeColor; return S_OK;
				case ColorSet::Type::textBkColor: outValue = it->second.textBkColor; return S_OK;
				case ColorSet::Type::textForeColor: outValue = it->second.textForeColor; return S_OK;
				case ColorSet::Type::textBackColor: outValue = it->second.textBackColor; return S_OK;
				}
			}
			if (::lstrlenW(varBSTR) == 0)
			{
				outValue = CLR_NONE;
				return S_OK;
			}
			return ::getPrivateProfileColor(element, name, outValue);
		}

		inline HRESULT WINAPI getPrivateProfileColorSet(
			const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, ColorSet& outValue)
		{
			_variant_t var = element->getAttribute(name);
			if (var.vt == VT_NULL) return S_FALSE;
			_bstr_t varBSTR(var);
			auto it = collection.find(varBSTR);
			if (it == collection.end()) return S_FALSE;
			outValue = it->second;
			return S_OK;
		}

		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			collection.clear();

			// <NamedColors>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"NamedColors");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				// <NamedColor>を読み込みます。

				MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"NamedColor");
				int c = nodeList->length;
				for (int i = 0; i < c; i++)
				{
					MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

					_bstr_t name = L"";
					getPrivateProfileString(element, L"name", name);

					ColorSet colorSet;

					colorSet.fillColor = CLR_NONE;
					getPrivateProfileColor(element, L"fillColor", colorSet.fillColor);

					colorSet.edgeColor = CLR_NONE;
					getPrivateProfileColor(element, L"edgeColor", colorSet.edgeColor);

					colorSet.textBkColor = CLR_NONE;
					getPrivateProfileColor(element, L"textBkColor", colorSet.textBkColor);

					colorSet.textForeColor = CLR_NONE;
					getPrivateProfileColor(element, L"textForeColor", colorSet.textForeColor);

					colorSet.textBackColor = CLR_NONE;
					getPrivateProfileColor(element, L"textBackColor", colorSet.textBackColor);

					collection[name] = colorSet;
				}
			}

			return S_OK;
		}
	} color_set_manager;

	inline HRESULT WINAPI getPrivateProfileNamedColor(
		const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, COLORREF& outValue, int type)
	{
		return color_set_manager.getPrivateProfileNamedColor(element, name, outValue, type);
	}

	inline HRESULT WINAPI getPrivateProfileColorSet(
		const MSXML2::IXMLDOMElementPtr& element, LPCWSTR name, ColorSet& outValue)
	{
		return color_set_manager.getPrivateProfileColorSet(element, name, outValue);
	}
}
