#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスは拡張編集のマテリアルです。
	//
	inline struct exedit_material_t : material_t
	{
		inline auto register_pigment(int part_id, int state_id,
			const std::wstring& section, const std::wstring& sub_key,
			const StyleColor& background_color, const StyleColor& border_color, const StyleColor& text_color)
		{
			// 引数からピグメントを作成します。
			auto pigment = create_pigment(section, sub_key, background_color, border_color, text_color);

			// 作成したピグメントをパレットに追加します。
			palette.set(part_id, state_id, pigment);

			// 作成したピグメントを返します。
			return pigment;
		}

		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			register_pigment(
				WP_EXEDIT, EES_SELECTION_FILL,
				L"ExEdit", L"FrameRangeSelect",
				style_t::color_e::FrameRangeSelect,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_SELECTION_EDGE,
				L"ExEdit", L"FrameRangeBorder",
				style_t::color_e::FrameRangeSelect,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_SELECTION_BACKGROUND,
				L"ExEdit", L"FrameRangeOutside",
				style_t::color_e::FrameRangeOutside,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, 0,
				L"ExEdit", L"Background",
				style_t::color_e::LayerHeader,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_SCALE_PRIMARY,
				L"ExEdit", L"ScalePrimary",
				style_t::color_e::Border,
				style_t::color_e::Border,
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_SCALE_SECONDARY,
				L"ExEdit", L"ScaleSecondary",
				style_t::color_e::Border,
				style_t::color_e::Border,
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_SCENE_BUTTON,
				L"ExEdit", L"SceneButton",
				style_t::color_e::TextSelect,
				style_t::color_e::Border,
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_EVEN_LAYER_BUTTON_ACTIVE,
				L"ExEdit", L"EvenLayerButtonActive",
				style_t::color_e::Layer,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_EVEN_LAYER_BUTTON_INACTIVE,
				L"ExEdit", L"EvenLayerButtonInactive",
				style_t::color_e::ButtonBodyDisable,
				{},
				style_t::color_e::TextDisable);

			register_pigment(
				WP_EXEDIT, EES_EVEN_LAYER_BACKGROUND_ACTIVE,
				L"ExEdit", L"EvenLayerBackgroundActive",
				style_t::color_e::Layer,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_EVEN_LAYER_BACKGROUND_INACTIVE,
				L"ExEdit", L"EvenLayerBackgroundInactive",
				style_t::color_e::LayerDisable,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_EVEN_LAYER_BACKGROUND_SELECTED,
				L"ExEdit", L"EvenLayerBackgroundSelect",
				style_t::color_e::LayerSelect,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_ODD_LAYER_BUTTON_ACTIVE,
				L"ExEdit", L"OddLayerButtonActive",
				style_t::color_e::LayerHover,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_ODD_LAYER_BUTTON_INACTIVE,
				L"ExEdit", L"OddLayerButtonInactive",
				style_t::color_e::ButtonBodyDisable,
				{},
				style_t::color_e::TextDisable);

			register_pigment(
				WP_EXEDIT, EES_ODD_LAYER_BACKGROUND_ACTIVE,
				L"ExEdit", L"OddLayerBackgroundActive",
				style_t::color_e::LayerHover,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_ODD_LAYER_BACKGROUND_INACTIVE,
				L"ExEdit", L"OddLayerBackgroundInactive",
				style_t::color_e::LayerDisable,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_ODD_LAYER_BACKGROUND_SELECTED,
				L"ExEdit", L"OddLayerBackgroundSelect",
				style_t::color_e::LayerSelect,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_LAYER_LINE_LEFT,
				L"ExEdit", L"layerLineLeft",
				style_t::color_e::Border,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_LAYER_LINE_TOP,
				L"ExEdit", L"layerLineTop",
				style_t::color_e::Border,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_LAYER_LINE_RIGHT,
				L"ExEdit", L"layerLineRight",
				style_t::color_e::Border,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_LAYER_LINE_BOTTOM,
				L"ExEdit", L"layerLineBottom",
				style_t::color_e::Border,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_LAYER_LINE_SEPARATOR,
				L"ExEdit", L"layerLineSeparator",
				style_t::color_e::Border,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_GROUP_BACKGROUND,
				L"ExEdit", L"GroupBackground",
				style_t::color_e::LayerRange,
				{},
				style_t::color_e::Text);

			register_pigment(
				WP_EXEDIT, EES_FRAME_CURSOR,
				L"ExEdit", L"FrameCursor",
				style_t::color_e::FrameCursor,
				{},
				style_t::color_e::Text);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} exedit_material;
}
