#pragma once

namespace apn::dark::theme
{
	inline struct ButtonRenderer : Renderer
	{
		int get_my_state_id(int part_id, int state_id)
		{
			switch (part_id)
			{
			case BP_RADIOBUTTON:
				{
					switch (state_id)
					{
						case RBS_UNCHECKEDNORMAL: return PBS_NORMAL;
						case RBS_UNCHECKEDDISABLED: return PBS_DISABLED;
						case RBS_UNCHECKEDHOT: return PBS_HOT;
						case RBS_UNCHECKEDPRESSED: return PBS_PRESSED;
						case RBS_CHECKEDNORMAL: return PBS_NORMAL;
						case RBS_CHECKEDDISABLED: return PBS_DISABLED;
						case RBS_CHECKEDHOT: return PBS_HOT;
						case RBS_CHECKEDPRESSED: return PBS_PRESSED;
					}

					break;
				}
			case BP_CHECKBOX:
				{
					switch (state_id)
					{
						case CBS_UNCHECKEDNORMAL: return PBS_NORMAL;
						case CBS_UNCHECKEDDISABLED: return PBS_DISABLED;
						case CBS_UNCHECKEDHOT: return PBS_HOT;
						case CBS_UNCHECKEDPRESSED: return PBS_PRESSED;
						case CBS_CHECKEDNORMAL: return PBS_NORMAL;
						case CBS_CHECKEDDISABLED: return PBS_DISABLED;
						case CBS_CHECKEDHOT: return PBS_HOT;
						case CBS_CHECKEDPRESSED: return PBS_PRESSED;
					}

					break;
				}
			}

			return 0;
		}

		HRESULT on_draw_theme_parent_background(HWND hwnd, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, ({/})", hwnd, dc, safe_string(rc));

			return hive.orig.DrawThemeParentBackground(hwnd, dc, rc);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			if (hive.specialize_checkbox && (part_id == BP_RADIOBUTTON || part_id == BP_CHECKBOX))
			{
				// 全体を描画します。

				auto hwnd = gdi::manager.current_state.hwnd;
				auto rc_client = my::get_client_rect(hwnd);

				auto my_part_id = BP_PUSHBUTTON;
				auto my_state_id = get_my_state_id(part_id, state_id);

				if (my_state_id != 0)
					python.call_draw_figure(gdi::manager.current_state.hwnd, theme, dc, my_part_id, my_state_id, &rc_client);

				// マークを描画します。

				auto rc_mark = *rc;

				if (my::get_style(hwnd) & BS_RIGHTBUTTON)
					::OffsetRect(&rc_mark, -2, 0);
				else
					::OffsetRect(&rc_mark, 2, 0);

				if (python.call_draw_figure(gdi::manager.current_state.hwnd, theme, dc, part_id, state_id, &rc_mark))
					return S_OK;
			}
			else
			{
				if (python.call_draw_figure(gdi::manager.current_state.hwnd, theme, dc, part_id, state_id, rc))
					return S_OK;
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
				if (hive.specialize_checkbox && (part_id == BP_RADIOBUTTON || part_id == BP_CHECKBOX))
				{
					// 文字列の描画位置を調整します。

					auto hwnd = gdi::manager.current_state.hwnd;
					auto rc_text = my::get_client_rect(hwnd);
					rc_text.left = rc->left;
					rc_text.right = rc->right;

					if (python.call_draw_text(gdi::manager.current_state.hwnd, theme, dc, part_id, state_id, text, c, text_flags, &rc_text))
						return S_OK;
				}
				else
				{
					if (python.call_draw_text(gdi::manager.current_state.hwnd, theme, dc, part_id, state_id, text, c, text_flags, rc))
						return S_OK;
				}
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
	} button_renderer;
}
