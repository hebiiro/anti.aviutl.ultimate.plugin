#pragma once

namespace apn::dark::theme
{
	inline struct MenuRenderer : Renderer
	{
		struct Clipper
		{
			HDC dc;

			Clipper(HDC dc, LPCRECT rc_clip)
				: dc(dc)
			{
				if (rc_clip)
				{
					auto rc = *rc_clip;
					::LPtoDP(dc, (LPPOINT)&rc, 2);
					painter::clip_rect(dc, &rc);
				}
			}

			~Clipper()
			{
//				::SelectClipRgn(dc, 0);
			}
		};

		HWND get_menu_window(HDC dc)
		{
#if 1
			auto current = HWND {};
			while (auto hwnd = ::FindWindowEx(nullptr, current, L"#32768", nullptr))
			{
				MY_TRACE_HWND(hwnd);

				current = hwnd;
			}

			return current;
#else
			auto hwnd = ::WindowFromDC(dc);
			auto class_name = my::get_class_name(hwnd);
			return (class_name ==  L"#32768") hwnd : nullptr;
#endif
		}

		HRESULT on_draw_theme_parent_background(HWND hwnd, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, ({})", hwnd, dc, safe_string(rc));

			return hive.orig.DrawThemeParentBackground(hwnd, dc, rc);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, ({}), ({})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			if (part_id == MENU_POPUPBORDERS && state_id == 0)
			{
				if (auto hwnd = get_menu_window(dc))
				{
					// この処理でポップアップメニューのボーダーの形状を変えられます。
					if (hive.round_mode == hive.c_round_mode.c_omit)
					{
						auto popup_menu_corner_mode = DWMWCP_DONOTROUND;
						::DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
							&popup_menu_corner_mode, sizeof(popup_menu_corner_mode));
					}
					else
					{
						if (skin::dwm.popup_menu.corner_mode != -1)
						{
							::DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
								&skin::dwm.popup_menu.corner_mode, sizeof(skin::dwm.popup_menu.corner_mode));
						}
					}

					// この処理でポップアップメニューのボーダーの色を変えられます。
					auto border_color = skin::dwm.popup_menu.border_color;
					if (border_color == CLR_NONE) border_color = DWMWA_COLOR_NONE;
					::DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &border_color, sizeof(border_color));
				}
			}

			{
				Clipper clipper(dc, rc_clip);

				if (python.call_draw_figure(theme, dc, part_id, state_id, rc))
					return S_OK;
			}

			return hive.orig.DrawThemeBackground(theme, dc, part_id, state_id, rc, rc_clip);
		}

		HRESULT on_draw_theme_background_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, ({}), {:#010x}", theme, dc, part_id, state_id, safe_string(rc), options);

			return hive.orig.DrawThemeBackgroundEx(theme, dc, part_id, state_id, rc, options);
		}

		HRESULT on_draw_theme_text(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, {}, {:#010x}, {:#010x}, ({})", theme, dc, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

			{
				if (python.call_draw_text(theme, dc, part_id, state_id, text, c, text_flags, rc))
					return S_OK;
			}

			return hive.orig.DrawThemeText(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
		}

		HRESULT on_draw_theme_text_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, {}, {:#010x}, ({}), {:#010x}", theme, dc, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

			return hive.orig.DrawThemeTextEx(theme, dc, part_id, state_id, text, c, text_flags, rc, options);
		}

		HRESULT on_draw_theme_icon(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int image_index) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, ({}), {:#010x}, {}", theme, dc, part_id, state_id, safe_string(rc), image_list, image_index);

			return hive.orig.DrawThemeIcon(theme, dc, part_id, state_id, rc, image_list, image_index);
		}

		HRESULT on_draw_theme_edge(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, ({}), {:#010x}, {:#010x}", theme, dc, part_id, state_id, safe_string(dest_rect), edge, flags);

			return hive.orig.DrawThemeEdge(theme, dc, part_id, state_id, dest_rect, edge, flags, content_rect);
		}
	} menu_renderer;
}
