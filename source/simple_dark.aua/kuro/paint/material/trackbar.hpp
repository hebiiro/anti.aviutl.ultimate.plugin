#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはトラックバーのマテリアルです。
	//
	inline struct trackbar_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto track_normal = create_pigment(L"TrackBar", L"Normal",
				style_t::color_e::ButtonBody,
				style_t::color_e::WindowBorder,
				style_t::color_e::Text);

			auto track_selected = create_pigment(L"TrackBar", L"Select",
#if 1
				style_t::color_e::SliderCursor,
				style_t::color_e::TrackBarRange,
				style_t::color_e::Text);
#else
				style_t::color_e::ButtonBodySelect,
				style_t::color_e::WindowBorder,
				style_t::color_e::Text);
#endif
			auto track_disabled = create_pigment(L"TrackBar", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				style_t::color_e::WindowBorder,
				style_t::color_e::Text);
#if 1
			auto thumb_normal = create_pigment(L"TrackBarThumb", L"Normal",
				style_t::color_e::SliderThumbBody,
				style_t::color_e::SliderThumbBorder,
				style_t::color_e::Text);

			auto thumb_disabled = create_pigment(L"TrackBarThumb", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				style_t::color_e::SliderThumbBorder,
				style_t::color_e::TextDisable);

			auto thumb_hot = create_pigment(L"TrackBarThumb", L"Hot",
				style_t::color_e::SliderThumbBodyFocus,
				style_t::color_e::SliderThumbBorder,
				style_t::color_e::Text);

			auto thumb_pressed = create_pigment(L"TrackBarThumb", L"Press",
				style_t::color_e::SliderThumbBodyFocus,
				style_t::color_e::SliderThumbBorder,
				style_t::color_e::Text);

			auto thumb_focused = thumb_pressed;
#else
			auto thumb_normal = create_pigment(L"TrackBarThumb", L"Normal",
				style_t::color_e::ButtonBodyHover,
				style_t::color_e::Border,
				style_t::color_e::Text);

			auto thumb_disabled = create_pigment(L"TrackBarThumb", L"Disable",
				style_t::color_e::ButtonBodyDisable,
				style_t::color_e::Border,
				style_t::color_e::TextDisable);

			auto thumb_hot = create_pigment(L"TrackBarThumb", L"Hot",
				style_t::color_e::ButtonBodySelect,
				style_t::color_e::BorderFocus,
				style_t::color_e::Text);

			auto thumb_pressed = create_pigment(L"TrackBarThumb", L"Press",
				style_t::color_e::ButtonBodyPress,
				style_t::color_e::BorderSelect,
				style_t::color_e::Text);

			auto thumb_focused = thumb_pressed;
#endif
			palette.set(TKP_TRACK, TRS_NORMAL, track_normal);
			palette.set(TKP_TRACK, TRS_SELECTED, track_selected);
			palette.set(TKP_TRACK, TRS_DISABLED, track_disabled);
			palette.set(TKP_THUMB, TUS_NORMAL, thumb_normal);
			palette.set(TKP_THUMB, TUS_DISABLED, thumb_disabled);
			palette.set(TKP_THUMB, TUS_HOT, thumb_hot);
			palette.set(TKP_THUMB, TUS_PRESSED, thumb_pressed);
			palette.set(TKP_THUMB, TUS_FOCUSED, thumb_focused);
			palette.set(TKP_THUMBBOTTOM, TUS_NORMAL, thumb_normal);
			palette.set(TKP_THUMBBOTTOM, TUS_DISABLED, thumb_disabled);
			palette.set(TKP_THUMBBOTTOM, TUS_HOT, thumb_hot);
			palette.set(TKP_THUMBBOTTOM, TUS_PRESSED, thumb_pressed);
			palette.set(TKP_THUMBBOTTOM, TUS_FOCUSED, thumb_focused);
			palette.set(TKP_THUMBTOP, TUS_NORMAL, thumb_normal);
			palette.set(TKP_THUMBTOP, TUS_DISABLED, thumb_disabled);
			palette.set(TKP_THUMBTOP, TUS_HOT, thumb_hot);
			palette.set(TKP_THUMBTOP, TUS_PRESSED, thumb_pressed);
			palette.set(TKP_THUMBTOP, TUS_FOCUSED, thumb_focused);

			palette.set(TKP_TRACKVERT, TRS_NORMAL, track_normal);
			palette.set(TKP_TRACKVERT, TRS_SELECTED, track_selected);
			palette.set(TKP_TRACKVERT, TRS_DISABLED, track_disabled);
			palette.set(TKP_THUMBVERT, TUS_NORMAL, thumb_normal);
			palette.set(TKP_THUMBVERT, TUS_DISABLED, thumb_disabled);
			palette.set(TKP_THUMBVERT, TUS_HOT, thumb_hot);
			palette.set(TKP_THUMBVERT, TUS_PRESSED, thumb_pressed);
			palette.set(TKP_THUMBVERT, TUS_FOCUSED, thumb_focused);
			palette.set(TKP_THUMBLEFT, TUS_NORMAL, thumb_normal);
			palette.set(TKP_THUMBLEFT, TUS_DISABLED, thumb_disabled);
			palette.set(TKP_THUMBLEFT, TUS_HOT, thumb_hot);
			palette.set(TKP_THUMBLEFT, TUS_PRESSED, thumb_pressed);
			palette.set(TKP_THUMBLEFT, TUS_FOCUSED, thumb_focused);
			palette.set(TKP_THUMBRIGHT, TUS_NORMAL, thumb_normal);
			palette.set(TKP_THUMBRIGHT, TUS_DISABLED, thumb_disabled);
			palette.set(TKP_THUMBRIGHT, TUS_HOT, thumb_hot);
			palette.set(TKP_THUMBRIGHT, TUS_PRESSED, thumb_pressed);
			palette.set(TKP_THUMBRIGHT, TUS_FOCUSED, thumb_focused);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} trackbar_material;
}
