#pragma once

namespace apn::dark::painter
{
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
	// 指定された透明度で矩形を描画します。
	//
	inline void draw_rect_with_alpha(HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int alpha)
	{
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		auto mem_rc = RECT { 0, 0, w, h };
		my::dc::unique_ptr<> mem_dc(::CreateCompatibleDC(dc));
		my::gdi::unique_ptr<HBITMAP> bitmap(::CreateCompatibleBitmap(dc, w, h));
		my::gdi::selector bitmap_selector(mem_dc.get(), bitmap.get());

		draw_rect(mem_dc.get(), &mem_rc, fill_color, edge_color, edge_width);

		BLENDFUNCTION bf = {};
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = alpha;
		bf.AlphaFormat = 0;//AC_SRC_ALPHA;

		::AlphaBlend(dc, rc->left, rc->top, w, h, mem_dc.get(), 0, 0, w, h, bf);
	}

	//
	// 丸角矩形を描画します。
	//
	inline void draw_round_rect(HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int round_width, int round_height)
	{
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		my::gdi::unique_ptr<HBRUSH> brush;
		my::gdi::selector brush_selector(dc, (fill_color != CLR_NONE) ?
			brush.reset(::CreateSolidBrush(fill_color)), brush.get() : (HBRUSH)::GetStockObject(NULL_BRUSH));

		my::gdi::unique_ptr<HPEN> pen;
		my::gdi::selector pen_selector(dc, (edge_color != CLR_NONE && edge_width > 0) ?
				pen.reset(::CreatePen(PS_INSIDEFRAME, edge_width, edge_color)), pen.get() : (HPEN)::GetStockObject(NULL_PEN));

		::RoundRect(dc, rc->left, rc->top, rc->right, rc->bottom, round_width, round_height);
	}

	//
	// 指定された透明度で丸角矩形を描画します。
	//
	inline void draw_round_rect_with_alpha(HDC dc, LPCRECT rc, COLORREF fill_color, COLORREF edge_color, int edge_width, int round_width, int round_height, int alpha)
	{
		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		auto mem_rc = RECT { 0, 0, w, h };
		my::dc::unique_ptr<> mem_dc(::CreateCompatibleDC(dc));
		my::gdi::unique_ptr<HBITMAP> bitmap(::CreateCompatibleBitmap(dc, w, h));
		my::gdi::selector bitmap_selector(mem_dc.get(), bitmap.get());

		::BitBlt(mem_dc.get(), 0, 0, w, h, dc, rc->left, rc->top, SRCCOPY);

		draw_round_rect(mem_dc.get(), &mem_rc, fill_color, edge_color, edge_width, round_width, round_height);

		BLENDFUNCTION bf = {};
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = alpha;
		bf.AlphaFormat = 0;//AC_SRC_ALPHA;

		::AlphaBlend(dc, rc->left, rc->top, w, h, mem_dc.get(), 0, 0, w, h, bf);
	}

	//
	// 軽量エッジを描画します。
	//
	inline void draw_light_edge(HDC dc, LPCRECT rc, COLORREF top_left_color, COLORREF bottom_right_color)
	{
		const auto edge_width = 1;

		auto w = my::get_width(*rc);
		auto h = my::get_height(*rc);

		{
			my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(top_left_color));
			my::gdi::selector brush_selector(dc, brush.get());

			hive.orig.PatBlt(dc, rc->left, rc->top, w, edge_width, PATCOPY);
			hive.orig.PatBlt(dc, rc->left, rc->top, edge_width, h, PATCOPY);
		}

		{
			my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(bottom_right_color));
			my::gdi::selector brush_selector(dc, brush.get());

