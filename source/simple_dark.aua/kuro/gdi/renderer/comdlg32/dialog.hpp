#pragma once

namespace apn::dark::kuro::gdi::comdlg32
{
	struct dialog_renderer_t : gdi::dialog_renderer_t
	{
		inline static constexpr auto c_top_combobox_id = 0x00000471;
		inline static constexpr auto c_top_right_toolbar_id = 0x00000440;
		inline static constexpr auto c_left_toolbar_id = 0x000004A0;

		//
		// ダイアログ上部に配置されているコンボボックスです。
		//
		HWND top_combobox = {};

		//
		// ダイアログ右上に配置されているツールバーです。
		//
		HWND top_right_toolbar = {};

		//
		// ダイアログ左側に配置されているツールバーです。
		//
		HWND left_toolbar = {};

		//
		// コモンダイアログが開いたときの処理です。
		//
		virtual BOOL on_attach(HWND hwnd)
		{
			// 現在表示されているコモンダイアログの数を増やします。
			hive.comdlg32_visible_count++;

			return __super::on_attach(hwnd);
		}

		//
		// コモンダイアログが閉じたときの処理です。
		//
		virtual BOOL on_detach(HWND hwnd)
		{
			// スレットの有効性を取得しておきます。
			auto is_valid_thread = hive.is_valid_thread();

			// 現在表示されているコモンダイアログの数を減らします。
			hive.comdlg32_visible_count--;

			// スレットの有効性が変化した場合は
			if (is_valid_thread != hive.is_valid_thread())
			{
				// すべてのウィンドウを再描画します。
				redraw_windows();
			}

			return __super::on_detach(hwnd);
		}

		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/hex}", hwnd, my::message_to_string(message), w_param, l_param);

			switch (message)
			{
			case WM_PARENTNOTIFY:
				{
					if (LOWORD(w_param) == WM_CREATE && HIWORD(w_param) == c_top_right_toolbar_id)
					{
						top_combobox = ::GetDlgItem(hwnd, c_top_combobox_id);
						MY_TRACE_HWND(top_combobox);

						left_toolbar = ::GetDlgItem(hwnd, c_left_toolbar_id);
						MY_TRACE_HWND(left_toolbar);

						top_right_toolbar = (HWND)l_param;
						MY_TRACE_HWND(top_right_toolbar);
					}

					break;
				}
			case WM_INITDIALOG:
				{
#if 1
					my::scope_exit scope_exit([&]()
					{
#if 1
						auto toolbar_rc = my::get_window_rect(top_right_toolbar);
						auto combobox_rc = my::get_window_rect(top_combobox);
						auto h = my::get_height(combobox_rc) + (combobox_rc.top - toolbar_rc.top) * 2;

						// ボタンの位置が上部コンボボックスと水平になるようにします。
						::SendMessage(top_right_toolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(h, h));
#elif 1
						auto c = (int)::SendMessage(top_right_toolbar, TB_BUTTONCOUNT, 0, 0);

						auto rc = my::get_client_rect(top_right_toolbar);
						auto w = my::get_width(rc) / (c + 1);
						auto h = my::get_height(rc);

						// ボタンサイズをクライアント領域いっぱいに広げます。
						::SendMessage(top_right_toolbar, TB_SETBUTTONSIZE, 0, MAKELPARAM(w, h));
#else
						// DPIを取得します。
						auto dpi = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

						// ボタンサイズをDPIで補正します。
						auto size = my::lp_to_pt(::SendMessage(top_right_toolbar, TB_GETBUTTONSIZE, 0, 0));
						size.x = ::MulDiv(size.x, dpi, USER_DEFAULT_SCREEN_DPI);
						size.y = ::MulDiv(size.y, dpi, USER_DEFAULT_SCREEN_DPI);
						::SendMessage(top_right_toolbar, TB_SETBUTTONSIZE, 0, my::pt_to_lp(size));
#endif
					});
#endif
					return __super::on_subclass_proc(hwnd, message, w_param, l_param);
				}
			}

			return __super::on_subclass_proc(hwnd, message, w_param, l_param);
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			// ブラシがカラーIDの場合は
			if (!HIWORD(brush))
			{
				// カラーIDに対応するブラシを返すようにします。

				auto color_id = LOWORD(brush) - 1;

				if (auto pigment = paint::sys_color_material.palette.get(color_id, 0))
					brush = pigment->background.get_brush();
			}

			return __super::on_fill_rect(current_state, dc, rc, brush);
		}
#ifdef _DEBUG // テスト用コードです。
		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, bk_color = {/hex}, text_color = {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			return __super::on_ext_text_out_w(current_state, dc, x, y, options, rc, text, c, dx);
		}
#endif
		virtual COLORREF on_get_sys_color(int color_id) override
		{
			MY_TRACE_FUNC("{/}", color_id);

			if (auto pigment = paint::sys_color_material.palette.get(color_id, 0))
				return pigment->background.get_win32_color();

			return __super::on_get_sys_color(color_id);
		}

		virtual HBRUSH on_get_sys_color_brush(int color_id) override
		{
			MY_TRACE_FUNC("{/}", color_id);

			if (auto pigment = paint::sys_color_material.palette.get(color_id, 0))
				return pigment->background.get_brush();

			return __super::on_get_sys_color_brush(color_id);
		}
	};
}
