#pragma once

namespace apn::dark::kuro::theme
{
	inline struct command_module_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::command_module_material.palette;

		//
		// ドロップダウンボタンを描画します。
		//
		BOOL draw_drop_down_button(HDC dc, LPCRECT arg_rc, int part_id, int state_id, int offset)
		{
			auto rc = *arg_rc;

			// アイコンの位置を微調整します。
			::OffsetRect(&rc, 1, 1);

			return paint::stylus.draw_icon(dc, &rc, palette, part_id, state_id, paint::symbol::k_font_name, paint::symbol::k_chevron_down_small);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			return hive.orig.DrawThemeBackground(theme, dc, part_id, state_id, rc, rc_clip);
		}

		HRESULT on_draw_theme_background_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(rc), options);

			switch (part_id)
			{
			case CMP_BUTTON:
				{
					// コマンドモジュールDCをセットします。
					// この後テキストを描画するときに参照されます。
					paint::command_module_material.dc = dc;

					if (paint::stylus.draw_round_rect(dc, rc, palette, part_id, state_id))
						return S_OK;

					break;
				}
			case CMP_BODY:
			case CMP_SPLITBUTTON:
			case CMP_SPLITBUTTONDROPDOWN:
				{
					if (paint::stylus.draw_rect(dc, rc, palette, part_id, state_id))
						return S_OK;

					break;
				}
			case CMP_DROPDOWNBUTTONGLYPH:
				{
					// ドロップダウンボタンを描画します。

					if (draw_drop_down_button(dc, rc, part_id, state_id, 0))
						return S_OK;

					break;
				}
			}

			return hive.orig.DrawThemeBackgroundEx(theme, dc, part_id, state_id, rc, options);
		}

		HRESULT on_draw_theme_text(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, ({/})", theme, dc, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

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
	} command_module_renderer;
}
