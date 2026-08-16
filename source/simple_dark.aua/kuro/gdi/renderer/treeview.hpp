#pragma once

namespace apn::dark::kuro::gdi
{
	struct treeview_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::treeview_material.palette;

		virtual LRESULT on_custom_draw(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
//			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, w_param, l_param);
#if 1
			// テキストの色と背景色を指定します。(選択アイテム以外)

			auto cd = (NMTVCUSTOMDRAW*)l_param;

			switch (cd->nmcd.dwDrawStage)
			{
			case CDDS_PREPAINT:
				{
					return CDRF_NOTIFYITEMDRAW;
				}
			case CDDS_ITEMPREPAINT:
				{
					MY_TRACE_HEX(cd->nmcd.uItemState);

					auto part_id = TVP_TREEITEM;
					auto state_id = TREIS_NORMAL;

					if (cd->nmcd.uItemState & CDIS_DISABLED)
					{
						state_id = TREIS_DISABLED;
					}
					else if (cd->nmcd.uItemState & CDIS_SELECTED)
					{
						if (cd->nmcd.uItemState & TREIS_HOT)
						{
							state_id = TREIS_HOTSELECTED;
						}
						else if (cd->nmcd.uItemState & CDIS_FOCUS)
						{
							state_id = TREIS_SELECTED;
						}
						else
						{
							state_id = TREIS_SELECTEDNOTFOCUS;
						}
					}
					else if (cd->nmcd.uItemState & TREIS_HOT)
					{
						state_id = TREIS_HOT;
					}

					if (auto pigment = palette.get(part_id, state_id))
					{
						if (pigment->background.is_valid())
							cd->clrTextBk = pigment->background.get_win32_color();

						if (pigment->text.is_valid())
							cd->clrText = pigment->text.get_win32_color();
					}

					return CDRF_NEWFONT;
				}
			}
#endif
			return __super::on_custom_draw(hwnd, message, w_param, l_param);
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
//			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			// アイテムがない部分の背景色を描画します。
			{
				if (auto pigment = palette.get(TVP_TREEITEM, TREIS_NORMAL))
				{
					if (pigment->background.is_valid())
						return paint::stylus.draw_rect(dc, rc, pigment), TRUE;
				}
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

			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(message_state_t* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
//			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(message_state_t* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
