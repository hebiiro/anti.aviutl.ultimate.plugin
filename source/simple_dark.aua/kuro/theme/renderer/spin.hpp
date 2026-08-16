#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはスピンコントロールのテーマをフックしてカスタム描画を実行します。
	//
	inline struct spin_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::spin_material.palette;

		//
		// スピンボタンを描画します。
		//
		BOOL draw_spin(HDC dc, LPCRECT arg_rc, int part_id, int state_id, WCHAR char_code, BOOL pressed)
		{
			auto rc = *arg_rc;

			// 背景を描画します。
			paint::stylus.draw_rect(dc, &rc, palette, part_id, state_id);

			// アイコンのサイズを調整します。
			::InflateRect(&rc, -my::get_width(rc) / 4, -my::get_height(rc) / 4);

			// プッシュ状態の場合は1pxずらします。
//			if (pressed) ::OffsetRect(&rc, 1, 1);

			// アイコンを描画します。
			return paint::stylus.draw_icon(dc, &rc, palette, part_id, state_id, paint::symbol::k_font_name, char_code);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			{
				// ここでスピンボタンを描画します。

				switch (part_id)
				{
				case SPNP_UP:
					{
						auto char_code = paint::symbol::k_chevron_up_small;
						auto pressed = (state_id == UPS_PRESSED);

						if (draw_spin(dc, rc, part_id, state_id, char_code, pressed))
							return S_OK;

						break;
					}
				case SPNP_DOWN:
					{
						auto char_code = paint::symbol::k_chevron_down_small;
						auto pressed = (state_id == DNS_PRESSED);

						if (draw_spin(dc, rc, part_id, state_id, char_code, pressed))
							return S_OK;

						break;
					}
				case SPNP_UPHORZ:
					{
						auto char_code = paint::symbol::k_chevron_right_small;
						auto pressed = (state_id == UPHZS_PRESSED);

						if (draw_spin(dc, rc, part_id, state_id, char_code, pressed))
							return S_OK;

						break;
					}
				case SPNP_DOWNHORZ:
					{
						auto char_code = paint::symbol::k_chevron_left_small;
						auto pressed = (state_id == DNHZS_PRESSED);

						if (draw_spin(dc, rc, part_id, state_id, char_code, pressed))
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
	} spin_renderer;
}
