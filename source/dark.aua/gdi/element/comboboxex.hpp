#pragma once

namespace apn::dark::gdi
{
	struct ComboBoxExRenderer : Renderer
	{
		virtual LRESULT on_subclass_proc(MessageState* current_state) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}",
				current_state->hwnd, current_state->message, current_state->wParam, current_state->lParam);

			return __super::on_subclass_proc(current_state);
		}

		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}, bk_color = {/hex}, text_color = {/hex}",
				hwnd, message, dc, control, brush, ::GetBkColor(dc), ::GetTextColor(dc));

			return __super::on_ctl_color(hwnd, message, dc, control, brush);
		}

		virtual BOOL on_rectangle(MessageState* current_state, HDC dc, int left, int top, int right, int bottom) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}", dc, left, top, right, bottom);

			// リストボックスアイテムの背景を描画します。

			if (auto theme = skin::theme::manager.get_theme(VSCLASS_LISTBOX))
			{
				auto brush = (HBRUSH)::GetCurrentObject(dc, OBJ_BRUSH);
				auto color = painter::get_brush_color(brush);

				auto part_id = EP_EDITTEXT;
				auto state_id = -1;

				if (color == ::GetSysColor(COLOR_WINDOW)) state_id = ETS_NORMAL;
				else if (color == ::GetSysColor(COLOR_HIGHLIGHT)) state_id = ETS_SELECTED;

				if (state_id != -1)
				{
					auto rc = RECT { left, top, right, bottom };
					if (python.call_draw_figure(current_state->hwnd, theme, dc, part_id, state_id, &rc))
						return TRUE;
				}
			}

			return hive.orig.Rectangle(dc, left, top, right, bottom);
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
			MY_TRACE_FUNC("{/hex}, {/}, {/}, options = {/hex}, {/}, {/}, {/}, {/hex}, bk_color = {/hex}, text_color = {/hex}",
				dc, x, y, options, safe_string(rc), safe_string(text, c, options), c, dx, ::GetBkColor(dc), ::GetTextColor(dc));
#if 1
//			if (!(options & (ETO_GLYPH_INDEX | ETO_IGNORELANGUAGE)))
			{
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_LISTBOX))
				{
					auto part_id = EP_EDITTEXT;
					auto state_id = ::IsWindowEnabled(current_state->hwnd) ? ETS_NORMAL : ETS_DISABLED;

					// 選択カラーの場合は
					if (::GetBkColor(dc) == ::GetSysColor(COLOR_HIGHLIGHT))
						state_id = ETS_SELECTED; // 選択状態として描画します。

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
