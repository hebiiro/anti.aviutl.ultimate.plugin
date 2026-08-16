#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはボタンのマテリアルです。
	//
	inline struct button_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto push_button_normal = create_pigment(L"PushButton", L"Normal",
				style_t::color_e::ButtonBody,
				style_t::color_e::Border,
				style_t::color_e::Text);

			auto push_button_hot = create_pigment(L"PushButton", L"Hot",
				style_t::color_e::ButtonBodyHover,
				style_t::color_e::Border,
				style_t::color_e::Text);

			auto push_button_pressed = create_pigment(L"PushButton", L"Press",
				style_t::color_e::ButtonBodyPress,
				style_t::color_e::BorderSelect,
				style_t::color_e::Text);

			auto push_button_disabled = create_pigment(L"PushButton", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				style_t::color_e::Border,
				style_t::color_e::TextDisable);

			auto radio_button_normal = create_pigment(L"RadioButton", L"Normal",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto radio_button_hot = create_pigment(L"RadioButton", L"Hot",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto radio_button_pressed = create_pigment(L"RadioButton", L"Press",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto radio_button_disabled = create_pigment(L"RadioButton", L"Disable",
				style_t::color_e::Background,
				{},
				style_t::color_e::TextDisable);

			auto checkbox_normal = create_pigment(L"CheckBox", L"Normal", radio_button_normal);
			auto checkbox_hot = create_pigment(L"CheckBox", L"Hot", radio_button_hot);
			auto checkbox_pressed = create_pigment(L"CheckBox", L"Press", radio_button_pressed);
			auto checkbox_disabled = create_pigment(L"CheckBox", L"Disable", radio_button_disabled);

			auto groupbox_normal = create_pigment(L"GroupBox", L"Normal", radio_button_normal);
			auto groupbox_disabled = create_pigment(L"GroupBox", L"Disable", radio_button_disabled);

			palette.set(BP_PUSHBUTTON, PBS_NORMAL, push_button_normal);
			palette.set(BP_PUSHBUTTON, PBS_DISABLED, push_button_disabled);
			palette.set(BP_PUSHBUTTON, PBS_DEFAULTED, push_button_normal);
			palette.set(BP_PUSHBUTTON, PBS_DEFAULTED_ANIMATING, push_button_normal);
			palette.set(BP_PUSHBUTTON, PBS_HOT, push_button_hot);
			palette.set(BP_PUSHBUTTON, PBS_PRESSED, push_button_pressed);

			palette.set(BP_PUSHBUTTONDROPDOWN, PBDDS_NORMAL, push_button_normal);
			palette.set(BP_PUSHBUTTONDROPDOWN, PBDDS_DISABLED, push_button_disabled);

			palette.set(BP_RADIOBUTTON, RBS_UNCHECKEDNORMAL, radio_button_normal);
			palette.set(BP_RADIOBUTTON, RBS_UNCHECKEDDISABLED, radio_button_disabled);
			palette.set(BP_RADIOBUTTON, RBS_UNCHECKEDHOT, radio_button_hot);
			palette.set(BP_RADIOBUTTON, RBS_UNCHECKEDPRESSED, radio_button_pressed);

			palette.set(BP_RADIOBUTTON, RBS_CHECKEDNORMAL, radio_button_normal);
			palette.set(BP_RADIOBUTTON, RBS_CHECKEDDISABLED, radio_button_disabled);
			palette.set(BP_RADIOBUTTON, RBS_CHECKEDHOT, radio_button_hot);
			palette.set(BP_RADIOBUTTON, RBS_CHECKEDPRESSED, radio_button_pressed);

			palette.set(BP_CHECKBOX, CBS_UNCHECKEDNORMAL, checkbox_normal);
			palette.set(BP_CHECKBOX, CBS_UNCHECKEDDISABLED, checkbox_disabled);
			palette.set(BP_CHECKBOX, CBS_UNCHECKEDHOT, checkbox_hot);
			palette.set(BP_CHECKBOX, CBS_UNCHECKEDPRESSED, checkbox_pressed);

			palette.set(BP_CHECKBOX, CBS_CHECKEDNORMAL, checkbox_normal);
			palette.set(BP_CHECKBOX, CBS_CHECKEDDISABLED, checkbox_disabled);
			palette.set(BP_CHECKBOX, CBS_CHECKEDHOT, checkbox_hot);
			palette.set(BP_CHECKBOX, CBS_CHECKEDPRESSED, checkbox_pressed);

			palette.set(BP_GROUPBOX, GBS_NORMAL, groupbox_normal);
			palette.set(BP_GROUPBOX, GBS_DISABLED, groupbox_disabled);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} button_material;
}
