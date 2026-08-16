#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはスクロールバーのマテリアルです。
	//
	inline struct scrollbar_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto background = create_pigment(L"ScrollBar", L"Background",
				style_t::color_e::TitleHeader,
				{},
				style_t::color_e::Text);

			auto size_box = create_pigment(L"ScrollBar", L"SizeBox",
				style_t::color_e::Background,
				{},
				style_t::color_e::TextDisable);

			auto button_normal = create_pigment(L"ScrollBarButton", L"Normal",
				style_t::color_e::ButtonBody,
				style_t::color_e::Border,
				style_t::color_e::Text);

			auto button_hover = create_pigment(L"ScrollBarButton", L"Hover",
				style_t::color_e::ButtonBodyHover,
				style_t::color_e::BorderSelect,
				style_t::color_e::Text);

			auto button_hot = create_pigment(L"ScrollBarButton", L"Hot",
				style_t::color_e::ButtonBodySelect,
				style_t::color_e::BorderFocus,
				style_t::color_e::Text);

			auto button_pressed = create_pigment(L"ScrollBarButton", L"Press",
				style_t::color_e::ButtonBodyPress,
				style_t::color_e::BorderSelect,
				style_t::color_e::Text);

			auto button_disabled = create_pigment(L"ScrollBarButton", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				style_t::color_e::Border,
				style_t::color_e::TextDisable);

			auto thumb_normal = button_normal;
			auto thumb_hover = button_hover;
			auto thumb_hot = button_hot;
			auto thumb_pressed = button_pressed;
			auto thumb_disabled = button_disabled;

			auto track_normal = button_normal;
			auto track_hover = button_hover;
			auto track_hot = button_hot;
			auto track_pressed = button_pressed;
			auto track_disabled = button_disabled;

			palette.set(0, 0, background);

			palette.set(SBP_ARROWBTN, ABS_LEFTNORMAL, button_normal);
			palette.set(SBP_ARROWBTN, ABS_LEFTHOVER, button_hover);
			palette.set(SBP_ARROWBTN, ABS_LEFTHOT, button_hot);
			palette.set(SBP_ARROWBTN, ABS_LEFTPRESSED, button_pressed);
			palette.set(SBP_ARROWBTN, ABS_LEFTDISABLED, button_disabled);

			palette.set(SBP_ARROWBTN, ABS_RIGHTNORMAL, button_normal);
			palette.set(SBP_ARROWBTN, ABS_RIGHTHOVER, button_hover);
			palette.set(SBP_ARROWBTN, ABS_RIGHTHOT, button_hot);
			palette.set(SBP_ARROWBTN, ABS_RIGHTPRESSED, button_pressed);
			palette.set(SBP_ARROWBTN, ABS_RIGHTDISABLED, button_disabled);

			palette.set(SBP_ARROWBTN, ABS_UPNORMAL, button_normal);
			palette.set(SBP_ARROWBTN, ABS_UPHOVER, button_hover);
			palette.set(SBP_ARROWBTN, ABS_UPHOT, button_hot);
			palette.set(SBP_ARROWBTN, ABS_UPPRESSED, button_pressed);
			palette.set(SBP_ARROWBTN, ABS_UPDISABLED, button_disabled);

			palette.set(SBP_ARROWBTN, ABS_DOWNNORMAL, button_normal);
			palette.set(SBP_ARROWBTN, ABS_DOWNHOVER, button_hover);
			palette.set(SBP_ARROWBTN, ABS_DOWNHOT, button_hot);
			palette.set(SBP_ARROWBTN, ABS_DOWNPRESSED, button_pressed);
			palette.set(SBP_ARROWBTN, ABS_DOWNDISABLED, button_disabled);

			palette.set(SBP_THUMBBTNHORZ, SCRBS_NORMAL, thumb_normal);
			palette.set(SBP_THUMBBTNHORZ, SCRBS_HOVER, thumb_hover);
			palette.set(SBP_THUMBBTNHORZ, SCRBS_HOT, thumb_hot);
			palette.set(SBP_THUMBBTNHORZ, SCRBS_PRESSED, thumb_pressed);
			palette.set(SBP_THUMBBTNHORZ, SCRBS_DISABLED, thumb_disabled);

			palette.set(SBP_THUMBBTNVERT, SCRBS_NORMAL, thumb_normal);
			palette.set(SBP_THUMBBTNVERT, SCRBS_HOVER, thumb_hover);
			palette.set(SBP_THUMBBTNVERT, SCRBS_HOT, thumb_hot);
			palette.set(SBP_THUMBBTNVERT, SCRBS_PRESSED, thumb_pressed);
			palette.set(SBP_THUMBBTNVERT, SCRBS_DISABLED, thumb_disabled);

			palette.set(SBP_LOWERTRACKHORZ, SCRBS_NORMAL, track_normal);
			palette.set(SBP_LOWERTRACKHORZ, SCRBS_HOVER, track_hover);
			palette.set(SBP_LOWERTRACKHORZ, SCRBS_HOT, track_hot);
			palette.set(SBP_LOWERTRACKHORZ, SCRBS_PRESSED, track_pressed);
			palette.set(SBP_LOWERTRACKHORZ, SCRBS_DISABLED, track_disabled);

			palette.set(SBP_UPPERTRACKHORZ, SCRBS_NORMAL, track_normal);
			palette.set(SBP_UPPERTRACKHORZ, SCRBS_HOVER, track_hover);
			palette.set(SBP_UPPERTRACKHORZ, SCRBS_HOT, track_hot);
			palette.set(SBP_UPPERTRACKHORZ, SCRBS_PRESSED, track_pressed);
			palette.set(SBP_UPPERTRACKHORZ, SCRBS_DISABLED, track_disabled);

			palette.set(SBP_LOWERTRACKVERT, SCRBS_NORMAL, track_normal);
			palette.set(SBP_LOWERTRACKVERT, SCRBS_HOVER, track_hover);
			palette.set(SBP_LOWERTRACKVERT, SCRBS_HOT, track_hot);
			palette.set(SBP_LOWERTRACKVERT, SCRBS_PRESSED, track_pressed);
			palette.set(SBP_LOWERTRACKVERT, SCRBS_DISABLED, track_disabled);

			palette.set(SBP_UPPERTRACKVERT, SCRBS_NORMAL, track_normal);
			palette.set(SBP_UPPERTRACKVERT, SCRBS_HOVER, track_hover);
			palette.set(SBP_UPPERTRACKVERT, SCRBS_HOT, track_hot);
			palette.set(SBP_UPPERTRACKVERT, SCRBS_PRESSED, track_pressed);
			palette.set(SBP_UPPERTRACKVERT, SCRBS_DISABLED, track_disabled);

			palette.set(SBP_GRIPPERHORZ, SCRBS_NORMAL, thumb_normal);
			palette.set(SBP_GRIPPERHORZ, SCRBS_HOVER, thumb_hover);
			palette.set(SBP_GRIPPERHORZ, SCRBS_HOT, thumb_hot);
			palette.set(SBP_GRIPPERHORZ, SCRBS_PRESSED, thumb_pressed);
			palette.set(SBP_GRIPPERHORZ, SCRBS_DISABLED, thumb_disabled);

			palette.set(SBP_GRIPPERVERT, SCRBS_NORMAL, thumb_normal);
			palette.set(SBP_GRIPPERVERT, SCRBS_HOVER, thumb_hover);
			palette.set(SBP_GRIPPERVERT, SCRBS_HOT, thumb_hot);
			palette.set(SBP_GRIPPERVERT, SCRBS_PRESSED, thumb_pressed);
			palette.set(SBP_GRIPPERVERT, SCRBS_DISABLED, thumb_disabled);

			palette.set(SBP_SIZEBOX, SZB_RIGHTALIGN, size_box);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} scrollbar_material;
}
