#pragma once

namespace apn::dark::gdi
{
	struct AviUtlRenderer : Renderer
	{
		virtual BOOL on_fill_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
//			MY_TRACE_FUNC("{:#010x}, ({}), {:#010x}", dc, safe_string(rc), brush);

			if (brush == (HBRUSH)(COLOR_BTNFACE + 1))
			{
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_WINDOW))
				{
					if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_DIALOG, ETS_NORMAL, rc))
						return TRUE;
				}
			}

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(MessageState* current_state, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {}, {:#010x}", dc, safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(MessageState* current_state, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {:#010x}, {:#010x}", dc, safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {:#010x}", dc, safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(MessageState* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {:#010x}, {:#010x}", dc, safe_string(rc), edge, flags);

			if (auto theme = skin::theme::manager.get_theme(VSCLASS_WINDOW))
			{
				BOOL result = FALSE;

				if (flags & BF_MIDDLE)
				{
					if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_DIALOG, ETS_NORMAL, rc))
						result |= TRUE;
				}

				switch (edge)
				{
				case BDR_RAISEDOUTER:
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_LIGHT_RAISED_EDGE, 0, rc))
							result |= TRUE;

						break;
					}
				case BDR_SUNKENOUTER:
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_LIGHT_SUNKEN_EDGE, 0, rc))
							result |= TRUE;

						break;
					}
				case EDGE_RAISED:
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_RAISED_EDGE, 0, rc))
							result |= TRUE;

						break;
					}
				case EDGE_SUNKEN:
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_SUNKEN_EDGE, 0, rc))
							result |= TRUE;

						break;
					}
				case EDGE_BUMP:
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_BUMP_EDGE, 0, rc))
							result |= TRUE;

						break;
					}
				case EDGE_ETCHED:
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_ETCHED_EDGE, 0, rc))
							result |= TRUE;

						break;
					}
				}

				if (result) return result;
			}

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(MessageState* current_state, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, ({})", dc, safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(MessageState* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, {}, {}, {:#010x}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(MessageState* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
//			MY_TRACE_FUNC("{:#010x}, {}, {}, {:#010x}, {}, {}, {}, {:#010x}, {:#010x}, {:#010x}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(MessageState* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
