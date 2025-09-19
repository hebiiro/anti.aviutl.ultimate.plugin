#pragma once

namespace apn::text_split
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr auto c_name = L"text_split";
		inline static constexpr auto c_display_name = L"テキスト分解";

		inline static constexpr struct ShiftMode {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_offset = 1;
			inline static constexpr int32_t c_range = 2;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_offset, L"offset" },
				{ c_range, L"range" },
			};
		} c_shift_mode;

		inline static constexpr struct EachMode {
			inline static constexpr int32_t c_ch = 0;
			inline static constexpr int32_t c_row = 1;
			inline static constexpr my::Label labels[] = {
				{ c_ch, L"ch" },
				{ c_row, L"row" },
			};
		} c_each_mode;

		inline static constexpr struct FitMode {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_auto = 1;
			inline static constexpr int32_t c_manual = 2;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_auto, L"auto" },
				{ c_manual, L"manual" },
			};
		} c_fit_mode;

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
		// シフトモードです。
		//
		int32_t shift_mode = c_shift_mode.c_none;

		//
		// 分割後のアイテムのフレーム位置に加算する値です。
		// シフトモードがc_offsetのときに使用されます。
		//
		int32_t shift_offset = 60;

		//
		// 分割後のアイテムのフレーム位置範囲です。
		// シフトモードがc_rangeのときに使用されます。
		//
		int32_t shift_range = 600;

		//
		// 分割方法です。
		//
		int32_t each_mode = c_each_mode.c_ch;

		//
		// 行端揃えモードです。
		//
		int32_t fit_mode = c_fit_mode.c_none;

		// 
		// 行端揃えモードするテキストの幅です。
		// 行端揃えモードがc_manualのときに使用されます。
		//
		int32_t fit_width = 720;

		//
		// TRUEの場合は元のテキストアイテムを削除します。
		//
		BOOL erase_orig_item = FALSE;

		//
		// メッセージボックスを表示します。
		//
		int32_t message_box(const std::wstring& text, HWND hwnd = nullptr, int32_t type = MB_OK | MB_ICONWARNING) {
			return magi.message_box(text, c_display_name, hwnd, type);
		}
	} hive;
}
