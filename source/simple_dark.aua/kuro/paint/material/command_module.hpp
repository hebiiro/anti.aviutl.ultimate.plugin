#pragma once

namespace apn::dark::kuro
{
	namespace
	{
		enum COMMANDMODULEPARTS {
			CMP_BODY = 1,
			CMP_BUTTON = 3,
			CMP_SPLITBUTTON = 4,
			CMP_SPLITBUTTONDROPDOWN = 5,
			CMP_DROPDOWNBUTTONGLYPH = 6,
		};

		enum COMMANDMODULESTATES {
			CMS_BACKGROUND = 0,
			CMS_NORMAL = 1,
			CMS_HOT = 2,
			CMS_PRESSED = 3,
			CMS_FOCUSED = 4,
			CMS_CHECKED = 5,
		};
	}
}

namespace apn::dark::kuro::paint
{
	//
	// このクラスはコマンドモジュールのマテリアルです。
	//
	inline struct command_module_material_t : material_t
	{
		//
		// コマンドモジュールを描画している現在のDCです。
		//
		HDC dc = {};

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

			palette.set(CMP_BODY, CMS_BACKGROUND, background);

			palette.set(CMP_BUTTON, CMS_NORMAL, normal);
			palette.set(CMP_BUTTON, CMS_HOT, hot);
			palette.set(CMP_BUTTON, CMS_PRESSED, pressed);
			palette.set(CMP_BUTTON, CMS_FOCUSED, checked);
			palette.set(CMP_BUTTON, CMS_CHECKED, checked);

			palette.set(CMP_SPLITBUTTON, CMS_BACKGROUND, background);
			palette.set(CMP_SPLITBUTTON, CMS_NORMAL, normal);
			palette.set(CMP_SPLITBUTTON, CMS_HOT, hot);
			palette.set(CMP_SPLITBUTTON, CMS_PRESSED, pressed);
			palette.set(CMP_SPLITBUTTON, CMS_FOCUSED, checked);
			palette.set(CMP_SPLITBUTTON, CMS_CHECKED, checked);

			palette.set(CMP_SPLITBUTTONDROPDOWN, CMS_BACKGROUND, background);
			palette.set(CMP_SPLITBUTTONDROPDOWN, CMS_NORMAL, normal);
			palette.set(CMP_SPLITBUTTONDROPDOWN, CMS_HOT, hot);
			palette.set(CMP_SPLITBUTTONDROPDOWN, CMS_PRESSED, pressed);
			palette.set(CMP_SPLITBUTTONDROPDOWN, CMS_FOCUSED, checked);
			palette.set(CMP_SPLITBUTTONDROPDOWN, CMS_CHECKED, checked);

			palette.set(CMP_DROPDOWNBUTTONGLYPH, CMS_BACKGROUND, background);
			palette.set(CMP_DROPDOWNBUTTONGLYPH, CMS_NORMAL, normal);
			palette.set(CMP_DROPDOWNBUTTONGLYPH, CMS_HOT, hot);
			palette.set(CMP_DROPDOWNBUTTONGLYPH, CMS_PRESSED, pressed);
			palette.set(CMP_DROPDOWNBUTTONGLYPH, CMS_FOCUSED, checked);
			palette.set(CMP_DROPDOWNBUTTONGLYPH, CMS_CHECKED, checked);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} command_module_material;
}
