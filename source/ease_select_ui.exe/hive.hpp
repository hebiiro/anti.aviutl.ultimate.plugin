#pragma once

namespace apn::ease_select_ui
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"ease_select_ui";
		inline static constexpr auto c_display_name = L"イージング選択のUI";

		struct App {
			virtual BOOL read_config() = 0;
			virtual BOOL write_config() = 0;
		} *app = nullptr;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このモジュールのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// ホストプロセス(AviUtlプロセス)のIDです。
		//
		DWORD host_process_id = 0;

		//
		// AviUtlプロセス内に存在するホストウィンドウです。
		//
		HWND host_window = nullptr;

		//
		// TRUEの場合はすでにコンフィグが読み込まれています。
		//
		BOOL config_loaded = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
