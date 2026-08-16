#pragma once

namespace apn::dark::kuro::paint::symbol
{
#if 1
	//
	// シンボル描画用のフォント名です。
	//
	inline static constexpr auto k_font_name = L"Segoe Fluent Icons";

	//
	// シンボル描画用の縦書きフォント名です。
	//
	inline static constexpr auto k_vert_font_name = L"@" L"Segoe Fluent Icons";
#else
	//
	// シンボル描画用のフォント名です。
	//
	inline static constexpr auto k_font_name = L"Segoe MDL2 Assets";

	//
	// シンボル描画用の縦書きフォント名です。
	//
	inline static constexpr auto k_vert_font_name = L"@" L"Segoe MDL2 Assets";
#endif
	//
	// シンボルの文字コードです。
	//
	enum : wchar_t
	{
		k_chevron_down = 0xe70d, // 下矢印
		k_chevron_up = 0xe70e, // 上矢印
		k_chevron_left = 0xe76b, // 左矢印
		k_chevron_right = 0xe76c, // 右矢印
//		k_inking_caret = 0xed65, // 右下矢印

		k_chevron_up_small = 0xe96d, // 小さい上矢印
		k_chevron_down_small = 0xe96e, // 小さい下矢印
		k_chevron_left_small = 0xe96f, // 小さい左矢印
		k_chevron_right_small = 0xe970, // 小さい右矢印

		k_chevron_up_med = 0xe971, // 中くらいの上矢印
		k_chevron_down_med = 0xe972, // 中くらいの下矢印
		k_chevron_left_med = 0xe973, // 中くらいの左矢印
		k_chevron_right_med = 0xe974, // 中くらいの右矢印

		k_caret_solid_left = 0xf08d, // 三角左矢印
		k_caret_solid_down = 0xf08e, // 三角下矢印
		k_caret_solid_right = 0xf08f, // 三角右矢印
		k_caret_solid_up = 0xf090, // 三角上矢印

		k_caret_left8 = 0xedd5,
		k_caret_right8 = 0xedd6,
		k_caret_up8 = 0xedd7,
		k_caret_down8 = 0xedd8,
		k_caret_left_solid8 = 0xedd9,
		k_caret_right_solid8 = 0xedda,
		k_caret_up_solid8 = 0xeddb,
		k_caret_down_solid8 = 0xeddc,
		k_caret_bottom_right_solid_center8 = 0xf169,

		k_global_nav_button = 0xe700,
		k_brightness = 0xe706,
		k_edit = 0xe70f, // 編集
		k_add = 0xe710, // 追加
		k_cancel = 0xe711, // キャンセル
		k_more = 0xe712,
		k_settings = 0xe713,
		k_zoom = 0xe71e,
		k_zoom_out = 0xe71f,
		k_search = 0xe721,
		k_forward = 0xe72a, // ブラウザの「進む」
		k_back = 0xe72b, // ブラウザの「戻る」
		k_refresh = 0xe72c, // ブラウザの「更新」
		k_share = 0xe72d, // ブラウザの「共有」
		k_remove = 0xe738, // 削除
		k_checkbox = 0xe739,
		k_checkbox_composite = 0xe73a,
		k_checkbox_fill = 0xe73b,
		k_checkbox_indeterminate = 0xe73c,
		k_checkbox_composite_reversed = 0xe73d,
		k_check_mark = 0xe73e,
		k_back_to_window = 0xe73f, // 「元に戻す」に使えるかもしれない。
		k_fullScreen = 0xe740, // 「最大化」に使えるかもしれない。
		k_sip_move = 0xe759, // 「四方向移動」に使えるかもしれない。
		k_move = 0xe7c2, // 「四方向移動」
		k_pan_mode = 0xece9, // 「四方向移動」に使えるかもしれない。

		k_gripper_bar_horizontal = 0xe76f,
		k_gripper_bar_vertical = 0xe784,
		k_gripper_resize = 0xe788,
		k_grid_view = 0xf0e2,
		k_grid_view_group = 0xf207,
		k_add_to= 0xecc8,
		k_remove_from= 0xecc9,
		k_radio_btn_off= 0xecca,
		k_radio_btn_on= 0xeccb,
		k_radio_bullet = 0xe915, // メニューのラジオバレット
		k_radio_bullet2 = 0xeccc, // メニューのラジオバレット

		k_chrome_close = 0xe8bb, // 「閉じる」
		k_chrome_minimize = 0xe921, // 「最小化」
		k_chrome_maximize = 0xe922, // 「最大化」
		k_chrome_restore = 0xe923, // 「元のサイズに戻す」

		k_chrome_close_contrast = 0xef2c,
		k_chrome_minimize_contrast = 0xef2d,
		k_chrome_maximize_contrast = 0xef2e,
		k_chrome_restore_contrast = 0xef2f,

		k_eye_dropper = 0xef3c, // スポイト
	};
}

namespace apn::dark::kuro::paint
{
	//
	// 縁の幅を整数で返します。
	//
	inline int get_border_width_as_int()
	{
		return hive.border.width / 10;
	}

	//
	// 縁の幅を実数で返します。
	//
	inline float get_border_width_as_float()
	{
		return hive.border.width / 10.0f;
	}

