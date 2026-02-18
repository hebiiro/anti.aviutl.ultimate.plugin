#pragma once

namespace apn::timeline_map
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t
	{
		inline static constexpr auto c_name = L"timeline_map";
		inline static constexpr auto c_display_name = L"タイムラインマップ";

		//
		// このモジュールのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// このモジュールのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text,
			HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
