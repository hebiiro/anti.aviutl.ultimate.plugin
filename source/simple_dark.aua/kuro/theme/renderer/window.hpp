#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはウィンドウのテーマをフックしてカスタム描画を実行します。
	//
	inline struct window_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::window_material.palette;

		virtual HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			switch (part_id)
			{
			case WP_BORDER: paint::draw_single_border(dc, rc); return S_OK;
			case WP_WINDOW_EDGE: paint::draw_raised_edge(dc, rc); return S_OK;
			case WP_CLIENT_EDGE: paint::draw_sunken_edge(dc, rc); return S_OK;
			case WP_STATIC_EDGE: paint::draw_single_sunken_edge(dc, rc); return S_OK;
			case WP_LIGHT_RAISED_EDGE: paint::draw_single_raised_edge(dc, rc); return S_OK;
			case WP_LIGHT_SUNKEN_EDGE: paint::draw_single_sunken_edge(dc, rc); return S_OK;
			case WP_RAISED_EDGE: paint::draw_raised_edge(dc, rc); return S_OK;
			case WP_SUNKEN_EDGE: paint::draw_sunken_edge(dc, rc); return S_OK;
			case WP_BUMP_EDGE: paint::draw_bump_edge(dc, rc); return S_OK;
			case WP_ETCHED_EDGE: paint::draw_etched_edge(dc, rc); return S_OK;
			}

			{
				if (paint::stylus.draw_rect(dc, rc, palette, part_id, state_id))
					return S_OK;
			}

			return hive.orig.DrawThemeBackground(theme, dc, part_id, state_id, rc, rc_clip);
		}

		virtual HRESULT on_draw_theme_text(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, ({/})", theme, dc, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

			if (!(text_flags & DT_CALCRECT))
			{
				if (paint::stylus.d2d_draw_text(dc, rc, text, c, text_flags, palette, part_id, state_id, FALSE))
					return S_OK;
			}

			return hive.orig.DrawThemeText(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
		}
	} window_renderer;
}
