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

		//
		// ステートIDを返します。
		//
		inline static COLORREF get_state_id(HDC dc)
		{
			auto bk_color = ::GetBkColor(dc);
			MY_TRACE_HEX(bk_color);

			// 背景色がホットライトの場合は
			if (bk_color == hive.orig.GetSysColor(COLOR_HOTLIGHT))
			{
				return LISS_HOT;
			}
			// 背景色がハイライトの場合は
			else if (bk_color == hive.orig.GetSysColor(COLOR_HIGHLIGHT))
			{
				return LISS_SELECTED;
			}
			// 背景色がボタンの場合は
			else if (bk_color == hive.orig.GetSysColor(COLOR_BTNFACE) ||
				bk_color == hive.orig.GetSysColor(COLOR_BTNTEXT))
			{
				return LISS_SELECTEDNOTFOCUS;
			}
			// 背景色がウィンドウの場合は
			else if (bk_color == hive.orig.GetSysColor(COLOR_WINDOW))
			{
				return LISS_NORMAL;
			}

			return -1;
		}

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
						// カスタムカラーをリセットします。
						custom_text_color = CLR_DEFAULT;
						custom_background_color = CLR_DEFAULT;
					}

					break;
				}
			case CDDS_ITEMPREPAINT:
				{
					// カスタムカラーをセットします。
					custom_text_color = cd->clrText;
					custom_background_color = cd->clrTextBk;

					break;
				}
			}

			return result;
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("dc = {/hex}, rc = ({/}), brush = {/hex}, bk_color = {/hex}, text_color = {/hex}",
				dc, safe_string(rc), brush, ::GetBkColor(dc), ::GetTextColor(dc));

			// カスタムカラーではない場合は
			if (custom_background_color == CLR_DEFAULT)
			{
				// 背景色を描画します。

				auto part_id = LVP_LISTITEM;
				auto state_id = get_state_id(dc);

				// ステートIDが有効の場合は
				if (state_id != -1)
				{
					// 背景を独自に描画します。
					if (auto pigment = palette.get(part_id, state_id))
						return paint::stylus.draw_rect(dc, rc, pigment);
				}
			}

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_focus_rect(message_state_t* current_state, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, ({/})", dc, safe_string(rc));

			return TRUE; // フォーカス矩形を描画しないようにします。
		}

		virtual BOOL on_draw_text_ex_w(message_state_t* current_state, HDC dc, LPWSTR text, int c, LPRECT rc, UINT flags, LPDRAWTEXTPARAMS dtp) override
		{
			MY_TRACE_FUNC("dc = {/hex}, flags = {/hex}, rc = ({/}), text = {/}, bk_color = {/hex}, text_color = {/hex}",
				dc, flags, safe_string(rc), safe_string(text, c), ::GetBkColor(dc), ::GetTextColor(dc));

			// 文字列を描画する場合は
			if (!(flags & (DT_CALCRECT | DT_MODIFYSTRING)) && !dtp)
			{
				auto part_id = LVP_LISTITEM;
				auto state_id = get_state_id(dc);

				// ステートIDが有効の場合は
				if (state_id != -1)
				{
					// 文字列を独自に描画します。
					// 文字列の背景は塗りつぶさないようにします。
					return paint::stylus.d2d_draw_text(dc, rc, text, c, flags, palette, part_id, state_id, FALSE);
				}
			}

			return hive.orig.DrawTextExW(dc, text, c, rc, flags, dtp);
		}

		virtual COLORREF on_get_sys_color(int color_id) override
		{
			MY_TRACE_FUNC("color_id = {/}", color_id);

			switch (color_id)
			{
			// グリッドラインの色を返します。
			case COLOR_BTNFACE: return palette.get(LVP_LISTITEM, 0)->background.get_win32_color();
			}

			return __super::on_get_sys_color(color_id);
		}
	};
}