	//
	// 丸みを整数で返します。
	//
	inline int get_round_as_int(int base_round_size)
	{
		return ::MulDiv(base_round_size, hive.round.size, 100);
	}

	//
	// 丸みを実数で返します。
	//
	inline float get_round_as_float(float base_round_size)
	{
		return base_round_size * hive.round.size / 100.0f;
	}

	//
	// グラデーション終了色のアルファを実数で返します。
	//
	inline float get_gradient_end_alpha()
	{
		return hive.gradient.end_alpha / 100.0f;
	}

	//
	// 影のオフセットを整数で返します。
	//
	inline POINT get_shadow_offset_as_int()
	{
		return { hive.shadow.offset.x / 10, hive.shadow.offset.y / 10 };
	}

	//
	// 影のサイズを整数で返します。
	//
	inline int get_shadow_size_as_int()
	{
		return hive.shadow.size / 10;
	}

	//
	// 指定されたペンの色を返します。
	//
	inline COLORREF get_pen_color(HPEN pen)
	{
		LOGPEN lp = {};
		if (::GetObject(pen, sizeof(lp), &lp) != sizeof(lp)) return CLR_NONE;
		return lp.lopnColor;
	}

	//
	// 指定されたブラシの色を返します。
	//
	inline COLORREF get_brush_color(HBRUSH brush)
	{
		LOGBRUSH lb = {};
		if (::GetObject(brush, sizeof(lb), &lb) != sizeof(lb)) return CLR_NONE;
		if (lb.lbStyle != BS_SOLID) return CLR_NONE;
		return lb.lbColor;
	}

	//
	// 指定された矩形とモードで描画領域をクリップします。
	//
	inline void clip_rect(HDC dc, LPCRECT rc, int mode = RGN_COPY)
	{
		my::gdi::unique_ptr<HRGN> rgn(::CreateRectRgnIndirect(rc));

		::ExtSelectClipRgn(dc, rgn.get(), mode);
	}

	//
	// 指定された色で指定された矩形を塗りつぶします。
	//
	inline void fill_rect(HDC dc, LPCRECT rc, COLORREF fill_color)
	{
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(fill_color));
		my::gdi::selector brush_selector(dc, brush.get());

		hive.orig.PatBlt(dc, rc->left, rc->top, w, h, PATCOPY);
	}

	//
	// 指定された色と幅で指定された矩形の縁を描画します。
	//
	inline void frame_rect(HDC dc, LPCRECT rc, COLORREF edge_color, int edge_width)
	{
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(edge_color));
		my::gdi::selector brush_selector(dc, brush.get());

		hive.orig.PatBlt(dc, rc->left, rc->top, w, edge_width, PATCOPY);
		hive.orig.PatBlt(dc, rc->left, rc->top, edge_width, h, PATCOPY);
		hive.orig.PatBlt(dc, rc->left, rc->bottom - edge_width, w, edge_width, PATCOPY);
		hive.orig.PatBlt(dc, rc->right - edge_width, rc->top, edge_width, h, PATCOPY);
	}

	//
	// 矩形を描画します。
	//
	inline void draw_rect(HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width)
	{
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		if (fill_color != CLR_NONE)
		{
			my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(fill_color));
			my::gdi::selector brush_selector(dc, brush.get());

			hive.orig.PatBlt(dc, rc->left, rc->top, w, h, PATCOPY);
		}

		if (edge_color != CLR_NONE && edge_width > 0)
		{
			my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(edge_color));
			my::gdi::selector brush_selector(dc, brush.get());

			hive.orig.PatBlt(dc, rc->left, rc->top, w, edge_width, PATCOPY);
			hive.orig.PatBlt(dc, rc->left, rc->bottom - edge_width, w, edge_width, PATCOPY);
			hive.orig.PatBlt(dc, rc->left, rc->top, edge_width, h, PATCOPY);
			hive.orig.PatBlt(dc, rc->right - edge_width, rc->top, edge_width, h, PATCOPY);
		}
	}

	//
	// 軽量エッジを描画します。
	//
	inline void draw_single_edge(HDC dc, LPCRECT rc, COLORREF near_color, COLORREF far_color)
	{
		const auto edge_width = 1;

		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		{
			my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(near_color));
			my::gdi::selector brush_selector(dc, brush.get());

			hive.orig.PatBlt(dc, rc->left, rc->top, w, edge_width, PATCOPY);
			hive.orig.PatBlt(dc, rc->left, rc->top, edge_width, h, PATCOPY);
		}

		{
			my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(far_color));
			my::gdi::selector brush_selector(dc, brush.get());

			hive.orig.PatBlt(dc, rc->left, rc->bottom - edge_width, w, edge_width, PATCOPY);
			hive.orig.PatBlt(dc, rc->right - edge_width, rc->top, edge_width, h, PATCOPY);
		}
	}

	//
	// エッジを描画します。
	//
	inline void draw_edge(HDC dc, LPCRECT rc,
		COLORREF outer_near_color, COLORREF outer_far_color,
		COLORREF inner_near_color, COLORREF inner_far_color)
	{
		auto rc_temp = *rc;
		draw_single_edge(dc, &rc_temp, outer_near_color, outer_far_color);
		::InflateRect(&rc_temp, -1, -1);
		draw_single_edge(dc, &rc_temp, inner_near_color, inner_far_color);
	}
}
