#pragma once

namespace apn::dark::skin
{
	inline struct Dwm
	{
		enum MY_DWMWINDOWATTRIBUTE
		{
			DWMWA_USE_IMMERSIVE_DARK_MODE = 20,
			DWMWA_WINDOW_CORNER_PREFERENCE = 33,
			DWMWA_BORDER_COLOR = 34,
			DWMWA_CAPTION_COLOR = 35,
			DWMWA_TEXT_COLOR = 36,
			DWMWA_VISIBLE_FRAME_BORDER_THICKNESS,
			DWMWA_LAST
		};

		//
		// TRUEの場合はDWMを利用できます。
		//
		BOOL available = FALSE;

		//
		// ウィンドウのダークモードです。
		//
		int dark_mode = -1;

		//
		// ウィンドウのコーナーモードです。
		//
		int corner_mode = -1;

		//
		// NC領域の配色です。
		//
		struct {
			COLORREF border_color = CLR_NONE;
			COLORREF caption_color = CLR_NONE;
			COLORREF text_color = CLR_NONE;
		} active, inactive;

		//
		// ポップアップメニューの設定です。
		//
		struct {
			//
			// ポップアップメニューのコーナーモードです。
			//
			int corner_mode = -1;

			//
			// ポップアップメニューのボーダーの色です。
			//
			COLORREF border_color = CLR_NONE;
		} popup_menu;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			available = is_available(hive.main_window);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// DWMが使用可能な場合はTRUEを返します。
		//
		BOOL is_available(HWND hwnd)
		{
			MY_TRACE_FUNC("{:#010x}", hwnd);

			// トップレベルウィンドウを取得します。
			while (auto parent = ::GetParent(hwnd))
				hwnd = parent;

			int corner_mode = 0;
			return S_OK == ::DwmGetWindowAttribute(hwnd,
				DWMWA_WINDOW_CORNER_PREFERENCE , &corner_mode, sizeof(corner_mode));
		}

		//
		// 指定されたウィンドウのアトリビュートを設定します。
		//
		void set_window_attribute(HWND hwnd, BOOL activate)
		{
//			MY_TRACE_FUNC("{:#010x}, {}", hwnd, activate);

			if (!available) return;

			if (activate)
			{
				if (active.border_color != CLR_NONE) ::DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR , &active.border_color, sizeof(active.border_color));
				if (active.caption_color != CLR_NONE) ::DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR , &active.caption_color, sizeof(active.caption_color));
				if (active.text_color != CLR_NONE) ::DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR , &active.text_color, sizeof(active.text_color));
			}
			else
			{
				if (inactive.border_color != CLR_NONE) ::DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR , &inactive.border_color, sizeof(inactive.border_color));
				if (inactive.caption_color != CLR_NONE) ::DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR , &inactive.caption_color, sizeof(inactive.caption_color));
				if (inactive.text_color != CLR_NONE) ::DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR , &inactive.text_color, sizeof(inactive.text_color));
			}

			if (dark_mode != -1) ::DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE , &dark_mode, sizeof(dark_mode));

			if (hive.as_round)
			{
				if (corner_mode != -1) ::DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE , &corner_mode, sizeof(corner_mode));
			}
			else
			{
				auto corner_mode = DWMWCP_DONOTROUND;
				::DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE, &corner_mode, sizeof(corner_mode));
			}
		}
	} dwm;
}
