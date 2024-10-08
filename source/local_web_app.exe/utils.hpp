#pragma once

namespace apn::local_web_app
{
	inline std::wstring trim(const std::wstring& s, const std::wstring& chars)
	{
		auto first = s.find_first_not_of(chars);
		if (first == s.npos) return {};
		auto last = s.find_last_not_of(chars);
		return s.substr(first, last);
	}
}
