#pragma once

namespace apn
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr LPCWSTR c_name = L"ultimate.aul";

		HMODULE instance = 0;
		HMODULE target = 0;

		template <typename... Args>
		int message_box(Args... args) {
			WCHAR text[MAX_PATH] = {};
			::StringCchPrintfW(text, std::size(text), args...);
			return ::MessageBoxW(0, text, c_name, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
