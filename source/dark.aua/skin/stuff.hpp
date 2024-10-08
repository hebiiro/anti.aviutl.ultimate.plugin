#pragma once

namespace apn::dark::skin
{
	using Stuff = share::Stuff;

	inline struct StuffManager
	{
		std::map<std::wstring, Stuff> collection;

		Stuff* get(LPCWSTR name)
		{
			return &collection[name];
		}

		void set(LPCWSTR name, const Stuff* stuff)
		{
			if (stuff) collection[name] = *stuff;
		}

		void clear()
		{
			collection.clear();
		}
	} stuff_manager;
}
