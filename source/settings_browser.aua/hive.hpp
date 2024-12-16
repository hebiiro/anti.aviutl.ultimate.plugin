#pragma once

namespace apn::settings_browser
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"settings_browser";
		inline static constexpr auto c_display_name = L"設定ブラウザ";

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// アセットのフォルダ名です。
		//
		std::wstring assets_folder_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// TRUEの場合はセッティングを自動的に取得します。
		//
		BOOL auto_get = FALSE;

		//
		// コンテンツのURLです。
		//
		std::wstring url = L"default.html";

		//
		// コンテンツとして表示するjsonです。
		//
		std::wstring settings_json;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text,
			HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, c_display_name, hwnd, type);
		}
	} hive;
}
