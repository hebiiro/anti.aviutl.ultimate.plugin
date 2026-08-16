#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはウィンドウのマテリアルです。
	//
	inline struct window_material_t : material_t
	{
		pigment_t active, inactive;

		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			active = create_pigment(L"Window", L"Active",
				style_t::color_e::GroupingHover,
				style_t::color_e::GroupingSeparator,
				style_t::color_e::Text);

			inactive = create_pigment(L"Window", L"Inactive",
				style_t::color_e::Grouping,
				style_t::color_e::GroupingSeparator,
				style_t::color_e::TextDisable);

			palette.set(WP_DIALOG, 0, active);
			palette.set(WP_WINDOW_FACE, 0, active);

			auto caption_active = create_pigment(L"Caption", L"Active",
				style_t::color_e::GroupingSelect,
				style_t::color_e::GroupingSeparator,
				style_t::color_e::Text);

			auto caption_inactive = create_pigment(L"Caption", L"Inactive",
				style_t::color_e::Grouping,
				style_t::color_e::GroupingSeparator,
				style_t::color_e::Text);

			auto caption_disabled = create_pigment(L"Caption", L"Disable",
				style_t::color_e::Grouping,
				style_t::color_e::GroupingSeparator,
				style_t::color_e::TextDisable);

			palette.set(WP_CAPTION, CS_ACTIVE, caption_active);
			palette.set(WP_CAPTION, CS_INACTIVE, caption_inactive);
			palette.set(WP_CAPTION, CS_DISABLED, caption_disabled);

			auto separator_active = create_pigment(L"Separator", L"Active",
				style_t::color_e::BorderFocus,
				{},
				style_t::color_e::Text);

			auto separator_inactive = create_pigment(L"Separator", L"Inactive",
				style_t::color_e::Border,
				{},
				style_t::color_e::Text);

			auto separator_disabled = create_pigment(L"Separator", L"Disable",
				style_t::color_e::WindowSeparator,
				{},
				style_t::color_e::Text);

			palette.set(WP_SEPARATOR, CS_ACTIVE, separator_active);
			palette.set(WP_SEPARATOR, CS_INACTIVE, separator_inactive);
			palette.set(WP_SEPARATOR, CS_DISABLED, separator_disabled);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} window_material;
}
