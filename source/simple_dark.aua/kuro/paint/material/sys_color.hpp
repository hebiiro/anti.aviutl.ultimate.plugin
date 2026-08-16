#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはシステムカラーのマテリアルです。
	//
	inline struct sys_color_material_t : material_t
	{
		pigment_t window;
		pigment_t window_text;
		pigment_t highlight;
		pigment_t highlight_text;
		pigment_t btn_face;
		pigment_t btn_text;
		pigment_t btn_highlight;
		pigment_t gray_text;

		pigment_t single_border;

		struct single_edge_t {
			pigment_t top_left;
			pigment_t bottom_right;
		};

		struct edge_t {
			single_edge_t outer;
			single_edge_t inner;
		};

		single_edge_t single_raised_edge;
		single_edge_t single_sunken_edge;
		edge_t raised_edge;
		edge_t sunken_edge;
		edge_t bump_edge;
		edge_t etched_edge;

		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			window = create_background_pigment(L"SysColor", L"Window", style_t::color_e::Background);
			window_text = create_background_pigment(L"SysColor", L"WindowText", style_t::color_e::Text);
			highlight = create_background_pigment(L"SysColor", L"Highlight", style_t::color_e::ButtonBodySelect);
			highlight_text = create_background_pigment(L"SysColor", L"HighlightText", style_t::color_e::Text);
			btn_face = create_background_pigment(L"SysColor", L"BtnFace", style_t::color_e::ButtonBody);
			btn_text = create_background_pigment(L"SysColor", L"BtnText", style_t::color_e::Text);
			btn_highlight = create_background_pigment(L"SysColor", L"BtnHighlight", style_t::color_e::ButtonBody);
			gray_text = create_background_pigment(L"SysColor", L"GrayText", style_t::color_e::TextDisable);

			single_border = create_background_pigment(L"SysColor", L"SingleBorder", style_t::color_e::Border);

			single_raised_edge.top_left = create_background_pigment(L"SysColor", L"SingleRaisedEdgeTopLeft", style_t::color_e::Border);
			single_raised_edge.bottom_right = create_background_pigment(L"SysColor", L"SingleRaisedEdgeBottomRight", style_t::color_e::Border);

			single_sunken_edge.top_left = create_background_pigment(L"SysColor", L"SingleSunkenEdgeTopLeft", style_t::color_e::Border);
			single_sunken_edge.bottom_right = create_background_pigment(L"SysColor", L"SingleSunkenEdgeBottomRight", style_t::color_e::Border);

			raised_edge.outer.top_left = create_background_pigment(L"SysColor", L"RaisedEdgeOuterTopLeft", style_t::color_e::Border);
			raised_edge.outer.bottom_right = create_background_pigment(L"SysColor", L"RaisedEdgeOuterBottomRight", style_t::color_e::Border);
			raised_edge.inner.top_left = create_background_pigment(L"SysColor", L"RaisedEdgeInnerTopLeft", style_t::color_e::ButtonBody);
			raised_edge.inner.bottom_right = create_background_pigment(L"SysColor", L"RaisedEdgeInnerBottomRight", style_t::color_e::ButtonBody);

			sunken_edge.outer.top_left = create_background_pigment(L"SysColor", L"SunkenEdgeOuterTopLeft", style_t::color_e::ButtonBody);
			sunken_edge.outer.bottom_right = create_background_pigment(L"SysColor", L"SunkenEdgeOuterBottomRight", style_t::color_e::ButtonBody);
			sunken_edge.inner.top_left = create_background_pigment(L"SysColor", L"SunkenEdgeInnerTopLeft", style_t::color_e::Border);
			sunken_edge.inner.bottom_right = create_background_pigment(L"SysColor", L"SunkenEdgeInnerBottomRight", style_t::color_e::Border);

			bump_edge.outer.top_left = create_background_pigment(L"SysColor", L"BumpEdgeOuterTopLeft", style_t::color_e::ButtonBodyHover);
			bump_edge.outer.bottom_right = create_background_pigment(L"SysColor", L"BumpEdgeOuterBottomRight", style_t::color_e::ButtonBody);
			bump_edge.inner.top_left = create_background_pigment(L"SysColor", L"BumpEdgeInnerTopLeft", style_t::color_e::ButtonBody);
			bump_edge.inner.bottom_right = create_background_pigment(L"SysColor", L"BumpEdgeInnerBottomRight", style_t::color_e::ButtonBodyHover);

			etched_edge.outer.top_left = create_background_pigment(L"SysColor", L"EtchedEdgeOuterTopLeft", style_t::color_e::ButtonBody);
			etched_edge.outer.bottom_right = create_background_pigment(L"SysColor", L"EtchedEdgeOuterBottomRight", style_t::color_e::ButtonBodyHover);
			etched_edge.inner.top_left = create_background_pigment(L"SysColor", L"EtchedEdgeInnerTopLeft", style_t::color_e::ButtonBodyHover);
			etched_edge.inner.bottom_right = create_background_pigment(L"SysColor", L"EtchedEdgeInnerBottomRight", style_t::color_e::ButtonBody);

			palette.set(COLOR_WINDOW, 0, window);
			palette.set(COLOR_WINDOWTEXT, 0, window_text);
			palette.set(COLOR_HIGHLIGHT, 0, highlight);
			palette.set(COLOR_HIGHLIGHTTEXT, 0, highlight_text);
			palette.set(COLOR_BTNFACE, 0, btn_face);
			palette.set(COLOR_BTNTEXT, 0, btn_text);
			palette.set(COLOR_BTNHIGHLIGHT, 0, btn_highlight);
			palette.set(COLOR_GRAYTEXT, 0, gray_text);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// 軽量エッジを描画します。
		//
		inline static void draw_single_edge(HDC dc, LPCRECT rc, single_edge_t& edge)
		{
			return paint::draw_single_edge(dc, rc,
				edge.top_left.background.get_win32_color(),
				edge.bottom_right.background.get_win32_color());
		}

		//
		// エッジを描画します。
		//
		inline static void draw_edge(HDC dc, LPCRECT rc, edge_t& edge)
		{
			return paint::draw_edge(dc, rc,
				edge.outer.top_left.background.get_win32_color(),
				edge.outer.bottom_right.background.get_win32_color(),
				edge.inner.top_left.background.get_win32_color(),
				edge.inner.bottom_right.background.get_win32_color());
		}
	} sys_color_material;

	//
	// 軽量ボーダーを描画します。
	//
	inline void draw_single_border(HDC dc, LPCRECT rc)
	{
		return frame_rect(dc, rc, sys_color_material.single_border.background.get_win32_color(), 1);
	}

	//
	// 浮き上がり軽量エッジを描画します。
	//
	inline void draw_single_raised_edge(HDC dc, LPCRECT rc)
	{
		return sys_color_material.draw_single_edge(dc, rc, sys_color_material.single_raised_edge);
	}

	//
	// 沈み込み軽量エッジを描画します。
	//
	inline void draw_single_sunken_edge(HDC dc, LPCRECT rc)
	{
		return sys_color_material.draw_single_edge(dc, rc, sys_color_material.single_sunken_edge);
	}

	//
	// 浮き上がりエッジを描画します。
	//
	inline void draw_raised_edge(HDC dc, LPCRECT rc)
	{
		return sys_color_material.draw_edge(dc, rc, sys_color_material.raised_edge);
	}

	//
	// 沈み込みエッジを描画します。
	//
	inline void draw_sunken_edge(HDC dc, LPCRECT rc)
	{
		return sys_color_material.draw_edge(dc, rc, sys_color_material.sunken_edge);
	}

	//
	// 凸エッジを描画します。
	//
	inline void draw_bump_edge(HDC dc, LPCRECT rc)
	{
		return sys_color_material.draw_edge(dc, rc, sys_color_material.bump_edge);
	}

	//
	// 凹エッジを描画します。
	//
	inline void draw_etched_edge(HDC dc, LPCRECT rc)
	{
		return sys_color_material.draw_edge(dc, rc, sys_color_material.etched_edge);
	}
}
