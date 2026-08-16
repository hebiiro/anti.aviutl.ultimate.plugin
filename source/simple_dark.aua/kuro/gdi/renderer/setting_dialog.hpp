#pragma once

namespace apn::dark::kuro::gdi
{
	struct setting_dialog_renderer_t : renderer_t
	{
		const paint::palette_t palette = paint::dialog_material.palette;

		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
//			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, w_param, l_param);

			switch (message)
			{
			case WM_ERASEBKGND:
				{
//					MY_TRACE_FUNC("WM_ERASEBKGND, {/hex}, {/hex}", w_param, l_param);

					// 「設定ダイアログ画面サイズ固定化プラグイン」用の処理です。

					auto dc = (HDC)w_param;
					auto rc = my::get_client_rect(hwnd);

					return paint::stylus.draw_rect(dc, &rc, palette, WP_DIALOG, ETS_NORMAL);
				}
			}

			return __super::on_subclass_proc(hwnd, message, w_param, l_param);
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			if (brush == (HBRUSH)(COLOR_BTNFACE + 1))
				return paint::stylus.draw_rect(dc, rc, palette, WP_DIALOG, ETS_NORMAL);

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			// 「設定ダイアログ画面サイズ固定化プラグイン」用の処理です。
			// ::ExtTextOut()を使用した塗りつぶし処理をフックします。

			if (options == ETO_OPAQUE)
			{
				if (auto bk_color = ::GetBkColor(dc); bk_color == ::GetSysColor(COLOR_BTNFACE))
					return paint::stylus.draw_rect(dc, rc, palette, WP_DIALOG, ETS_NORMAL);
			}

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}
	};
}
