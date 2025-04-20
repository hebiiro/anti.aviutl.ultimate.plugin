#pragma once

namespace apn::selector
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"selector";
		inline static constexpr auto c_display_name = L"セレクタ";

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
		// aviutlウィンドウです。
		//
		HWND aviutl_window = nullptr;

		//
		// アイテムの長さです。
		// タイムラインにアイテムを配置するときに初期値として使用されます。
		//
		int32_t item_length = 300;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_display_name, hwnd, type);
		}
	} hive;
}
