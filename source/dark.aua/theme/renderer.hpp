﻿#pragma once

namespace apn::dark::theme
{
	struct Renderer
	{
		virtual HRESULT on_draw_theme_parent_background(HWND hwnd, HDC dc, LPCRECT rc)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, ({})", hwnd, dc, safe_string(rc));

			return hive.orig.DrawThemeParentBackground(hwnd, dc, rc);
		}

		virtual HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, ({}), ({})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			return hive.orig.DrawThemeBackground(theme, dc, part_id, state_id, rc, rc_clip);
		}

		virtual HRESULT on_draw_theme_background_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, ({}), {:#010x}", theme, dc, part_id, state_id, safe_string(rc), options);

			return hive.orig.DrawThemeBackgroundEx(theme, dc, part_id, state_id, rc, options);
		}

		virtual HRESULT on_draw_theme_text(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, {}, {:#010x}, {:#010x}, ({})", theme, dc, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

			return hive.orig.DrawThemeText(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
		}

		virtual HRESULT on_draw_theme_text_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, {}, {:#010x}, ({}), {:#010x}", theme, dc, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

			return hive.orig.DrawThemeTextEx(theme, dc, part_id, state_id, text, c, text_flags, rc, options);
		}

		virtual HRESULT on_draw_theme_icon(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int image_index)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, ({}), {:#010x}, {}", theme, dc, part_id, state_id, safe_string(rc), image_list, image_index);

			return hive.orig.DrawThemeIcon(theme, dc, part_id, state_id, rc, image_list, image_index);
		}

		virtual HRESULT on_draw_theme_edge(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, ({}), {:#010x}, {:#010x}", theme, dc, part_id, state_id, safe_string(dest_rect), edge, flags);

			return hive.orig.DrawThemeEdge(theme, dc, part_id, state_id, dest_rect, edge, flags, content_rect);
		}
	};
}
