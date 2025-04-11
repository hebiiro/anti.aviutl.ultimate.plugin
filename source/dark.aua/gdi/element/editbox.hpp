#pragma once

namespace apn::dark::gdi
{
	struct EditBoxRenderer : RendererNc
	{
#if 0
		virtual LRESULT on_subclass_proc(MessageState* current_state) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}",
				current_state->hwnd, current_state->message, current_state->wParam, current_state->lParam);

			return __super::on_subclass_proc(current_state);
		}
#endif
		virtual LRESULT draw_nc_paint(HWND hwnd, HDC dc, const POINT& origin, LPRECT rc) override
		{
			// スピンボタンのバディの場合の処理です。

			auto spin = ::GetWindow(hwnd, GW_HWNDNEXT);
			if (my::get_class_name(spin) == UPDOWN_CLASS)
			{
				auto spin_style = my::get_style(spin);
				if (spin_style & UDS_AUTOBUDDY)
				{
					auto spin_rc = my::get_window_rect(spin);
					auto spin_w = my::get_width(spin_rc);
					auto spin_h = my::get_height(spin_rc);

					if (spin_style & UDS_ALIGNLEFT)
					{
					}
					else if (spin_style & UDS_ALIGNRIGHT)
					{
						if (auto theme = skin::theme::manager.get_theme(VSCLASS_EDIT))
						{
							my::WindowDC dc(hwnd);

							auto client_rc = my::get_client_rect(hwnd);
							auto client_w = my::get_width(client_rc);
							auto window_w = my::get_width(*rc);

							auto fill_rc = *rc;
							fill_rc.left = fill_rc.right - (window_w - client_w) / 2;

							auto part_id = EP_EDITTEXT;
							auto state_id = ::IsWindowEnabled(hwnd) ? ETS_NORMAL : ETS_DISABLED;
							python.call_draw_figure(hwnd, theme, dc, part_id, state_id, &fill_rc);
						}

						rc->right += spin_w;
					}
				}
			}

			return __super::draw_nc_paint(hwnd, dc, origin, rc);
		}

		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}, bk_color = {/hex}, text_color = {/hex}", hwnd, message, dc, control, brush, ::GetBkColor(dc), ::GetTextColor(dc));

			// ブラシもしくは背景色がダイアログカラーの場合は
			if (brush == (HBRUSH)(COLOR_BTNFACE + 1) ||
				::GetBkColor(dc) == ::GetSysColor(COLOR_BTNFACE))
			{
				// スタティックコントロールとして描画します。
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_STATIC))
				{
					auto part_id = STAT_TEXT;
					auto state_id = ::IsWindowEnabled(hwnd) ? ETS_NORMAL : ETS_DISABLED;

					return skin::theme::manager.get_fill_brush(dc, brush, theme, part_id, state_id);
				}
			}
			else
			{
				// エディットボックスとして描画します。
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_EDIT))
				{
					auto part_id = EP_EDITTEXT;
					auto state_id = ::IsWindowEnabled(hwnd) ? ETS_NORMAL : ETS_DISABLED;

					return skin::theme::manager.get_fill_brush(dc, brush, theme, part_id, state_id);
				}
			}

			return __super::on_ctl_color(hwnd, message, dc, control, brush);
		}

		virtual BOOL on_fill_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(MessageState* current_state, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/}, {/hex}", dc, safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(MessageState* current_state, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(MessageState* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), edge, flags);

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(MessageState* current_state, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, ({/})", dc, safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(MessageState* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(MessageState* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
//			MY_TRACE_FUNC("{/hex}, {/}, {/}, options = {/hex}, {/}, {/}, {/}, {/hex}, bk_color = {/hex}, text_color = {/hex}",
//				dc, x, y, options, safe_string(rc), safe_string(text, c, options), c, dx, ::GetBkColor(dc), ::GetTextColor(dc));
#if 1
//			if (!(options & ETO_IGNORELANGUAGE))
			{
				auto bk_color = ::GetBkColor(dc);

				// 背景色がダイアログカラーの場合は
				if (bk_color == ::GetSysColor(COLOR_BTNFACE))
				{
					// スタティックコントロールとして描画します。
					if (auto theme = skin::theme::manager.get_theme(VSCLASS_STATIC))
					{
						auto part_id = STAT_TEXT;
						auto state_id = ::IsWindowEnabled(current_state->hwnd) ? ETS_NORMAL : ETS_DISABLED;

						if (python.call_text_out(current_state->hwnd, theme, dc, part_id, state_id, x, y, options, rc, text, c, dx))
							return TRUE;
					}
				}
				else
				{
					// エディットボックスとして描画します。
					if (auto theme = skin::theme::manager.get_theme(VSCLASS_EDIT))
					{
						auto part_id = EP_EDITTEXT;
						auto state_id = ::IsWindowEnabled(current_state->hwnd) ? ETS_NORMAL : ETS_DISABLED;

						// 選択カラーの場合は
						if (bk_color == ::GetSysColor(COLOR_HIGHLIGHT))
							state_id = ETS_SELECTED; // 選択状態として描画します。

						if (python.call_text_out(current_state->hwnd, theme, dc, part_id, state_id, x, y, options, rc, text, c, dx))
							return TRUE;
					}
				}
			}
#endif
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(MessageState* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
