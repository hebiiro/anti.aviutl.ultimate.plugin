#pragma once

namespace apn::one_for_text
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"one_for_text";
		inline static constexpr auto c_display_name = L"テキスト一括編集";

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
		// 保存したテキストファイルのパスです。
		//
		std::wstring text_file_name;

		//
		// テキストエディタのパスです。
		//
		std::wstring text_editor_path;

		//
		// 適用範囲の先頭レイヤー番号です。
		//
		int32_t layer_front = 1;

		//
		// 適用範囲の末尾レイヤー番号です。
		//
		int32_t layer_back = 100;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
