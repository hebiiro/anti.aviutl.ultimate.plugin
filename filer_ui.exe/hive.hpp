#pragma once

namespace apn::filer_ui
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"filer_ui";
		inline static constexpr auto c_display_name = L"ファイラのUI";

		inline static constexpr struct TimerID {
			inline static constexpr uint32_t c_check_host_window = 1000;
			inline static constexpr uint32_t c_check_config = 1001;
		} c_timer_id;

		struct App {
			virtual void on_init() = 0;
			virtual void on_exit() = 0;
			virtual void on_init_filer_window(HWND filer_window, BOOL full) = 0;
			virtual void on_check_host_window() = 0;
			virtual void on_check_config() = 0;
		} *app = 0;

		//
		// このモジュールのインスタンスハンドルです。
		//
		HINSTANCE instance = 0;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// AviUtlプロセス内に存在するホストウィンドウです。
		//
		HWND host_window = 0;

		//
		// このモジュールのメインウィンドウです。
		//
		HWND main_window = 0;

		//
		// メッセージボックスを表示します。
		//
		int message_box(const std::wstring& text, int type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text.c_str(), c_name, host_window, type);
		}
	} hive;
}
