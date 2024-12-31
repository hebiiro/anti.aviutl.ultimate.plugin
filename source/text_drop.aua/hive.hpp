#pragma once

namespace apn::text_drop
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"text_drop";
		inline static constexpr auto c_display_name = L"テキストドロップ";

		inline static constexpr struct Mode {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_whole = 1;
			inline static constexpr int32_t c_lines = 2;
			inline static constexpr int32_t c_chars = 3;
			inline static constexpr int32_t c_blocks = 4;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_whole, L"whole" },
				{ c_lines, L"lines" },
				{ c_chars, L"chars" },
				{ c_blocks, L"blocks" },
			};
		} c_mode;

		inline static constexpr struct Direction {
			inline static constexpr int32_t c_horz = 0;
			inline static constexpr int32_t c_vert = 1;
			inline static constexpr my::Label labels[] = {
				{ c_horz, L"horz" },
				{ c_vert, L"vert" },
			};
		} c_direction;

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
		// 作成するアイテムの長さ(フレーム数)です。
		//
		int32_t item_range = 300;

		//
		// アイテムの間隔(フレーム数)です。
		//
		int32_t space_range = 0;

		//
		// 作成するアイテムの文字毎の長さ(フレーム数)です。
		//
		int32_t char_range = 0;

		//
		// アイテムの配置方向です。
		//
		int32_t item_direction = c_direction.c_horz;

		//
		// 作成するexoのファイル名です。
		//
		std::wstring exo_file_name;

		//
		// ドロップするテキストです。
		//
		std::wstring drop_text;

		//
		// 挿入位置のレイヤーです。
		//
		int32_t insert_layer = 1;

		//
		// 選択テキストのみを使用します。
		//
		BOOL selected_text_only = TRUE;

		//
		// 読み込んだテキストファイルのパスです。
		//
		std::wstring text_file_name;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
