#pragma once

namespace apn::font_preview
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"font_preview";
		inline static constexpr auto c_display_name = L"フォントプレビュー";

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
		// プレビューウィンドウのサイズです。
		//
		SIZE window_size = { 400, 600 };

		//
		// プレビューアイテムの高さです。
		//
		int item_height = 48;

		//
		// プレビューフォントの高さです。
		//
		int font_height = 48;

		//
		// プレビュー用のサンプル文字列です。
		//
		std::wstring sample = L"サンプル0123456789";

		//
		// TRUEの場合はフォント名とサンプルをまとめて表示します。
		//
		BOOL singleline = FALSE;

		//
		// TRUEの場合はコンテキストメニューを表示します。
		//
		BOOL use_context_menu = TRUE;

		//
		// コンテキストメニューを表示するときに各修飾キーを必須にします。
		//
		BOOL use_shift_key = FALSE;
		BOOL use_ctrl_key = FALSE;
		BOOL use_alt_key = FALSE;
		BOOL use_win_key = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_display_name, hwnd, type);
		}
	} hive;
}
