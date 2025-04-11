#pragma once

namespace apn::dark::gdi
{
	struct ListViewRenderer : Renderer
	{
		virtual BOOL on_fill_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			{
				// 背景色を描画します。

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_LISTVIEW))
				{
					auto color = painter::get_brush_color(brush);
					auto part_id = LVP_LISTITEM;
					auto state_id = LISS_NORMAL;

					if (color == ::GetSysColor(COLOR_HIGHLIGHT))
					{
						state_id = LISS_HOT;
					}
					else if (color == ::GetSysColor(COLOR_BTNFACE))
					{
						state_id = LISS_SELECTEDNOTFOCUS;
					}

					MY_TRACE("color = {/hex}, part_id = {/}, state_id = {/}\n", color, (int)part_id, (int)state_id);

					if (python.call_draw_figure(current_state->hwnd, theme, dc, part_id, state_id, rc))
						return TRUE;
				}
			}

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

			return TRUE; // フォーカス矩形を描画しないようにします。
//			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(MessageState* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(MessageState* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));
#if 1
//			if (!(options & ETO_IGNORELANGUAGE))
			{
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_LISTVIEW))
				{
					if (options == ETO_OPAQUE)
					{
						// セパレータのカラーを指定します。

						auto state = skin::theme::manager.get_state(theme, LVP_LISTITEM, 0);

						if (state && state->stuff.fill.color != CLR_NONE)
						{
							::SetBkColor(dc, state->stuff.fill.color);

							return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
						}
					}

					auto color = ::GetPixel(dc, x, y);
					auto part_id = LVP_LISTITEM;
					auto state_id = LISS_NORMAL;
					if (auto state = skin::theme::manager.get_state(theme, part_id, state_id))
					{
						if (color != state->stuff.fill.color)
							state_id = LISS_HOT;
					}

					MY_TRACE("color = {/hex}, part_id = {/}, state_id = {/}\n", color, (int)part_id, (int)state_id);

					if (python.call_text_out(current_state->hwnd, theme, dc, part_id, state_id, x, y, options, rc, text, c, dx))
						return TRUE;
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
