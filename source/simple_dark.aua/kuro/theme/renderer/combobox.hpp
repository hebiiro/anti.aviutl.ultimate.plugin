#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはコンボボックスのテーマをフックしてカスタム描画を実行します。
	//
	inline struct combobox_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::combobox_material.palette;

		int border_state_id = 0;
		int readonly_state_id = 0;
		int dropdown_state_id = 0;

		//
		// コンボボックス本体を描画します。
		//
		BOOL draw_combobox(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return paint::stylus.draw_round_rect(dc, arg_rc, palette, part_id, state_id);
		}

		//
		// ドロップダウンボタンを描画します。
		//
		BOOL draw_drop_down_button(HDC dc, LPCRECT arg_rc, int part_id, int state_id, int offset_x)
		{
			auto icon_rc = *arg_rc;
			auto width = ::MulDiv(my::get_width(icon_rc), 2, 3);
			icon_rc.top = (icon_rc.top + icon_rc.bottom - width) / 2;
			icon_rc.bottom = icon_rc.top + width;
			::OffsetRect(&icon_rc, offset_x, 0);

			return paint::stylus.draw_icon(dc, &icon_rc, palette, part_id, state_id, paint::symbol::k_font_name, paint::symbol::k_chevron_down);
//			return paint::stylus.draw_icon(dc, &icon_rc, palette, part_id, state_id, paint::symbol::k_font_name, paint::symbol::k_chevron_down_med);
//			return paint::stylus.draw_icon(dc, &icon_rc, palette, part_id, state_id, paint::symbol::k_font_name, paint::symbol::k_chevron_down_small);
		}

		//
		// 右側ドロップダウンボタンを描画します。
		//
		BOOL draw_drop_down_button_right(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_drop_down_button(dc, arg_rc, part_id, state_id, -2);
		}

		//
		// 左側ドロップダウンボタンを描画します。
		//
		BOOL draw_drop_down_button_left(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_drop_down_button(dc, arg_rc, part_id, state_id, 2);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			{
				// ここで背景を描画しつつ、state_idを取得しておきます。

				switch (part_id)
				{
				case CP_BORDER:
					{
						border_state_id = state_id;

						if (draw_combobox(dc, rc, part_id, state_id))
							return S_OK;

						break;
					}
				case CP_READONLY:
					{
						readonly_state_id = state_id;

						if (draw_combobox(dc, rc, part_id, state_id))
							return S_OK;

						break;
					}
				case CP_DROPDOWNBUTTONRIGHT:
					{
						dropdown_state_id = state_id;

						if (draw_drop_down_button_right(dc, rc, part_id, state_id))
							return S_OK;

						break;
					}
				case CP_DROPDOWNBUTTONLEFT:
					{
						dropdown_state_id = state_id;

						if (draw_drop_down_button_left(dc, rc, part_id, state_id))
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

			{
				// state_idは通常状態しか送られてこないので、
				// 代わりに::DrawThemeBackground()で指定されたstate_idを使用します。

				switch (part_id)
				{
				case CP_BORDER: state_id = border_state_id; break;
				case CP_READONLY: state_id = readonly_state_id; break;
				case CP_DROPDOWNBUTTONRIGHT: state_id = dropdown_state_id; break;
				case CP_DROPDOWNBUTTONLEFT: state_id = dropdown_state_id; break;
				}

				// 選択項目名を描画しますが、そのとき背景は塗りつぶさないようにします。
				if (paint::stylus.draw_text(dc, rc, text, c, text_flags, palette, part_id, state_id, FALSE))
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
	} combobox_renderer;
}
