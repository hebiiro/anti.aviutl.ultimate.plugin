#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはツリービューのテーマをフックしてカスタム描画を実行します。
	//
	inline struct treeview_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::treeview_material.palette;

		//
		// 項目の背景を描画します。
		//
		BOOL draw_item(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return paint::stylus.draw_round_rect(dc, arg_rc, palette, part_id, state_id);
		}

		//
		// グリフ(伸縮ボタン)を描画します。
		//
		BOOL draw_glyph(HDC dc, LPCRECT arg_rc, int part_id, int state_id, WCHAR char_code)
		{
			auto icon_rc = *arg_rc;

			// アイコンのサイズを微調整します。
			auto width = ::MulDiv(my::get_width(icon_rc), 2, 3);
			icon_rc.top = (icon_rc.top + icon_rc.bottom - width) / 2;
			icon_rc.bottom = icon_rc.top + width;

			// アイコンを描画します。
			return paint::stylus.draw_icon(dc, &icon_rc, palette, part_id, state_id, paint::symbol::k_font_name, char_code);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			{
				switch (part_id)
				{
				case TVP_TREEITEM:
					{
						// 項目の背景を描画します。

						if (draw_item(dc, rc, part_id, state_id))
							return S_OK;

						break;
					}
				case TVP_GLYPH:
				case TVP_HOTGLYPH:
					{
						// グリフ(伸縮ボタン)を描画します。

						auto char_code = wchar_t {};

						switch (state_id)
						{
						case GLPS_CLOSED: char_code = paint::symbol::k_chevron_down; break;
						case GLPS_OPENED: char_code = paint::symbol::k_chevron_right; break;
						}

						if (char_code && draw_glyph(dc, rc, part_id, state_id, char_code))
							return S_OK;

						break;
					}
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

			return hive.orig.DrawThemeText(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
		}

		HRESULT on_draw_theme_text_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) override
		{
//			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

			if (!(text_flags & DT_CALCRECT))
			{
				if (paint::stylus.draw_text(dc, rc, text, c, text_flags, palette, part_id, state_id))
					return S_OK;
			}

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
	} treeview_renderer;
}
