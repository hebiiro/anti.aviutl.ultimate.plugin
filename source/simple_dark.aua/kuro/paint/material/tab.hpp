#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはタブコントロールのマテリアルです。
	//
	inline struct tab_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto background = create_pigment(L"Tab", L"Background",
				style_t::color_e::Background,
				{},
				{});

			auto pane = create_pigment(L"Tab", L"Pane",
				style_t::color_e::Background,
				style_t::color_e::Border,
				{});

			auto item_normal = create_pigment(L"TabItem", L"Normal",
				style_t::color_e::ButtonBody,
				style_t::color_e::Border,
				style_t::color_e::Text);

			auto item_disabled = create_pigment(L"TabItem", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				style_t::color_e::Border,
				style_t::color_e::TextDisable);

			auto item_hot = create_pigment(L"TabItem", L"Hot",
				style_t::color_e::ButtonBody,
				style_t::color_e::BorderSelect,
				style_t::color_e::Text);

			auto item_selected = create_pigment(L"TabItem", L"Select",
				style_t::color_e::ButtonBodySelect,
				style_t::color_e::BorderSelect,
				style_t::color_e::Text);

			auto item_focused = create_pigment(L"TabItem", L"Focus",
				style_t::color_e::ButtonBody,
				style_t::color_e::BorderFocus,
				style_t::color_e::Text);

			palette.set(0, 0, background);
			palette.set(TABP_PANE, 0, pane);
			palette.set(TABP_TOPTABITEM, TIS_NORMAL, item_normal);
			palette.set(TABP_TOPTABITEM, TIS_DISABLED, item_disabled);
			palette.set(TABP_TOPTABITEM, TIS_HOT, item_hot);
			palette.set(TABP_TOPTABITEM, TIS_SELECTED, item_selected);
			palette.set(TABP_TOPTABITEM, TIS_FOCUSED, item_focused);
			palette.set(TABP_TOPTABITEMLEFTEDGE, TIS_NORMAL, item_normal);
			palette.set(TABP_TOPTABITEMLEFTEDGE, TIS_DISABLED, item_disabled);
			palette.set(TABP_TOPTABITEMLEFTEDGE, TIS_HOT, item_hot);
			palette.set(TABP_TOPTABITEMLEFTEDGE, TIS_SELECTED, item_selected);
			palette.set(TABP_TOPTABITEMLEFTEDGE, TIS_FOCUSED, item_focused);
			palette.set(TABP_TOPTABITEMRIGHTEDGE, TIS_NORMAL, item_normal);
			palette.set(TABP_TOPTABITEMRIGHTEDGE, TIS_DISABLED, item_disabled);
			palette.set(TABP_TOPTABITEMRIGHTEDGE, TIS_HOT, item_hot);
			palette.set(TABP_TOPTABITEMRIGHTEDGE, TIS_SELECTED, item_selected);
			palette.set(TABP_TOPTABITEMRIGHTEDGE, TIS_FOCUSED, item_focused);
			palette.set(TABP_TOPTABITEMBOTHEDGE, TIS_NORMAL, item_normal);
			palette.set(TABP_TOPTABITEMBOTHEDGE, TIS_DISABLED, item_disabled);
			palette.set(TABP_TOPTABITEMBOTHEDGE, TIS_HOT, item_hot);
			palette.set(TABP_TOPTABITEMBOTHEDGE, TIS_SELECTED, item_selected);
			palette.set(TABP_TOPTABITEMBOTHEDGE, TIS_FOCUSED, item_focused);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} tab_material;
}
