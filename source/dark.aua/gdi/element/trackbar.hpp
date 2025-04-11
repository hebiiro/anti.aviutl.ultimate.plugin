#pragma once

namespace apn::dark::gdi
{
	struct TrackBarRenderer : Renderer
	{
		virtual LRESULT on_subclass_proc(MessageState* current_state) override
		{
			switch (current_state->message)
			{
			case WM_ERASEBKGND:
				{
//					MY_TRACE_FUNC("WM_ERASEBKGND, {/hex}, {/hex}, {/hex}, {/hex}", current_state->hwnd, current_state->message, current_state->wParam, current_state->lParam);

					// ちらつき防止用の処理です。
					// デフォルトの塗りつぶし処理をスキップします。
					return TRUE;
				}
			case WM_LBUTTONDOWN:
				{
//					MY_TRACE_FUNC("WM_LBUTTONDOWN, {/hex}, {/hex}, {/hex}, {/hex}", current_state->hwnd, current_state->message, current_state->wParam, current_state->lParam);

					auto result = __super::on_subclass_proc(current_state);

					// コモンコントロールをバージョン6にすると、
					// WM_LBUTTONDOWNでSB_THUMBTRACKが送られてこなくなります。
					// なので、SB_THUMBTRACKを手動で送ります。
					auto hwnd = current_state->hwnd;
					auto pos = (int)::SendMessage(hwnd, TBM_GETPOS, 0, 0);
					::SendMessage(::GetParent(hwnd), WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, pos), (LPARAM)hwnd);

					return result;
				}
			}

			return __super::on_subclass_proc(current_state);
		}

		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, dc, control, brush);

			// トラックバーの背景の色を変更します。

			if (auto theme = skin::theme::manager.get_theme(VSCLASS_TRACKBAR))
			{
				if (auto state = skin::theme::manager.get_state(theme, TKP_TRACK, TRS_BACKGROUND))
					if (auto brush = state->get_fill_brush()) return brush;
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
//			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			{
				// トラックの選択部分の色を変更します。

				if (::GetBkColor(dc) == ::GetSysColor(COLOR_HIGHLIGHT))
				{
					if (auto theme = skin::theme::manager.get_theme(VSCLASS_TRACKBAR))
					{
						auto state = skin::theme::manager.get_state(theme, TKP_TRACK, TRS_SELECTED);
						if (state && state->stuff.fill.color != CLR_NONE)
							::SetBkColor(dc, state->stuff.fill.color);
					}
				}
			}

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(MessageState* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
