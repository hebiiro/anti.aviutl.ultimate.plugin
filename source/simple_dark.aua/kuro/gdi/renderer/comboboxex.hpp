#pragma once

namespace apn::dark::kuro::gdi
{
	struct combobox_ex_renderer_t : renderer_t
	{
		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/hex}", hwnd, my::message_to_string(message), w_param, l_param);

			return __super::on_subclass_proc(hwnd, message, w_param, l_param);
		}

		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}, bk_color = {/hex}, text_color = {/hex}",
				hwnd, message, dc, control, brush, ::GetBkColor(dc), ::GetTextColor(dc));

			return __super::on_ctl_color(hwnd, message, dc, control, brush);
		}

		virtual BOOL on_rectangle(message_state_t* current_state, HDC dc, int left, int top, int right, int bottom) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}", dc, left, top, right, bottom);

			// リストボックスアイテムの背景を描画します。
			{
				const auto& palette = paint::listbox_material.palette;

				auto brush = (HBRUSH)::GetCurrentObject(dc, OBJ_BRUSH);
				auto color = paint::get_brush_color(brush);

				auto part_id = EP_EDITTEXT;
				auto state_id = -1;

				if (color == hive.orig.GetSysColor(COLOR_WINDOW)) state_id = ETS_NORMAL;
				else if (color == hive.orig.GetSysColor(COLOR_HIGHLIGHT)) state_id = ETS_SELECTED;

				if (auto pigment = palette.get(part_id, state_id))
				{
					auto rc = RECT { left, top, right, bottom };
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
			MY_TRACE_FUNC("{/hex}, {/}, {/}, options = {/hex}, {/}, {/}, {/}, {/hex}, bk_color = {/hex}, text_color = {/hex}",
				dc, x, y, options, safe_string(rc), safe_string(text, c, options), c, dx, ::GetBkColor(dc), ::GetTextColor(dc));
#if 1
//			if (!(options & (ETO_GLYPH_INDEX | ETO_IGNORELANGUAGE)))
			{
				// リストボックスアイテムの項目を描画します。

				const auto& palette = paint::listbox_material.palette;

				auto part_id = EP_EDITTEXT;
				auto state_id = ::IsWindowEnabled(current_state->hwnd) ? ETS_NORMAL : ETS_DISABLED;

				// 選択カラーの場合は
				if (::GetBkColor(dc) == hive.orig.GetSysColor(COLOR_HIGHLIGHT))
					state_id = ETS_SELECTED; // 選択状態として描画します。

				if (auto pigment = palette.get(part_id, state_id))
					return paint::stylus.ext_text_out(dc, x, y, options, rc, text, c, dx, pigment);
			}
#endif
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(message_state_t* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
