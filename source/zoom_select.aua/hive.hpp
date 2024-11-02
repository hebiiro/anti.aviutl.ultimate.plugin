#pragma once

namespace apn::zoom_select
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"zoom_select";
		inline static constexpr auto c_display_name = L"ズーム選択";

		inline static constexpr struct Zoom {
			inline static constexpr int32_t c_min = 0;
			inline static constexpr int32_t c_max = 26;
			inline static constexpr int32_t c_max_size = 6;
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
		// ズーム値の配列です。
		//
		int32_t zoom_values[c_zoom.c_max_size] = { 0, 5, 10, 15, 20, 26 };

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
