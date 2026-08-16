#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはヘッダーコントロールのマテリアルです。
	//
	inline struct header_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto his_background = create_pigment(L"Header", L"Background",
				style_t::color_e::TitleHeader,
				{},
				style_t::color_e::Text);

			auto his_normal = create_pigment(L"Header", L"Normal",
				style_t::color_e::TitleHeader,
				style_t::color_e::Border,
				style_t::color_e::Text);

			auto his_disabled = create_pigment(L"Header", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				style_t::color_e::Border,
				style_t::color_e::TextDisable);

			auto his_hot = create_pigment(L"Header", L"Hot",
				style_t::color_e::ButtonBodyHover,
				style_t::color_e::BorderFocus,
				style_t::color_e::Text);

			auto his_pressed = create_pigment(L"Header", L"Press",
				style_t::color_e::ButtonBodyPress,
				style_t::color_e::BorderSelect,
				style_t::color_e::Text);

			palette.set(0, 1, his_background);

			palette.set(HP_HEADERITEM, HIS_NORMAL, his_normal);
			palette.set(HP_HEADERITEM, HIS_SORTEDNORMAL, his_normal);
			palette.set(HP_HEADERITEM, HIS_ICONNORMAL, his_normal);
			palette.set(HP_HEADERITEM, HIS_ICONSORTEDNORMAL, his_normal);

			palette.set(HP_HEADERITEM, HIS_HOT, his_hot);
			palette.set(HP_HEADERITEM, HIS_SORTEDHOT, his_hot);
			palette.set(HP_HEADERITEM, HIS_ICONHOT, his_hot);
			palette.set(HP_HEADERITEM, HIS_ICONSORTEDHOT, his_hot);

			palette.set(HP_HEADERITEM, HIS_PRESSED, his_pressed);
			palette.set(HP_HEADERITEM, HIS_SORTEDPRESSED, his_pressed);
			palette.set(HP_HEADERITEM, HIS_ICONPRESSED, his_pressed);
			palette.set(HP_HEADERITEM, HIS_ICONSORTEDPRESSED, his_pressed);

			palette.set(HP_HEADERITEMLEFT, HDDFS_NORMAL, his_normal);
			palette.set(HP_HEADERITEMLEFT, HDDFS_SOFTHOT, his_normal);
			palette.set(HP_HEADERITEMLEFT, HDDFS_HOT, his_hot);

			palette.set(HP_HEADERITEMRIGHT, HDDFS_NORMAL, his_normal);
			palette.set(HP_HEADERITEMRIGHT, HDDFS_SOFTHOT, his_normal);
			palette.set(HP_HEADERITEMRIGHT, HDDFS_HOT, his_hot);

			palette.set(HP_HEADERDROPDOWN, HDDFS_NORMAL, his_normal);
			palette.set(HP_HEADERDROPDOWN, HDDFS_SOFTHOT, his_normal);
			palette.set(HP_HEADERDROPDOWN, HDDFS_HOT, his_hot);

			palette.set(HP_HEADERDROPDOWNFILTER, HDDFS_NORMAL, his_normal);
			palette.set(HP_HEADERDROPDOWNFILTER, HDDFS_SOFTHOT, his_normal);
			palette.set(HP_HEADERDROPDOWNFILTER, HDDFS_HOT, his_hot);

			palette.set(HP_HEADERSORTARROW, HSAS_SORTEDUP, his_normal);
			palette.set(HP_HEADERSORTARROW, HSAS_SORTEDDOWN, his_normal);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} header_material;
}
