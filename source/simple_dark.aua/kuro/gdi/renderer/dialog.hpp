#pragma once

namespace apn::dark::kuro::gdi
{
	struct dialog_renderer_t : renderer_nc_t
	{
#if 0 // テスト用コードです。
		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, w_param, l_param);

			return __super::on_subclass_proc(hwnd, message, w_param, l_param);
		}
#endif
		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, dc, control, brush);

			return get_dialog_brush(hwnd, message, dc, control, brush);
		}

		virtual BOOL on_rectangle(message_state_t* current_state, HDC dc, int left, int top, int right, int bottom) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}, {/}, {/}, {/})", dc, left, top, right, bottom);

			// 現在のブラシの色を取得します。
			auto brush = (HBRUSH)::GetCurrentObject(dc, OBJ_BRUSH);
			auto brush_color = paint::get_brush_color(brush);

			// メッセージボックスの下側を描画します。
			if (brush_color == hive.orig.GetSysColor(COLOR_BTNFACE))
//			if (brush_color == hive.orig.GetSysColor(COLOR_MENU))
			{
				// ダイアログとして描画します。
				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ETS_FOOTER;

				if (auto pigment = palette.get(part_id, state_id))
				{
#if 1
					// クライアント矩形を取得します。
					auto rc = my::get_client_rect(current_state->hwnd);

					// topの座標だけ使用して描画します。
					rc.top = top;
#else
					// 引数の座標を使用して描画します。
					auto rc = RECT { left, top, right, bottom };
#endif
					return paint::stylus.draw_rect(dc, &rc, pigment);
				}
			}

			return hive.orig.Rectangle(dc, left, top, right, bottom);
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(message_state_t* current_state, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/}, {/hex}", dc, safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(message_state_t* current_state, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(message_state_t* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), edge, flags);
#if 0
			auto part_id = 0;
			auto state_id = 0;

			switch (edge)
			{
			case EDGE_RAISED: part_id = WP_RAISED_EDGE; break;
			case EDGE_SUNKEN: part_id = WP_SUNKEN_EDGE; break;
			case EDGE_BUMP: part_id = WP_BUMP_EDGE; break;
			case EDGE_ETCHED: part_id = WP_ETCHED_EDGE; break;
			}

			if (part_id != 0)
			{
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_WINDOW))
				{
					auto result = FALSE;

					if (flags & BF_MIDDLE)
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_DIALOG, PBS_NORMAL, rc))
							result |= TRUE;
					}

					if (python.call_draw_figure(current_state->hwnd, theme, dc, part_id, state_id, rc))
					{
						if (flags & BF_ADJUST)
							::InflateRect(rc, -2, -2);

						result |= TRUE;
					}

					if (result) return TRUE;
				}
			}
#endif
			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(message_state_t* current_state, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, ({/})", dc, safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(message_state_t* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(message_state_t* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
