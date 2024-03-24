#pragma once

namespace apn
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr LPCWSTR c_name = L"editbox_tweaker";
		inline static constexpr LPCWSTR c_display_name = L"エディットボックス微調整";

		HINSTANCE instance = 0;

		bool unicode_input = true;
		bool ctrl_a = true;
		bool zoomable = true;
		int delta = 0;
		struct Font {
			std::wstring name = L"";
			int height = 0;
			int pitch = 0;
			std::shared_ptr<std::remove_pointer<HFONT>::type> handle;
		} font;

		int message_box(const std::wstring& text) {
			return ::MessageBoxW(0, text.c_str(), c_name, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
