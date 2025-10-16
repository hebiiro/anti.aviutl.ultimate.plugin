#pragma once

namespace apn::slimming
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"slimming";
		inline static constexpr auto c_display_name = L"スリム化";

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
		// スリムバー(を持つウィンドウ)です。
		//
		my::slimbar_t slimbar;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_display_name, hwnd, type);
		}
	} hive;
}
