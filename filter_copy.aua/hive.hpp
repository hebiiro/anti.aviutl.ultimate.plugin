#pragma once

namespace apn
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr LPCWSTR c_name = L"filter_copy";
		inline static constexpr LPCWSTR c_display_name = L"フィルタコピー";

		HINSTANCE instance = 0;

		int message_box(const std::wstring& text) {
			return ::MessageBoxW(0, text.c_str(), c_name, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
