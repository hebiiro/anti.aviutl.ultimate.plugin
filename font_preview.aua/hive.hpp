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

		inline static constexpr struct ControlID {
			inline static constexpr uint32_t c_font_combobox = 0x5654;
		} c_control_id;

		inline static constexpr struct Message {
			inline static const uint32_t c_drop_down = ::RegisterWindowMessage(_T("apn::font_preview::drop_down"));
		} c_message;

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = 0;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// このアドインのメインウィンドウです。
		//
		HWND main_window = 0;

		//
		// プレビューアイテムのサイズです。
		//
		SIZE item_size = { 0, 48 };

		//
		// プレビューウィンドウのサイズです。
		//
		SIZE window_size = { 400, 600 };

		//
		// プレビュー用のサンプル文字列です。
		//
		std::wstring sample = L"サンプル0123456789";

		//
		// TRUEの場合はフォント名とサンプルをまとめて表示します。
		//
		BOOL single_line = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = 0, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
