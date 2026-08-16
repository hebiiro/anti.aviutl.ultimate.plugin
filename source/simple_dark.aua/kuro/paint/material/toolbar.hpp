#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはツールバーのマテリアルです。
	//
	inline struct toolbar_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto background = create_pigment(L"ToolBar", L"Background",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			struct {
				pigment_t normal = create_pigment(L"ToolBarButton", L"Normal",
					style_t::color_e::ButtonBody,
					style_t::color_e::Border,
					style_t::color_e::Text);

				pigment_t disabled = create_pigment(L"ToolBarButton", L"Disable",
					style_t::color_e::ButtonBodyDisable,
					style_t::color_e::Border,
					style_t::color_e::TextDisable);

				pigment_t hot = create_pigment(L"ToolBarButton", L"Hot",
					style_t::color_e::ButtonBodyHover,
					style_t::color_e::Border,
					style_t::color_e::Text);

				pigment_t pressed = create_pigment(L"ToolBarButton", L"Press",
					style_t::color_e::ButtonBodyPress,
					style_t::color_e::BorderSelect,
					style_t::color_e::Text);

				pigment_t checked = create_pigment(L"ToolBarButton", L"Check",
					style_t::color_e::ButtonBodySelect,
					style_t::color_e::BorderFocus,
					style_t::color_e::Text);
			} button;

			struct {
				pigment_t normal = create_pigment(L"ToolBarFlatButton", L"Normal",
					style_t::color_e::Background,
					{},
					style_t::color_e::Text);

				pigment_t disabled = create_pigment(L"ToolBarFlatButton", L"Disable",
					style_t::color_e::Background,
					{},
					style_t::color_e::TextDisable);

				pigment_t hot = create_pigment(L"ToolBarFlatButton", L"Hot",
					style_t::color_e::ButtonBodySelect,
					style_t::color_e::Border,
					style_t::color_e::Text);

				pigment_t pressed = create_pigment(L"ToolBarFlatButton", L"Press",
					style_t::color_e::ButtonBodyPress,
					style_t::color_e::BorderSelect,
					style_t::color_e::Text);

				pigment_t checked = create_pigment(L"ToolBarFlatButton", L"Check",
					style_t::color_e::ButtonBodySelect,
					style_t::color_e::BorderFocus,
					style_t::color_e::Text);
			} flat_button;

			const auto set = [&](auto part_id, const auto& button)
			{
				palette.set(part_id, 0, background);
				palette.set(part_id, TS_NORMAL, button.normal);
				palette.set(part_id, TS_DISABLED, button.disabled);
				palette.set(part_id, TS_HOT, button.hot);
				palette.set(part_id, TS_PRESSED, button.pressed);
				palette.set(part_id, TS_CHECKED, button.checked);
				palette.set(part_id, TS_HOTCHECKED, button.checked);
				palette.set(part_id, TS_NEARHOT, button.normal);
				palette.set(part_id, TS_OTHERSIDEHOT, button.normal);
			};

			set(0, button);
			set(TP_BUTTON, flat_button);
			set(TP_DROPDOWNBUTTON, flat_button);
			set(TP_SPLITBUTTON, flat_button);
			set(TP_SPLITBUTTONDROPDOWN, flat_button);
			set(TP_SEPARATOR, flat_button);
			set(TP_SEPARATORVERT, flat_button);
			set(TP_DROPDOWNBUTTONGLYPH, flat_button);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} toolbar_material;
}
