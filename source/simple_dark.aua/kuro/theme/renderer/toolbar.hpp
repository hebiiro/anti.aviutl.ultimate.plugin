#pragma once

namespace apn::dark::kuro::theme
{
	inline struct toolbar_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::toolbar_material.palette;

		//
		// ドロップダウンボタンを描画します。
		//
		BOOL draw_drop_down_button(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			auto icon_rc = *arg_rc;

			paint::stylus.draw_rect(dc, &icon_rc, palette, part_id, state_id);

			auto width = ::MulDiv(my::get_width(icon_rc), 2, 3);
			icon_rc.top = (icon_rc.top + icon_rc.bottom - width) / 2;
			icon_rc.bottom = icon_rc.top + width;

			return paint::stylus.draw_icon(dc, &icon_rc, palette, part_id, state_id, paint::symbol::k_font_name, paint::symbol::k_chevron_down);
		}

		//
		// ドロップダウンボタングリフを描画します。
		//
		BOOL draw_drop_down_button_glyph(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			auto icon_rc = *arg_rc;

			auto w = ::MulDiv(my::get_width(icon_rc), 2, 3);
			auto h = ::MulDiv(my::get_height(icon_rc), 2, 3);

			// 縦長矩形の場合は
			if (w < h)
			{
				// 横幅を基準にしてセンタリングされた正方形に変更します。
				icon_rc.top = (icon_rc.top + icon_rc.bottom - w) / 2;
				icon_rc.bottom = icon_rc.top + w;
			}
			// 横長矩形の場合は
			else
			{
				// 縦幅を基準にしてセンタリングされた正方形に変更します。
				icon_rc.left = (icon_rc.left + icon_rc.right - h) / 2;
				icon_rc.right = icon_rc.left + h;
			}

			return paint::stylus.draw_icon(dc, &icon_rc, palette, part_id, state_id, paint::symbol::k_font_name, paint::symbol::k_chevron_down);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			switch (part_id)
			{
			case 0:
			case TP_BUTTON:
			case TP_DROPDOWNBUTTON:
				{
					if (state_id)
					{
						// ボタンを描画します。
						if (paint::stylus.draw_round_rect(dc, rc, palette, part_id, state_id))
							return S_OK;
					}
					else
					{
						// 背景を描画します。
						if (paint::stylus.draw_rect(dc, rc, palette, part_id, state_id))
							return S_OK;
					}

					break;
				}
			case TP_SPLITBUTTON:
			case TP_SEPARATOR:
			case TP_SEPARATORVERT:
				{
					// ボタンを描画します。
					if (paint::stylus.draw_rect(dc, rc, palette, part_id, state_id))
						return S_OK;

					break;
				}
			case TP_SPLITBUTTONDROPDOWN:
				{
					// ドロップダウンを描画します。

					if (draw_drop_down_button(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case TP_DROPDOWNBUTTONGLYPH:
				{
					// ドロップダウングリフを描画します。

					if (draw_drop_down_button_glyph(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			}

			return hive.orig.DrawThemeBackground(theme, dc, part_id, state_id, rc, rc_clip);
		}

		HRESULT on_draw_theme_background_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(rc), options);

			return hive.orig.DrawThemeBackgroundEx(theme, dc, part_id, state_id, rc, options);
		}

		HRESULT on_draw_theme_text(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, ({/})", theme, dc, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

			if (!(text_flags & DT_CALCRECT))
			{
				if (paint::stylus.draw_text(dc, rc, text, c, text_flags, palette, part_id, state_id))
					return S_OK;
			}

			return hive.orig.DrawThemeText(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
		}

		HRESULT on_draw_theme_text_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

			return hive.orig.DrawThemeTextEx(theme, dc, part_id, state_id, text, c, text_flags, rc, options);
		}

		HRESULT on_draw_theme_icon(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int image_index) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/}", theme, dc, part_id, state_id, safe_string(rc), image_list, image_index);

			return hive.orig.DrawThemeIcon(theme, dc, part_id, state_id, rc, image_list, image_index);
		}

		HRESULT on_draw_theme_edge(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/hex}", theme, dc, part_id, state_id, safe_string(dest_rect), edge, flags);

			return hive.orig.DrawThemeEdge(theme, dc, part_id, state_id, dest_rect, edge, flags, content_rect);
		}
	} toolbar_renderer;
}
