﻿#pragma once

namespace apn::item_copy
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"item_copy";
		inline static constexpr auto c_display_name = L"アイテムコピー";

		inline static constexpr struct FileSplitMode {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_item_exo = 1;
			inline static constexpr int32_t c_item_exa = 2;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_item_exo, L"item_exo" },
				{ c_item_exa, L"item_exa" },
			};
		} c_file_split_mode;

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
		// 挿入位置のレイヤーです。
		//
		int32_t insert_layer = 1;

		//
		// 作成するexoのプレフィックスです。
		//
		std::wstring prefix;

		//
		// 選択テキストのみを使用します。
		//
		BOOL use_empty_text = FALSE;

		//
		// ファイルに書き込むときの分割モードです。
		//
		int32_t file_split_mode = c_file_split_mode.c_none;

		//
		// 読み込んだexoファイルのパスです。
		//
		std::wstring exo_file_name;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
