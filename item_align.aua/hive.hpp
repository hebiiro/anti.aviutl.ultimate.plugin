#pragma once

namespace apn::item_align
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"item_align";
		inline static constexpr auto c_display_name = L"アイテム整列";

		struct App {
			virtual BOOL shift_selection() = 0;
			virtual BOOL move_current_frame_to_start() = 0;
			virtual BOOL move_current_frame_to_end() = 0;
			virtual BOOL move_selection_start() = 0;
			virtual BOOL move_selection_end() = 0;
			virtual BOOL stretch_selection_start() = 0;
			virtual BOOL stretch_selection_end() = 0;
			virtual BOOL delete_midpt() = 0;
			virtual BOOL fix_selection_bpm() = 0;
		} *app = 0;

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
		// アイテムをずらす量(フレーム数)です。
		//
		int32_t frame = 0;

		//
		// TRUEの場合は現在フレームを基準にします。
		//
		BOOL use_current_frame = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int message_box(const std::wstring& text, HWND hwnd = 0, int type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
