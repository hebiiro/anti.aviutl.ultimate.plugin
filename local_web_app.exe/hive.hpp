#pragma once

namespace apn::local_web_app
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"local_web_app";
		inline static constexpr auto c_display_name = L"ローカルウェブアプリ";

		//
		// インスタンスハンドルです。
		//
		HINSTANCE instance = nullptr;

		//
		// メインウィンドウです。
		//
		HWND main_window = nullptr;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// ローカルコンテンツリソースの起点フォルダです。
		//
		std::filesystem::path origin_folder_path;

		//
		// コンテンツのURLです。
		//
		std::wstring url = L"";

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
