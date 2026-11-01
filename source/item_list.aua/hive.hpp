#pragma once

namespace apn::item_list
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct hive_t
	{
		inline static constexpr auto k_name = L"item_list";
		inline static constexpr auto k_display_name = L"アイテム一覧";

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
		// TRUEの場合は位置や範囲を時間として表示します。
		//
		BOOL flag_as_time = FALSE;

		//
		// アイテム一覧をリフレッシュするまでの遅延時間です。
		//
		int refresh_delay = 20;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text,
			HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, k_display_name, hwnd, type);
		}
	} hive;
}
