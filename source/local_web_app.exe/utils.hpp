#pragma once

namespace apn::local_web_app
{
	//
	// 指定された文字列の先頭と末尾にある指定された文字を取り除いて返します。
	//
	inline std::wstring trim(const std::wstring& s, const std::wstring& chars)
	{
		auto first = s.find_first_not_of(chars);
		if (first == s.npos) return {};
		auto last = s.find_last_not_of(chars);
		return s.substr(first, last - first + 1);
	}
}