			hive.orig.PatBlt(dc, rc->left, rc->bottom - edge_width, w, edge_width, PATCOPY);
			hive.orig.PatBlt(dc, rc->right - edge_width, rc->top, edge_width, h, PATCOPY);
		}
	}

	//
	// エッジを描画します。
	//
	inline void draw_edge(HDC dc, LPCRECT rc, COLORREF inner_top_left_color, COLORREF inner_bottom_right_color, COLORREF outer_top_left_color, COLORREF outer_bottom_right_color)
	{
		auto rc_temp = *rc;
		draw_light_edge(dc, &rc_temp, outer_top_left_color, outer_bottom_right_color);
		::InflateRect(&rc_temp, -1, -1);
		draw_light_edge(dc, &rc_temp, inner_top_left_color, inner_bottom_right_color);
	}

	//
	// フォーカスを表す矩形を描画します。
	//
	inline void draw_focus_rect(HDC dc, LPCRECT rc, COLORREF line_color, UINT line_style)
	{
		auto rc_temp = *rc;
		rc_temp.right -= 1; rc_temp.bottom -= 1;

		LOGBRUSH lb = {};
		lb.lbStyle = BS_SOLID;
		lb.lbColor = line_color;
		lb.lbHatch = 0;
		DWORD pen_style[2] = { 0, 2 };

		my::gdi::unique_ptr<HPEN> pen(::ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE, 1, &lb, std::size(pen_style), pen_style));
		my::gdi::selector pen_selector(dc, pen.get());

		::MoveToEx(dc, rc_temp.left, rc_temp.top, 0);
		::LineTo(dc, rc_temp.right, rc_temp.top);
		::LineTo(dc, rc_temp.right, rc_temp.bottom);
		::LineTo(dc, rc_temp.left, rc_temp.bottom);
		::LineTo(dc, rc_temp.left, rc_temp.top);
	}

	//
	// 指定された色で文字列を描画します。
	//
	inline void draw_text(HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, COLORREF fill_color, COLORREF text_color)
	{
		auto old_bk_mode = ::GetBkMode(dc);
		auto old_bk_color = ::GetBkColor(dc);
		auto old_text_color = ::GetTextColor(dc);

		if (!opaque)
			::SetBkMode(dc, TRANSPARENT);

		if (fill_color != CLR_NONE)
			::SetBkColor(dc, fill_color);

		if (text_color != CLR_NONE)
			::SetTextColor(dc, text_color);

		::DrawTextW(dc, text, length, (LPRECT)rc, format);

		::SetBkMode(dc, old_bk_mode);
		::SetBkColor(dc, old_bk_color);
		::SetTextColor(dc, old_text_color);
	}

	//
	// 指定された色で文字列を描画します。
	//
	inline void text_out(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx, COLORREF fill_color, COLORREF text_color)
	{
		auto old_bk_mode = ::GetBkMode(dc);
		auto old_bk_color = ::GetBkColor(dc);
		auto old_text_color = ::GetTextColor(dc);

		if (options & ETO_OPAQUE)
		{
			if (fill_color != CLR_NONE)
				::SetBkColor(dc, fill_color);
		}
		else
		{
			::SetBkMode(dc, TRANSPARENT);
		}

		if (text_color != CLR_NONE)
			::SetTextColor(dc, text_color);

		hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);

		::SetBkMode(dc, old_bk_mode);
		::SetBkColor(dc, old_bk_color);
		::SetTextColor(dc, old_text_color);
	}

	//
	// 指定された色で影付き文字列を描画します。
	//
	inline void draw_text_with_shadow(HDC dc, LPCWSTR text, int length, LPCRECT rc, UINT format, BOOL opaque, COLORREF fill_color, COLORREF text_color, COLORREF text_shadow_color)
	{
		if (!hive.draw_shadow)
			return draw_text(dc, text, length, rc, format, opaque, fill_color, text_color);

		auto old_bk_mode = ::SetBkMode(dc, TRANSPARENT);
		auto old_text_color = ::GetTextColor(dc);

		if (opaque)
		{
			auto old_bk_color = ::SetBkColor(dc,
				(fill_color != CLR_NONE) ? fill_color : ::GetBkColor(dc));
			hive.orig.ExtTextOutW(dc, 0, 0, ETO_OPAQUE, rc, 0, 0, 0);
			::SetBkColor(dc, old_bk_color);
		}

		if (text_shadow_color != CLR_NONE)
		{
			auto rc_shadow = RECT {};
			if (rc) rc_shadow = *rc;
			::OffsetRect(&rc_shadow, +1, +1);
			::SetTextColor(dc, text_shadow_color);
			::DrawTextW(dc, text, length, &rc_shadow, format);
		}

		::SetTextColor(dc, text_color);
		::DrawTextW(dc, text, length, (LPRECT)rc, format);

		::SetBkMode(dc, old_bk_mode);
		::SetTextColor(dc, old_text_color);
	}

	//
	// 指定された色で影付き文字列を描画します。
	//
	inline void text_out_with_shadow(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx, COLORREF fill_color, COLORREF text_color, COLORREF text_shadow_color)
	{
		if (!hive.draw_shadow)
			return text_out(dc, x, y, options, rc, text, c, dx, fill_color, text_color);

		auto old_bk_mode = ::GetBkMode(dc);
		auto old_bk_color = ::GetBkColor(dc);
		auto old_text_color = ::GetTextColor(dc);

		if (options & ETO_OPAQUE)
		{
			// 背景を塗りつぶします。

			if (fill_color != CLR_NONE)
				::SetBkColor(dc, fill_color);

			hive.orig.ExtTextOutW(dc, x, y, options, rc, 0, 0, dx);
		}

		if (text && c)
		{
			// 文字列を描画します。

			::SetBkMode(dc, TRANSPARENT);

			if (text_shadow_color != CLR_NONE)
			{
				auto align = ::GetTextAlign(dc);
				::SetTextAlign(dc, align & ~TA_UPDATECP);
				::SetTextColor(dc, text_shadow_color);

				x += 1, y += 1;
				hive.orig.ExtTextOutW(dc, x, y, options & ~ETO_OPAQUE, rc, text, c, dx);
				x -= 1, y -= 1;

				::SetTextAlign(dc, align);
			}

			::SetTextColor(dc, text_color);

			hive.orig.ExtTextOutW(dc, x, y, options & ~ETO_OPAQUE, rc, text, c, dx);
		}

		::SetBkMode(dc, old_bk_mode);
		::SetBkColor(dc, old_bk_color);
		::SetTextColor(dc, old_text_color);
	}

	namespace gdiplus
	{
		//
		// このクラスは描画関数の引数です。
		//
		using PaintArgs = Dark::Stuff;

		//
		// このクラスは自分用にカスタム設定したグラフィックスです。
		//
		struct MyGraphics : Graphics
		{
			MyGraphics(HDC dc)
				: Graphics(dc)
			{
				SetSmoothingMode(SmoothingModeAntiAlias);
//				SetCompositingMode(CompositingModeSourceOver);
//				SetTextRenderingHint(TextRenderingHintAntiAlias);
//				SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
//				SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
//				SetInterpolationMode(InterpolationModeHighQualityBicubic);
//				TranslateTransform(-0.5f, -0.5f);
			}
		};

		//
		// 与えられた引数からペンを作成して返します。
		//
		inline std::shared_ptr<Pen> create_pen(Graphics& graphics, const RectF& rc, const PaintArgs& args, GraphicsPath* path)
		{
			auto pen = std::make_shared<Pen>(MyColor(args.etc.alpha, args.border.color), args.border.width);
			pen->SetAlignment(PenAlignmentInset);
//			pen->SetLineJoin(LineJoinBevel);
			return pen;
		}

		//
		// 与えられた引数からブラシを作成して返します。
		//
		inline std::shared_ptr<Brush> create_brush(Graphics& graphics, const RectF& rc, const PaintArgs& args, GraphicsPath* path)
		{
			switch (args.fill.mode)
			{
			default:
			case args.fill.c_mode.c_normal:
				{
					return std::make_shared<SolidBrush>(MyColor(args.etc.alpha, args.fill.color));
				}
			case args.fill.c_mode.c_left:
				{
					auto brush = std::make_shared<LinearGradientBrush>(
						PointF(rc.X, rc.Y),
						PointF(rc.X + rc.Width, rc.Y),
						MyColor(args.etc.alpha, args.fill.color),
						MyColor(args.etc.alpha, args.fill.sub_color));
					brush->SetGammaCorrection(TRUE);
					return brush;
				}
			case args.fill.c_mode.c_top:
				{
					auto brush = std::make_shared<LinearGradientBrush>(
						PointF(rc.X, rc.Y),
						PointF(rc.X, rc.Y + rc.Height),
						MyColor(args.etc.alpha, args.fill.color),
						MyColor(args.etc.alpha, args.fill.sub_color));
					brush->SetGammaCorrection(TRUE);
					return brush;
				}
			case args.fill.c_mode.c_right:
				{
					auto brush = std::make_shared<LinearGradientBrush>(
						PointF(rc.X, rc.Y),
						PointF(rc.X + rc.Width, rc.Y),
						MyColor(args.etc.alpha, args.fill.sub_color),
						MyColor(args.etc.alpha, args.fill.color));
					brush->SetGammaCorrection(TRUE);
					return brush;
				}
			case args.fill.c_mode.c_bottom:
				{
					auto brush = std::make_shared<LinearGradientBrush>(
						PointF(rc.X, rc.Y),
						PointF(rc.X, rc.Y + rc.Height),
						MyColor(args.etc.alpha, args.fill.sub_color),
						MyColor(args.etc.alpha, args.fill.color));
					brush->SetGammaCorrection(TRUE);
					return brush;
				}
			case args.fill.c_mode.c_top_left:
				{
					auto brush = std::make_shared<LinearGradientBrush>(
						PointF(rc.X, rc.Y),
						PointF(rc.X + rc.Width, rc.Y + rc.Height),
						MyColor(args.etc.alpha, args.fill.color),
						MyColor(args.etc.alpha, args.fill.sub_color));
					brush->SetGammaCorrection(TRUE);
					return brush;
				}
			case args.fill.c_mode.c_top_right:
				{
					auto brush = std::make_shared<LinearGradientBrush>(
						PointF(rc.X + rc.Width, rc.Y),
						PointF(rc.X, rc.Y + rc.Height),
						MyColor(args.etc.alpha, args.fill.color),
						MyColor(args.etc.alpha, args.fill.sub_color));
					brush->SetGammaCorrection(TRUE);
					return brush;
				}
			case args.fill.c_mode.c_bottom_left:
				{
					auto brush = std::make_shared<LinearGradientBrush>(
						PointF(rc.X + rc.Width, rc.Y),
						PointF(rc.X, rc.Y + rc.Height),
						MyColor(args.etc.alpha, args.fill.sub_color),
						MyColor(args.etc.alpha, args.fill.color));
					brush->SetGammaCorrection(TRUE);
					return brush;
				}
			case args.fill.c_mode.c_bottom_right:
				{
					auto brush = std::make_shared<LinearGradientBrush>(
						PointF(rc.X, rc.Y),
						PointF(rc.X + rc.Width, rc.Y + rc.Height),
						MyColor(args.etc.alpha, args.fill.sub_color),
						MyColor(args.etc.alpha, args.fill.color));
					brush->SetGammaCorrection(TRUE);
					return brush;
				}
			case args.fill.c_mode.c_center:
				{
					std::shared_ptr<PathGradientBrush> brush;
					if (path)
					{
						brush = std::make_shared<PathGradientBrush>(path);
					}
					else
					{
						GraphicsPath rect_path;
						rect_path.AddRectangle(rc);
						brush = std::make_shared<PathGradientBrush>(&rect_path);
					}
					brush->SetGammaCorrection(TRUE);
					brush->SetCenterColor(MyColor(args.etc.alpha, args.fill.color));
					Color colors[] = { MyColor(args.etc.alpha, args.fill.sub_color) };
					auto count = (int)std::size(colors);
					brush->SetSurroundColors(colors, &count);
//					brush->SetFocusScales(0.3f, 0.3f);
					return brush;
				}
			}
		}

		//
		// 指定された設定で矩形を描画します。
		//
		inline void draw_rect(Graphics& graphics, const RectF& rc, const PaintArgs& args)
		{
			if (args.fill.is_enabled())
			{
				auto brush = create_brush(graphics, rc, args, nullptr);
				graphics.FillRectangle(brush.get(), rc);
			}

			if (args.border.is_enabled())
			{
				auto pen = create_pen(graphics, rc, args, nullptr);
				graphics.DrawRectangle(pen.get(), rc);
			}
		}

		//
		// 指定された設定でパスの内部を塗りつぶします。
		//
		inline void fill_path(Graphics& graphics, GraphicsPath& path, const RectF& rc, const PaintArgs& args)
		{
			auto brush = create_brush(graphics, rc, args, &path);
			graphics.FillPath(brush.get(), &path);
		}

		//
		// 指定された設定でパスの縁を描画します。
		//
		inline void frame_path(Graphics& graphics, GraphicsPath& path, const RectF& rc, const PaintArgs& args)
		{
			auto pen = create_pen(graphics, rc, args, &path);
			graphics.DrawPath(pen.get(), &path);
		}

		//
		// 角丸矩形を描画します。
		//
		template <typename T>
		inline void draw_round_figure(HDC dc, LPCRECT rc, const PaintArgs& args, T create_path)
		{
			auto ellipse_enabled = hive.as_round;
			auto fix_enabled = hive.fix_dpi_scaling;

			MyGraphics graphics(dc);
			auto rc_base = *rc;

			// GDI+のバグ対策用の処理です。
			if (fix_enabled)
			{
				auto window_org = POINT {};
				::GetWindowOrgEx(dc, &window_org);
				::OffsetRect(&rc_base, -window_org.x, -window_org.y);
			}

			if (ellipse_enabled)
			{
				auto fix = 0.0f;
//				auto fix = fix_enabled ? 0.5f : 0.0f;

				MyRectF rc(rc_base);

				// このままだと少しはみ出してしまうので、矩形を少し縮めます。
				rc.Width -= 1.0f;
				rc.Height -= 1.0f;

				auto left = rc.X;
				auto top = rc.Y;
				auto right = rc.X + rc.Width;
				auto bottom = rc.Y + rc.Height;

				GraphicsPath path;
				create_path(path, rc, left, top, right, bottom, fix);

				if (args.fill.is_enabled())
				{
					fill_path(graphics, path, rc, args);
				}

				if (args.border.is_enabled())
				{
					frame_path(graphics, path, rc, args);
				}
			}
			else
			{
				draw_rect(graphics, MyRectF(rc_base), args);
			}
		}

		inline void draw_round_left(HDC dc, LPCRECT rc, const PaintArgs& args)
		{
			draw_round_figure(dc, rc, args,
				[](GraphicsPath& path, const RectF& rc, REAL left, REAL top, REAL right, REAL bottom, REAL fix)
				{
					auto diameter = rc.Height;
					path.AddArc(left, top, diameter, diameter + fix, 90.0f, 180.0f); // 左の円弧を下から上に描画します。
					path.AddLine(right, top, right, bottom); // 右辺を上から下に描画します。
					path.AddLine(right, bottom, left + diameter / 2, bottom); // 下辺を右から左に描画します。
				}
			);
		}

		inline void draw_round_right(HDC dc, LPCRECT rc, const PaintArgs& args)
		{
			draw_round_figure(dc, rc, args,
				[](GraphicsPath& path, const RectF& rc, REAL left, REAL top, REAL right, REAL bottom, REAL fix)
				{
					auto diameter = rc.Height;
					path.AddArc(right - diameter, top, diameter, diameter + fix, 270.0f, 180.0f); // 右の円弧を上から下に描画します。
					path.AddLine(left, bottom, left, top); // 左辺を下から上に描画します。
					path.AddLine(left, top, right - diameter / 2, top); // 上辺を左から右に描画します。
				}
			);
		}

		inline void draw_round_top(HDC dc, LPCRECT rc, const PaintArgs& args)
		{
			draw_round_figure(dc, rc, args,
				[](GraphicsPath& path, const RectF& rc, REAL left, REAL top, REAL right, REAL bottom, REAL fix)
				{
					auto diameter = rc.Width;
					path.AddArc(left, top, diameter + fix, diameter, 180.0f, 180.0f); // 上の円弧を左から右に描画します。
					path.AddLine(right, bottom, left, bottom); // 下辺を右から左に描画します。
					path.AddLine(left, bottom, left, top + diameter / 2); // 左辺を下から上に描画します。
				}
			);
		}

		inline void draw_round_bottom(HDC dc, LPCRECT rc, const PaintArgs& args)
		{
			draw_round_figure(dc, rc, args,
				[](GraphicsPath& path, const RectF& rc, REAL left, REAL top, REAL right, REAL bottom, REAL fix)
				{
					auto diameter = rc.Width;
					path.AddArc(left, bottom - diameter, diameter + fix, diameter, 0.0f, 180.0f); // 下の円弧を右から左に描画します。
					path.AddLine(left, top, right, top); // 上辺を左から右に描画します。
					path.AddLine(right, top, right, bottom - diameter / 2); // 右辺を上から下に描画します。
				}
			);
		}

		inline void draw_round_horz(HDC dc, LPCRECT rc, const PaintArgs& args)
		{
			draw_round_figure(dc, rc, args,
				[](GraphicsPath& path, const RectF& rc, REAL left, REAL top, REAL right, REAL bottom, REAL fix)
				{
					auto diameter = rc.Height;
					path.AddArc(left, top, diameter, diameter, 90.0f, 180.0f); // 左の円弧を下から上に描画します。
					path.AddArc(right - diameter, top, diameter, diameter, 270.0f, 180.0f); // 右の円弧を上から下に描画します。
					path.AddLine(right - diameter / 2, bottom, left + diameter / 2, bottom); // 下辺を描画します。
				}
			);
		}

		inline void draw_round_vert(HDC dc, LPCRECT rc, const PaintArgs& args)
		{
			draw_round_figure(dc, rc, args,
				[](GraphicsPath& path, const RectF& rc, REAL left, REAL top, REAL right, REAL bottom, REAL fix)
				{
					auto diameter = rc.Width;
					path.AddArc(left, top, diameter, diameter, 180.0f, 180.0f); // 上の円弧を左から右に描画します。
					path.AddArc(left, bottom - diameter, diameter, diameter, 0.0f, 180.0f); // 下の円弧を右から左に描画します。
					path.AddLine(left, bottom - diameter / 2, left, top + diameter / 2); // 左辺を描画します。
				}
			);
		}

		inline void draw_round_all(HDC dc, LPCRECT rc, const PaintArgs& args)
		{
			draw_round_figure(dc, rc, args,
				[args](GraphicsPath& path, const RectF& rc, REAL left, REAL top, REAL right, REAL bottom, REAL fix)
				{
					auto dx = std::min(args.etc.ellipse, rc.Width);
					auto dy = std::min(args.etc.ellipse, rc.Height);
					if (dx != 0 && dy != 0)
					{
						path.AddArc(left, top, dx, dy, 180.0f, 90.0f); // 左上の円弧を左から上に描画します。
						path.AddArc(right - dx, top, dx, dy, 270.0f, 90.0f); // 右上の円弧を上から右に描画します。
						path.AddArc(right - dx, bottom - dy, dx, dy, 0.0f, 90.0f); // 右下の円弧を右から下に描画します。
						path.AddArc(left, bottom - dy, dx, dy, 90.0f, 90.0f); // 左下の円弧を下から左に描画します。
						path.AddLine(left, bottom - dy / 2, left, top + dy / 2); // 左辺を描画します。
					}
					else
					{
						path.AddRectangle(rc);
					}
				}
			);
		}

		inline void draw_ellipse(HDC dc, LPCRECT rc, const PaintArgs& args)
		{
			draw_round_figure(dc, rc, args,
				[](GraphicsPath& path, const RectF& rc, REAL left, REAL top, REAL right, REAL bottom, REAL fix)
				{
					path.AddEllipse(left, top, rc.Width, rc.Height);
				}
			);
		}
	}
}
