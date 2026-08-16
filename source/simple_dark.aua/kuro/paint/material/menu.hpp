#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはメニューのマテリアルです。
	//
	inline struct menu_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto background = create_pigment(L"Menu", L"Background",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto bar_active = create_pigment(L"Menu", L"Active",
				style_t::color_e::GroupingHover,
				style_t::color_e::GroupingSeparator,
				style_t::color_e::Text);

			auto bar_inactive = create_pigment(L"Menu", L"Inactive",
				style_t::color_e::Grouping,
				style_t::color_e::GroupingSeparator,
				style_t::color_e::TextDisable);

			auto border = create_pigment(L"Menu", L"Border",
				style_t::color_e::WindowBorder,
				{},
				style_t::color_e::Text);

			auto gutter = create_pigment(L"Menu", L"Gutter",
				style_t::color_e::TitleHeader,
				{},
				style_t::color_e::Text);

			auto separator = create_pigment(L"Menu", L"Separator",
				style_t::color_e::TitleHeader,
				{},
				style_t::color_e::Text);

			auto item_normal = create_pigment(L"MenuItem", L"Normal",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto item_disabled = create_pigment(L"MenuItem", L"Disable",
				style_t::color_e::Background,
				{},
				style_t::color_e::TextDisable);

			auto item_hot = create_pigment(L"MenuItem", L"Hot",
				style_t::color_e::TextSelect,
				style_t::color_e::Border,
				style_t::color_e::Text);

			auto item_focus = create_pigment(L"MenuItem", L"Focus",
				{},
				style_t::color_e::BorderFocus,
				style_t::color_e::Text);

			auto icon_normal = create_pigment(L"MenuIcon", L"Normal",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto icon_disabled = create_pigment(L"MenuIcon", L"Disable",
				style_t::color_e::Background,
				{},
				style_t::color_e::TextDisable);

			palette.set(MENU_BARBACKGROUND, MB_ACTIVE, bar_active);
			palette.set(MENU_BARBACKGROUND, MB_INACTIVE, bar_inactive);

			palette.set(MENU_BARITEM, MBI_NORMAL, item_normal);
			palette.set(MENU_BARITEM, MBI_DISABLED, item_disabled);
			palette.set(MENU_BARITEM, MBI_HOT, item_hot);
			palette.set(MENU_BARITEM, MBI_DISABLEDHOT, item_disabled);
			palette.set(MENU_BARITEM, MBI_PUSHED, item_hot);
			palette.set(MENU_BARITEM, MBI_DISABLEDPUSHED, item_disabled);

			// Menu
			palette.set(MENU_POPUPBACKGROUND, 0, background);
			palette.set(MENU_POPUPBORDERS, 0, border);
			palette.set(MENU_POPUPGUTTER, 0, gutter);
			palette.set(MENU_POPUPSEPARATOR, 0, separator);

			// Menu (comdlg32)
			// (MPI_NORMAL = 1 かもしれません)
			palette.set(MENU_POPUPBACKGROUND, 1, background);
			palette.set(MENU_POPUPBORDERS, 1, border);
			palette.set(MENU_POPUPGUTTER, 1, gutter);
			palette.set(MENU_POPUPSEPARATOR, 1, separator);

			// Menu (comdlg32)
			// (MPI_HOT = 2 かもしれません)
			palette.set(MENU_POPUPBACKGROUND, 2, background);
			palette.set(MENU_POPUPBORDERS, 2, border);
			palette.set(MENU_POPUPGUTTER, 2, gutter);
			palette.set(MENU_POPUPSEPARATOR, 2, separator);

			// ImmersiveStart::Menu
			// (MPI_DISABLED = 3 かもしれません)
			palette.set(MENU_POPUPBACKGROUND, 3, background);
			palette.set(MENU_POPUPBORDERS, 3, border);
			palette.set(MENU_POPUPGUTTER, 3, gutter);
			palette.set(MENU_POPUPSEPARATOR, 3, separator);

			palette.set(MENU_POPUPITEM, MPI_NORMAL, item_normal);
			palette.set(MENU_POPUPITEM, MPI_DISABLED, item_disabled);
			palette.set(MENU_POPUPITEM, MPI_HOT, item_hot);
			palette.set(MENU_POPUPITEM, MPI_DISABLEDHOT, item_disabled);

			palette.set(MENU_POPUPITEMKBFOCUS, MPIKBFOCUS_NORMAL, item_focus);

			palette.set(MENU_POPUPITEMFOCUSABLE, MPI_NORMAL, item_normal);
			palette.set(MENU_POPUPITEMFOCUSABLE, MPI_DISABLED, item_disabled);
			palette.set(MENU_POPUPITEMFOCUSABLE, MPI_HOT, item_hot);
			palette.set(MENU_POPUPITEMFOCUSABLE, MPI_DISABLEDHOT, item_disabled);

			palette.set(MENU_POPUPCHECK, MC_CHECKMARKNORMAL, icon_normal);
			palette.set(MENU_POPUPCHECK, MC_CHECKMARKDISABLED, icon_disabled);
			palette.set(MENU_POPUPCHECK, MC_BULLETNORMAL, icon_normal);
			palette.set(MENU_POPUPCHECK, MC_BULLETDISABLED, icon_disabled);

			palette.set(MENU_POPUPCHECKBACKGROUND, MCB_DISABLED, icon_disabled);
			palette.set(MENU_POPUPCHECKBACKGROUND, MCB_NORMAL, icon_normal);
			palette.set(MENU_POPUPCHECKBACKGROUND, MCB_BITMAP, icon_normal);

			palette.set(MENU_POPUPSUBMENU, MSM_NORMAL, icon_normal);
			palette.set(MENU_POPUPSUBMENU, MSM_DISABLED, icon_disabled);

			palette.set(MENU_SYSTEMCLOSE, MSYSC_NORMAL, icon_normal);
			palette.set(MENU_SYSTEMCLOSE, MSYSC_DISABLED, icon_disabled);

			palette.set(MENU_SYSTEMMAXIMIZE, MSYSMX_NORMAL, icon_normal);
			palette.set(MENU_SYSTEMMAXIMIZE, MSYSMX_DISABLED, icon_disabled);

			palette.set(MENU_SYSTEMMINIMIZE, MSYSMN_NORMAL, icon_normal);
			palette.set(MENU_SYSTEMMINIMIZE, MSYSMN_DISABLED, icon_disabled);

			palette.set(MENU_SYSTEMRESTORE, MSYSR_NORMAL, icon_normal);
			palette.set(MENU_SYSTEMRESTORE, MSYSR_DISABLED, icon_disabled);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} menu_material;
}
