#pragma once
#include "Skin/Theme/Part.h"

namespace fgo::dark::skin::theme
{
	struct VSClass
	{
		_bstr_t id;
		std::vector<std::shared_ptr<Part>> parts;

		VSClass(LPCWSTR id)
			: id(id)
		{
		}

		LPCWSTR getId() const
		{
			return id;
		}

		std::shared_ptr<Part> addPart(int partId)
		{
			if ((size_t)partId >= parts.size())
				parts.resize(partId + 1);

			if (!parts[partId])
				parts[partId] = std::make_shared<Part>();

			return parts[partId];
		}

		std::shared_ptr<Part> getPart(int partId) const
		{
			if ((size_t)partId >= parts.size())
				return 0;

			return parts[partId];
		}

		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			// <Part>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"Part");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t id = L"";
				getPrivateProfileBSTR(element, L"id", id);
//				MY_TRACE_WSTR((LPCWSTR)id);

				auto part = addPart(pocket.getPartId(id));

				part->load(element);
			}

			return S_OK;
		}
	};
}
