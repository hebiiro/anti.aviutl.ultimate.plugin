#pragma once

namespace apn::dark::kuro::gdi
{
	struct listview_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::listview_material.palette;

		//
		// カスタムドローで指定されている配色です。
		//
		COLORREF custom_text_color = CLR_DEFAULT;
		COLORREF custom_background_color = CLR_DEFAULT;

#if 0 // テスト用コードです。
		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_ERASEBKGND:
				{
					MY_TRACE_FUNC("WM_ERASEBKGND, {/hex}, {/hex}, {/hex}, {/hex}",
						current_state->hwnd, current_state->message, current_state->w_param, current_state->l_param);

					break;
				}
			}

			return __super::on_subclass_proc(hwnd, message, w_param, l_param);
		}
#endif

		virtual LRESULT on_custom_draw(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
//			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, w_param, l_param);

			auto cd = (NMTVCUSTOMDRAW*)l_param;
			auto result = __super::on_custom_draw(hwnd, message, w_param, l_param);

			switch (cd->nmcd.dwDrawStage)
			{
			case CDDS_PREPAINT:
				{
					if (result != CDRF_NOTIFYITEMDRAW)
					{
						custom_text_color = CLR_DEFAULT;
						custom_background_color = CLR_DEFAULT;
					}

					break;
				}
			case CDDS_ITEMPREPAINT:
				{
					custom_text_color = cd->clrText;
					custom_background_color = cd->clrTextBk;

					break;
				}
			}

			return result;
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			if (custom_background_color == CLR_DEFAULT)
			{
				// 背景色を描画します。

				auto color = paint::get_brush_color(brush);
				auto part_id = LVP_LISTITEM;
				auto state_id = LISS_NORMAL;

				if (color == hive.orig.GetSysColor(COLOR_HIGHLIGHT))
				{
					state_id = LISS_HOT;
				}
				else if (color == hive.orig.GetSysColor(COLOR_BTNFACE))
				{
					state_id = LISS_SELECTEDNOTFOCUS;
				}

				MY_TRACE("color = {/hex}, part_id = {/}, state_id = {/}\n", color, (int)part_id, (int)state_id);

				if (auto pigment = palette.get(part_id, state_id))
					return paint::stylus.draw_rect(dc, rc, pigment);
			}

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

			return TRUE; // フォーカス矩形を描画しないようにします。
//			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(message_state_t* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));
#if 1
//			if (!(options & ETO_IGNORELANGUAGE))
			{
				if (options == ETO_OPAQUE && rc)
				{
					auto part_id = LVP_LISTITEM;
					auto state_id = LISS_NORMAL;

					// セパレータの場合は
					if (my::get_width(*rc) == 1 || my::get_height(*rc) == 1)
						state_id = (decltype(state_id))0;

					if (auto pigment = palette.get(part_id, state_id))
						return paint::stylus.ext_text_out(dc, x, y, options, rc, text, c, dx, pigment);
				}
				else if (custom_text_color == CLR_DEFAULT)
				{
					auto current_color = ::GetPixel(dc, x, y);
					auto part_id = LVP_LISTITEM;
					auto state_id = LISS_NORMAL;

					if (auto pigment = palette.get(part_id, state_id))
					{
						if (current_color != pigment->background.get_win32_color())
							state_id = LISS_HOT;
					}

					MY_TRACE("current_color = {/hex}, part_id = {/}, state_id = {/}\n",
						current_color, (int)part_id, (int)state_id);

					if (auto pigment = palette.get(part_id, state_id))
						return paint::stylus.ext_text_out(dc, x, y, options, rc, text, c, dx, pigment);
				}
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
