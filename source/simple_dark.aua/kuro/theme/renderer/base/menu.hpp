#pragma once

namespace apn::dark::kuro::theme::base
{
	//
	// このクラスはメニューレンダラーのベースクラスです。
	//
	struct menu_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::menu_material.palette;

		virtual HRESULT on_get_theme_color(HTHEME theme, int part_id, int state_id, int prop_id, COLORREF* result) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}", theme, part_id, state_id, prop_id);

			// ポップアップメニューの背景の色を変更します。
			if (part_id == MENU_POPUPBACKGROUND && state_id == 0 && prop_id == TMT_FILLCOLOR)
				return *result = palette.get(part_id, state_id)->background.get_win32_color(), S_OK;

			// ポップアップメニューの縁の色を変更します。
			if (part_id == MENU_POPUPBORDERS && state_id == 0 && prop_id == TMT_FILLCOLORHINT)
				return *result = palette.get(part_id, state_id)->background.get_win32_color(), S_OK;

			// ポップアップメニュー項目のテキストの色を変更します。
			if (part_id == MENU_POPUPITEM && prop_id == TMT_TEXTCOLOR)
				return *result = palette.get(part_id, state_id)->text.get_win32_color(), S_OK;

			// ポップアップメニュー項目のテキストの色を変更します。
			if (part_id == MENU_POPUPITEMFOCUSABLE && prop_id == TMT_TEXTCOLOR)
				return *result = palette.get(part_id, state_id)->text.get_win32_color(), S_OK;

			return __super::on_get_theme_color(theme, part_id, state_id, prop_id, result);
		}
	};
}
