#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはエディットボックスのマテリアルです。
	//
	inline struct editbox_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto ets_normal = create_pigment(L"EditBox", L"Normal",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto ets_disabled = create_pigment(L"EditBox", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				{},
				style_t::color_e::TextDisable);

			auto ets_selected = create_pigment(L"EditBox", L"Select",
				style_t::color_e::TextSelect,
				{},
				style_t::color_e::Text);

			auto ets_readonly = create_pigment(L"EditBox", L"ReadOnly",
				style_t::color_e::ButtonBody,
				{},
				style_t::color_e::Text);

			palette.set(EP_EDITTEXT, ETS_NORMAL, ets_normal);
			palette.set(EP_EDITTEXT, ETS_DISABLED, ets_disabled);
			palette.set(EP_EDITTEXT, ETS_SELECTED, ets_selected);
			palette.set(EP_EDITTEXT, ETS_HOT, ets_selected);
			palette.set(EP_EDITTEXT, ETS_FOCUSED, ets_selected);
			palette.set(EP_EDITTEXT, ETS_READONLY, ets_readonly);
			palette.set(EP_EDITTEXT, ETS_ASSIST, ets_disabled);
			palette.set(EP_EDITTEXT, ETS_CUEBANNER, ets_disabled);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} editbox_material;
}
