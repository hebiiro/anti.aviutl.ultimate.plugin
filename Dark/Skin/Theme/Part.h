#pragma once
#include "Skin/Theme/State.h"

namespace fgo::dark::skin::theme
{
	struct Part
	{
		std::vector<std::shared_ptr<State>> collection;

		std::shared_ptr<State> addState(int stateId)
		{
			if ((size_t)stateId >= collection.size())
				collection.resize(stateId + 1);

			if (!collection[stateId])
				collection[stateId] = std::make_shared<State>();

			return collection[stateId];
		}

		std::shared_ptr<State> getState(int stateId)
		{
			if ((size_t)stateId >= collection.size())
				return 0;

			return collection[stateId];
		}

		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE_FUNC("");

			// <State>を読み込みます。
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"State");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t id = L"";
				getPrivateProfileBSTR(element, L"id", id);
//				MY_TRACE_WSTR((LPCWSTR)id);

				auto state = addState(pocket.getStateId(id));

				state->load(element);
			}

			return S_OK;
		}
	};
}
