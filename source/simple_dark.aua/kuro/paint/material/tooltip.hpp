#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはツールチップのマテリアルです。
	//
	inline struct tooltip_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto ttss_normal = create_pigment(L"ToolTip", L"Normal",
				style_t::color_e::Background,
				style_t::color_e::Border,
				style_t::color_e::Text);

			palette.set(TTP_STANDARD, 0, ttss_normal);
			palette.set(TTP_STANDARD, TTSS_NORMAL, ttss_normal);
			palette.set(TTP_STANDARD, TTSS_LINK, ttss_normal);
			palette.set(TTP_STANDARDTITLE, 0, ttss_normal);
			palette.set(TTP_STANDARDTITLE, TTSS_NORMAL, ttss_normal);
			palette.set(TTP_STANDARDTITLE, TTSS_LINK, ttss_normal);
			palette.set(TTP_BALLOON, 0, ttss_normal);
			palette.set(TTP_BALLOON, TTBS_NORMAL, ttss_normal);
			palette.set(TTP_BALLOON, TTBS_LINK, ttss_normal);
			palette.set(TTP_BALLOONTITLE, 0, ttss_normal);
			palette.set(TTP_BALLOONTITLE, TTBS_NORMAL, ttss_normal);
			palette.set(TTP_BALLOONTITLE, TTBS_LINK, ttss_normal);
			palette.set(TTP_CLOSE, 0, ttss_normal);
			palette.set(TTP_CLOSE, TTCS_NORMAL, ttss_normal);
			palette.set(TTP_CLOSE, TTCS_HOT, ttss_normal);
			palette.set(TTP_CLOSE, TTCS_PRESSED, ttss_normal);
			palette.set(TTP_BALLOONSTEM, 0, ttss_normal);
			palette.set(TTP_BALLOONSTEM, TTBSS_POINTINGUPLEFTWALL, ttss_normal);
			palette.set(TTP_BALLOONSTEM, TTBSS_POINTINGUPCENTERED, ttss_normal);
			palette.set(TTP_BALLOONSTEM, TTBSS_POINTINGUPRIGHTWALL, ttss_normal);
			palette.set(TTP_BALLOONSTEM, TTBSS_POINTINGDOWNRIGHTWALL, ttss_normal);
			palette.set(TTP_BALLOONSTEM, TTBSS_POINTINGDOWNCENTERED, ttss_normal);
			palette.set(TTP_BALLOONSTEM, TTBSS_POINTINGDOWNLEFTWALL, ttss_normal);
			palette.set(TTP_WRENCH, 0, ttss_normal);
			palette.set(TTP_WRENCH, TTWS_NORMAL, ttss_normal);
			palette.set(TTP_WRENCH, TTWS_HOT, ttss_normal);
			palette.set(TTP_WRENCH, TTWS_PRESSED, ttss_normal);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} tooltip_material;
}
