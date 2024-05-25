#pragma once

namespace apn::dark
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"dark.pyd";
		inline static constexpr auto c_display_name = L"ダークモード化(Python)";

		//
		// このモジュールのインスタンスハンドルです。
		//
		HINSTANCE instance = 0;

		//
		// ダークモード化アドインのエクスポートシンボルです。
		//
		share::Exports exports;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = 0, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
