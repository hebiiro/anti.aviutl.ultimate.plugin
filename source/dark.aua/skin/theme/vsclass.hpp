#pragma once

namespace apn::dark::skin::theme
{
	struct VSClass
	{
		my::theme::unique_ptr<> theme;
		std::wstring name;
		std::unordered_map<int, Part> parts;

		VSClass(HTHEME theme, const std::wstring& name)
			: theme(theme)
			, name(name)
		{
		}
	};
}
