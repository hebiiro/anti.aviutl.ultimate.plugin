#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはアドレスバンドのテーマをフックしてカスタム描画を実行します。
	//
	inline struct address_band_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::address_band_material.palette;

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			{
				if (paint::stylus.draw_round_rect(dc, rc, palette, 0, 0))
					return S_OK;
			}

			return hive.orig.DrawThemeBackground(theme, dc, part_id, state_id, rc, rc_clip);
		}
	} address_band_renderer;
}
