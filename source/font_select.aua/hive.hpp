#pragma once

namespace apn::font_select
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"font_select";
		inline static constexpr auto c_display_name = L"フォント選択";

		inline static constexpr struct ControlID {
			inline static constexpr uint32_t c_font_combobox = 0x5654;
		} c_control_id;

		//
		// この構造体はフォントメニュー用のデータを保持します。
		//
		struct Menu {
			//
			// この構造体はフォントメニューの項目です。
			//
			struct Node {
				uint32_t flags;
				uint32_t command_id;
				std::wstring name;
				std::vector<Node> nodes;
			};
			std::vector<Node> nodes;
		};

		//
		// メニューのルートノードです。
		//
		Menu::Node menu_root;

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
		// プレビューアイテムのサイズです。
		//
		SIZE item_size = { 400, 48 };

		//
		// フォントのサイズです。
		//
		SIZE font_size = { 0, 34 };

		//
		// プレビュー用のサンプル文字列です。
		//
		std::wstring sample = L"サンプル0123456789";

		//
		// TRUEの場合はフォント名とサンプルをまとめて表示します。
		//
		BOOL single_line = FALSE;

		//
		// TRUEの場合はコンテキストメニュー(右クリック)を使用します。
		//
		BOOL use_context_menu = TRUE;

		//
		// TRUEの場合は戻るボタンを使用します。
		//
		BOOL use_backward = TRUE;

		//
		// TRUEの場合は進むボタンを使用します。
		//
		BOOL use_forward = TRUE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
