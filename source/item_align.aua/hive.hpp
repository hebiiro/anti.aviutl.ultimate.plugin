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

		inline static constexpr struct SubTimeMode {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_frame = 1;
			inline static constexpr int32_t c_bpm = 2;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_frame, L"frame" },
				{ c_bpm, L"bpm" },
			};
		} c_sub_time_mode;

		struct App {
			virtual void align_left() = 0;
			virtual void align_right() = 0;
			virtual void stretch_left() = 0;
			virtual void stretch_right() = 0;
			virtual void relative_space_left() = 0;
			virtual void relative_space_right() = 0;
			virtual void absolute_space_left() = 0;
			virtual void absolute_space_right() = 0;
			virtual void shift_up() = 0;
			virtual void shift_down() = 0;
			virtual void move_vert_down() = 0;
			virtual void move_vert_up() = 0;
			virtual void fix_bpm() = 0;
			virtual void erase_midpt() = 0;
		} *app = nullptr;

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
		// TRUEの場合は現在フレームを基準にします。
		//
		BOOL use_current_frame = FALSE;

		//
		// アイテムを寄せる量(時間)です。
		//
		std::wstring align_time = L"0.5";

		//
		// アイテムを伸ばす量(時間)です。
		//
		std::wstring stretch_time = L"0.5";

		//
		// 相対スペース量(時間)です。
		//
		std::wstring relative_space_time = L"0.5";

		//
		// 絶対スペース量(時間)です。
		//
		std::wstring absolute_space_time = L"1";

		//
		// アイテムをずらす量(時間)です。
		//
		std::wstring shift_time = L"0.5";

		//
		// アイテムを詰める量(レイヤー数)です。
		//
		int32_t move_vert_layer = 2;

		//
		// 時間モードです。
		//
		int32_t sub_time_mode = c_sub_time_mode.c_none;

		//
		// アイテムを寄せる量(フレームまたはBPM)です。
		//
		int32_t align_sub_time = 0;

		//
		// アイテムを伸ばす量(フレームまたはBPM)です。
		//
		int32_t stretch_sub_time = 0;

		//
		// 相対スペース量(フレームまたはBPM)です。
		//
		int32_t relative_space_sub_time = 0;

		//
		// 絶対スペース量(フレームまたはBPM)です。
		//
		int32_t absolute_space_sub_time = 0;

		//
		// アイテムをずらす量(フレームまたはBPM)です。
		//
		int32_t shift_sub_time = 0;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text,
			HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, c_name, hwnd, type);
		}
	} hive;
}
