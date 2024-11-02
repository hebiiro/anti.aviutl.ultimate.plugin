#pragma once

namespace apn::output_check
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"output_check";
		inline static constexpr auto c_display_name = L"出力確認";

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
		// TRUEの場合は出力範囲をチェックします。
		//
		BOOL check_range = TRUE;

		//
		// TRUEの場合は最終フレームをチェックします。
		//
		BOOL check_last_frame = TRUE;

		//
		// TRUEの場合はフレームレートをチェックします。
		//
		BOOL check_frame_rate = TRUE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
