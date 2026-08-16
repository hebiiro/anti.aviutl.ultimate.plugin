#pragma once

namespace apn::dark::kuro
{
	namespace
	{
		enum EDITTEXTSTATES {
			ETS_FOOTER = 100,
		};
	}
}

namespace apn::dark::kuro::paint
{
	//
	// このクラスはダイアログのマテリアルです。
	//
	inline struct dialog_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto normal = create_pigment(L"Dialog", L"Normal",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto disabled = create_pigment(L"Dialog", L"Disable",
				style_t::color_e::Background,
				{},
				style_t::color_e::TextDisable);

			auto selected = create_pigment(L"Dialog", L"Select",
				style_t::color_e::ButtonBodySelect,
				{},
				style_t::color_e::Text);

			auto readonly = create_pigment(L"Dialog", L"ReadOnly",
				style_t::color_e::ButtonBody,
				{},
				style_t::color_e::Text);

			auto footer = create_pigment(L"Dialog", L"Footer",
				style_t::color_e::Footer,
				{},
				{});

			palette.set(WP_DIALOG, ETS_NORMAL, normal);
			palette.set(WP_DIALOG, ETS_DISABLED, disabled);
			palette.set(WP_DIALOG, ETS_SELECTED, selected);
			palette.set(WP_DIALOG, ETS_HOT, selected);
			palette.set(WP_DIALOG, ETS_FOCUSED, selected);
			palette.set(WP_DIALOG, ETS_READONLY, readonly);
			palette.set(WP_DIALOG, ETS_ASSIST, disabled);
			palette.set(WP_DIALOG, ETS_CUEBANNER, disabled);
			palette.set(WP_DIALOG, ETS_FOOTER, footer);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} dialog_material;
}
