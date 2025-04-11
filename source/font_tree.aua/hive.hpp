#pragma once

namespace apn::font_tree
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"font_tree";
		inline static constexpr auto c_display_name = L"フォントツリー";

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
		// このクラスはお気に入りフォントの設定です。
		//
		struct FavoriteOption {
			//
			// 表示名の書式です。
			//
			std::wstring display_name_format = L"%alias% --- %name%";

			//
			// セパレータの書式です。
			//
			std::wstring separator_format = L"---------";
		} favorite;

		//
		// このクラスはプレビューの設定です。
		//
		struct PreviewOption {
			inline static constexpr struct Mode {
				inline static constexpr int32_t c_none = 0;
				inline static constexpr int32_t c_left = 1;
				inline static constexpr int32_t c_right = 2;
				inline static constexpr my::Label labels[] = {
					{ c_none, L"none" },
					{ c_left, L"left" },
					{ c_right, L"right" },
				};
			} c_mode;

			//
			// プレビューの表示モードです。
			//
			int32_t mode = c_mode.c_left;

			//
			// プレビュー用文字列の書式です。
			//
			std::wstring sample_format = L"プレビュー(%font_name%)";

			//
			// プレビュー項目のサイズです。
			//
			SIZE item_size = { 600, 36 };

			//
			// プレビュー項目の背景色です。
			//
			COLORREF fill_color = RGB(0x00, 0x00, 0x00);

			//
			// プレビュー項目の文字色です。
			//
			COLORREF text_color = RGB(0xff, 0xff, 0xff);
		} preview;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
