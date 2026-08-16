#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはエクスプローラナビゲーションペインのテーマをフックしてカスタム描画を実行します。
	//
	inline struct explorer_nav_pane_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::dialog_material.palette;

		virtual HRESULT on_get_theme_color(HTHEME theme, int part_id, int state_id, int prop_id, COLORREF* result) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}", theme, part_id, state_id, prop_id);

			// 背景の色を変更します。
			if (part_id == 0 && state_id == 0 && prop_id == TMT_FILLCOLOR)
				return *result = palette.get(WP_DIALOG, ETS_NORMAL)->background.get_win32_color(), S_OK;

			return __super::on_get_theme_color(theme, part_id, state_id, prop_id, result);
		}
	} explorer_nav_pane_renderer;
}
