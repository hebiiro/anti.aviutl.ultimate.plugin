#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはスピンボタンのマテリアルです。
	//
	inline struct spin_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto button_normal = create_pigment(L"Spin", L"Normal",
				style_t::color_e::ButtonBody,
				style_t::color_e::Border,
				style_t::color_e::Text);

			auto button_disabled = create_pigment(L"Spin", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				style_t::color_e::Border,
				style_t::color_e::TextDisable);

			auto button_hot = create_pigment(L"Spin", L"Hot",
				style_t::color_e::ButtonBodySelect,
				style_t::color_e::BorderFocus,
				style_t::color_e::Text);

			auto button_pressed = create_pigment(L"Spin", L"Press",
				style_t::color_e::ButtonBodyPress,
				style_t::color_e::BorderSelect,
				style_t::color_e::Text);

			palette.set(SPNP_UP, UPS_NORMAL, button_normal);
			palette.set(SPNP_UP, UPS_DISABLED, button_disabled);
			palette.set(SPNP_UP, UPS_HOT, button_hot);
			palette.set(SPNP_UP, UPS_PRESSED, button_pressed);
			palette.set(SPNP_DOWN, DNS_NORMAL, button_normal);
			palette.set(SPNP_DOWN, DNS_DISABLED, button_disabled);
			palette.set(SPNP_DOWN, DNS_HOT, button_hot);
			palette.set(SPNP_DOWN, DNS_PRESSED, button_pressed);
			palette.set(SPNP_UPHORZ, UPHZS_NORMAL, button_normal);
			palette.set(SPNP_UPHORZ, UPHZS_DISABLED, button_disabled);
			palette.set(SPNP_UPHORZ, UPHZS_HOT, button_hot);
			palette.set(SPNP_UPHORZ, UPHZS_PRESSED, button_pressed);
			palette.set(SPNP_DOWNHORZ, DNHZS_NORMAL, button_normal);
			palette.set(SPNP_DOWNHORZ, DNHZS_DISABLED, button_disabled);
			palette.set(SPNP_DOWNHORZ, DNHZS_HOT, button_hot);
			palette.set(SPNP_DOWNHORZ, DNHZS_PRESSED, button_pressed);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} spin_material;
}
