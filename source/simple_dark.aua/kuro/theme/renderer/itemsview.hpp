#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはItemsViewのテーマをフックしてカスタム描画を実行します。
	//
	inline struct itemsview_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::listview_material.palette;

		virtual HRESULT on_get_theme_color(HTHEME theme, int part_id, int state_id, int prop_id, COLORREF* result) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}", theme, part_id, state_id, prop_id);

			if (auto pigment = palette.get(LVP_LISTITEM, LISS_NORMAL))
			{
				switch (prop_id)
				{
				case TMT_FILLCOLOR: return *result = pigment->background.get_win32_color(), S_OK;
				case TMT_TEXTCOLOR: return *result = pigment->text.get_win32_color(), S_OK;
				}
			}

			return __super::on_get_theme_color(theme, part_id, state_id, prop_id, result);
		}
	} itemsview_renderer;
}
