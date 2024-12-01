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

		//
		// TRUEの場合はプレビューを拡大します。
		//
		BOOL enabled = FALSE;

		//
		// TRUEの場合はフレームを表示します。
		//
		BOOL show_frame = TRUE;

		//
		// TRUEの場合は時間を表示します。
		//
		BOOL show_time = TRUE;

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
