#pragma once

namespace apn::clipboard_viewer
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"clipboard_viewer";
		inline static constexpr auto c_display_name = L"クリップボード画像表示";

		inline static constexpr struct Zoom {
			inline static constexpr auto c_default = 100;
			inline static constexpr auto c_min = 25;
			inline static constexpr auto c_max = 800;
		} c_zoom;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// 画像の表示倍率です。
		//
		int32_t zoom = c_zoom.c_default;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
