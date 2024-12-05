#pragma once

namespace apn::reboot::spreader
{
	//
	// このクラスはスプレッダーのアジトです。
	//
	inline struct Agit
	{
		inline static constexpr struct Message {
			inline static constexpr uint32_t c_post_init = WM_APP + 100;
			inline static constexpr uint32_t c_show_window = WM_APP + 101;
		} c_message;

		inline static constexpr struct StretchMode {
			inline static constexpr uint32_t c_none = 0;
			inline static constexpr uint32_t c_fit = 1;
			inline static constexpr uint32_t c_crop = 2;
			inline static constexpr uint32_t c_full = 3;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_fit, L"fit" },
				{ c_crop, L"crop" },
				{ c_full, L"full" },
			};
		} c_stretch_mode;

		inline static constexpr struct SizeMode {
			inline static constexpr uint32_t c_relative = 0;
			inline static constexpr uint32_t c_absolute = 1;
			inline static constexpr my::Label labels[] = {
				{ c_relative, L"relative" },
				{ c_absolute, L"absolute" },
			};
		} c_size_mode;

		//
		// TRUEの場合はプレビューを拡大します。
		//
		BOOL enabled = FALSE;

		//
		// TRUEの場合は選択範囲だけを再生します。
		//
		BOOL play_select_frame = FALSE;

		//
		// TRUEの場合はループ再生します。
		//
		BOOL play_loop = FALSE;

		//
		// TRUEの場合はフレームを表示します。
		//
		BOOL show_frame = TRUE;

		//
		// TRUEの場合は時間を表示します。
		//
		BOOL show_time = TRUE;

		//
		// TRUEの場合はサムネイルを表示します。
		//
		BOOL show_thumbnail = FALSE;

		//
		// 伸縮モードです。
		// c_noneの場合は原寸大で表示します。
		// c_fitの場合は内接するように拡大します。
		// c_cropの場合は外接するように拡大します。
		// c_fullの場合はアスペクト比を無視して拡大します。
		//
		uint32_t stretch_mode = c_stretch_mode.c_none;

		//
		// サイズモードです。
		// c_relativeの場合はサイズを相対値で指定します。
		// c_absoluteの場合はサイズを絶対値で指定します。
		//
		uint32_t size_mode = c_size_mode.c_relative;

		//
		// サイズの絶対値です。
		// 0の場合は原寸大になります。
		//
		SIZE absolute_size = {};

		//
		// サイズの絶対値です。
		// %単位で指定します。100の場合は原寸大になります。
		//
		int32_t relative_size = 100;

		//
		// サムネイルにする縮小率を指定します。
		// %単位で指定します。
		//
		int32_t thumbnail_scale[3] = { 300, 0, 0 };

		//
		// aviutl側のオプションです。
		// TRUEの場合は再生ウィンドウでプレビューを表示します。
		//
		BOOL movieplaymain = FALSE;

		//
		// 再生中にプレビューを表示するウィンドウです。
		// オプション次第でaviutlウィンドウか再生ウィンドウかになります。
		//
		HWND preview_window = nullptr;
	} agit;
}
