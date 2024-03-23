#pragma once

namespace apn::color_code
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"color_code";
		inline static constexpr auto c_display_name = L"カラーコード追加";

		//
		// このアドインのインスタンスハンドルです。
		//
		HINSTANCE instance = 0;

		//
		// コンフィグのファイル名です。
		//
		std::wstring config_file_name;

		//
		// 拡張編集の「色の選択」ダイアログ関連の値です。
		//
		HWND dialog = 0;
		BOOL* unlock = 0;
		int32_t* r = 0;
		int32_t* g = 0;
		int32_t* b = 0;

		//
		// カラーサークルを動かす量です。
		//
		POINT offset = { 0, 0 };

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = 0, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
