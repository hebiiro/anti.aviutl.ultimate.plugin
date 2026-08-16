#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはタブコントロールのテーマをフックしてカスタム描画を実行します。
	//
	inline struct tab_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::tab_material.palette;

		//
		// 背景を描画します。
		//
		BOOL draw_background(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return paint::stylus.draw_rect(dc, arg_rc, palette, part_id, state_id);
		}

		//
		// ペインを描画します。
		//
		BOOL draw_pane(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return paint::stylus.draw_rect(dc, arg_rc, palette, part_id, state_id);
		}

		//
		// アイテムを描画します。
		//
		BOOL draw_item(HDC dc, LPCRECT arg_rc, int part_id, int state_id, BOOL selected)
		{
			auto rc = *arg_rc;
			if (!selected) ::InflateRect(&rc, 0, 2);
			rc.bottom -= 4;

			return paint::stylus.draw_rect(dc, &rc, palette, part_id, state_id);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			{
				switch (part_id)
				{
				case 0:
					{
						if (draw_background(dc, rc, part_id, state_id))
							return S_OK;

						break;
					}
				case TABP_PANE:
					{
						if (draw_pane(dc, rc, part_id, state_id))
							return S_OK;

						break;
					}
				case TABP_TOPTABITEM:
				case TABP_TOPTABITEMLEFTEDGE:
				case TABP_TOPTABITEMRIGHTEDGE:
				case TABP_TOPTABITEMBOTHEDGE:
					{
						auto selected = (state_id == TIS_SELECTED);

						if (draw_item(dc, rc, part_id, state_id, selected))
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

			// タブ項目名を描画しますが、そのとき背景は塗りつぶさないようにします。
			if (paint::stylus.draw_text(dc, rc, text, c, text_flags, palette, part_id, state_id, FALSE))
				return S_OK;

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
	} tab_renderer;
}
