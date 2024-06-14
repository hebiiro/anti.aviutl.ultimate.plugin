#pragma once

namespace apn::item_wave::ui
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"item_wave_ui";
		inline static constexpr auto c_display_name = L"アイテム波形のUI";

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// UIプロセスのウィンドウです。
		//
		HWND ui_window = nullptr;

		//
		// ホストプロセスのウィンドウです。
		//
		HWND host_window = nullptr;

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
