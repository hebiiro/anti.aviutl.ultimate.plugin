#pragma once

namespace apn::timeline_map::main_thread
{
	//
	// このクラスはメインスレッドのステートです。
	// 主にDirectXのリソース管理を担当します。
	//
	inline struct state_t
	{
		ComPtr<ID2D1Factory> d2d_factory;
		ComPtr<IDWriteFactory> dw_factory;
		ComPtr<ID2D1HwndRenderTarget> render_target;

		struct text_t {
			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<ID2D1SolidColorBrush> shadow_brush;
		} text;

		struct layer_t {
			ComPtr<ID2D1SolidColorBrush> odd_brush;
			ComPtr<ID2D1SolidColorBrush> even_brush;
			ComPtr<ID2D1SolidColorBrush> undisp_brush;
			ComPtr<ID2D1SolidColorBrush> undisp_stroke_brush;
			ComPtr<ID2D1SolidColorBrush> locked_stroke_brush;
			ComPtr<ID2D1SolidColorBrush> coordlink_stroke_brush;
			ComPtr<ID2D1SolidColorBrush> clip_stroke_brush;
		} layer;

		struct item_t {
			inline static constexpr auto c_gradient_w = 256;
			inline static constexpr auto c_gradient_h = 1;

			ComPtr<ID2D1SolidColorBrush> stroke_brush;
			struct node_t {
				ComPtr<ID2D1Bitmap> gradient_bitmap;
				ComPtr<ID2D1BitmapBrush> gradient_brush;
			} nodes[property.item.c_item.c_max_size];
		} item;

		struct midpt_t {
			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<ID2D1SolidColorBrush> line_brush;
		} midpt;

		struct current_frame_t {
			ComPtr<ID2D1SolidColorBrush> brush;
		} current_frame;

		struct visible_area_t {
			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<ID2D1SolidColorBrush> stroke_brush;
		} visible_area;

		struct control_range_t {
			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<ID2D1SolidColorBrush> stroke_brush;
		} control_range;

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND hwnd)
		{
			if (!d2d_factory)
			{
				// D2Dファクトリを作成します。
				auto hr = ::D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, IID_PPV_ARGS(&d2d_factory));
				if (!d2d_factory) return FALSE;
			}

			if (!dw_factory)
			{
				// DWriteファクトリを作成します。
				auto hr = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
					__uuidof(IDWriteFactory), (IUnknown**)dw_factory.GetAddressOf());
				if (!dw_factory) return FALSE;
			}

			if (!render_target)
			{
				auto rc = my::get_client_rect(hwnd);
				auto w = my::get_width(rc);
				auto h = my::get_height(rc);

				auto system_dpi = ::GetDpiForSystem();
//				auto window_dpi = ::GetDpiForWindow(hwnd);
//				auto unaware_dpi = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
				auto system_aware_dpi = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
//				auto per_monitor_aware_dpi = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
//				auto pre_monitor_aware_v2_dpi = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
//				auto unaware_gdiscaled_dpi = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED);

				auto dpi = (float)system_dpi / (system_aware_dpi / USER_DEFAULT_SCREEN_DPI);

				// レンダーターゲットを作成します。
				d2d_factory->CreateHwndRenderTarget(
					{ D2D1_RENDER_TARGET_TYPE_DEFAULT, {}, dpi, dpi },
					{ hwnd, { (UINT32)w, (UINT32)h } }, &render_target);
				if (!render_target) return FALSE;

//				render_target->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

				return recreate_resources();
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			render_target = nullptr;
			d2d_factory = nullptr;

			return TRUE;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL recreate_resources()
		{
			if (!render_target) return FALSE;

			{
				// テキスト用のブラシを作成します。
				text.brush = state.create_solid_brush(property.text.color);
				if (!text.brush) return FALSE;

				// テキストシャドウ用のブラシを作成します。
				text.shadow_brush = state.create_solid_brush(property.text.shadow_color);
				if (!text.shadow_brush) return FALSE;
			}

			{
				// 奇数レイヤー用のブラシを作成します。
				layer.odd_brush = state.create_solid_brush(property.layer.odd_color);
				if (!layer.odd_brush) return FALSE;

				// 偶数レイヤー用のブラシを作成します。
				layer.even_brush = state.create_solid_brush(property.layer.even_color);
				if (!layer.even_brush) return FALSE;

				// 無効レイヤー用のブラシを作成します。
				layer.undisp_brush = state.create_solid_brush(property.layer.undisp_color);
				if (!layer.undisp_brush) return FALSE;

				// 無効レイヤー用のストロークブラシを作成します。
				layer.undisp_stroke_brush = state.create_solid_brush(property.layer.undisp_stroke_color);
				if (!layer.undisp_stroke_brush) return FALSE;

				// ロックレイヤー用のブラシを作成します。
				layer.locked_stroke_brush = state.create_solid_brush(property.layer.locked_stroke_color);
				if (!layer.locked_stroke_brush) return FALSE;

				// 座標リンクレイヤー用のブラシを作成します。
				layer.coordlink_stroke_brush = state.create_solid_brush(property.layer.coordlink_stroke_color);
				if (!layer.coordlink_stroke_brush) return FALSE;

				// クリップレイヤー用のブラシを作成します。
				layer.clip_stroke_brush = state.create_solid_brush(property.layer.clip_stroke_color);
				if (!layer.clip_stroke_brush) return FALSE;
			}

			{
				// アイテム枠用のブラシを作成します。
				item.stroke_brush = state.create_solid_brush(property.item.stroke_color);
				if (!item.stroke_brush) return FALSE;

				// アイテム用のグラデーションビットマップを作成します。
				{
					std::vector<uint32_t> gradient(item.c_gradient_w);

					//
					// この関数はグラデーションビットマップを作成して返します。
					//
					const auto create_gradient_bitmap = [&](const auto& node)
					{
						for (auto x = 0; x < item.c_gradient_w; x++)
						{
							auto t = (float)x / (item.c_gradient_w - 1);
							auto s = 1.0f - t;;

							auto r = (uint8_t)(node.start_color.r * s + node.end_color.r * t);
							auto g = (uint8_t)(node.start_color.g * s + node.end_color.g * t);
							auto b = (uint8_t)(node.start_color.b * s + node.end_color.b * t);
							auto a = (uint8_t)(node.start_color.a * s + node.end_color.a * t);

							gradient[x] = (a << 24) | (r << 16) | (g << 8) | (b << 0);
						}

						ComPtr<ID2D1Bitmap> bitmap;
						render_target->CreateBitmap(
							D2D1::SizeU(item.c_gradient_w, item.c_gradient_h),
							gradient.data(), item.c_gradient_w * 4,
							{ { DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE }, 96.0f, 96.0f, },
							&bitmap);
						return bitmap;
					};

					//
					// この関数はグラデーションビットマップブラシを作成して返します。
					//
					const auto create_gradient_bitmap_brush = [&](const auto& node)
					{
						ComPtr<ID2D1BitmapBrush> gradient_bitmap_brush;
						render_target->CreateBitmapBrush(node.gradient_bitmap.Get(),
							{ D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR },
							&gradient_bitmap_brush);
						return gradient_bitmap_brush;
					};

					for (size_t i = 0; i < std::size(item.nodes); i++)
					{
						item.nodes[i].gradient_bitmap = create_gradient_bitmap(property.item.nodes[i]);
						if (!item.nodes[i].gradient_bitmap) return FALSE;

						item.nodes[i].gradient_brush = create_gradient_bitmap_brush(item.nodes[i]);
						if (!item.nodes[i].gradient_brush) return FALSE;
					}
				}
			}

			{
				// 中間点図形用のブラシを作成します。
				midpt.brush = state.create_solid_brush(property.midpt.color);
				if (!midpt.brush) return FALSE;

				// 中間点位置用のブラシを作成します。
				midpt.line_brush = state.create_solid_brush(property.midpt.line_color);
				if (!midpt.line_brush) return FALSE;
			}

			{
				// 現在フレーム用のブラシを作成します。
				current_frame.brush = state.create_solid_brush(property.current_frame.line_color);
				if (!current_frame.brush) return FALSE;
			}

			{
				// 表示範囲用のブラシを作成します。
				visible_area.brush = state.create_solid_brush(property.visible_area.color);
				if (!visible_area.brush) return FALSE;

				// 表示範囲枠用のブラシを作成します。
				visible_area.stroke_brush = state.create_solid_brush(property.visible_area.stroke_color);
				if (!visible_area.stroke_brush) return FALSE;
			}

			{
				// 制御範囲用のブラシを作成します。
				control_range.brush = state.create_solid_brush(property.control_range.color);
				if (!control_range.brush) return FALSE;

				// 制御範囲枠用のブラシを作成します。
				control_range.stroke_brush = state.create_solid_brush(property.control_range.stroke_color);
				if (!control_range.stroke_brush) return FALSE;
			}

			return TRUE;
		}

		//
		// ソリッドブラシを作成して返します。
		//
		inline ComPtr<ID2D1SolidColorBrush> create_solid_brush(const D2D1_COLOR_F& d2d_color)
		{
			// ソリッドブラシを作成します。
			ComPtr<ID2D1SolidColorBrush> solid_brush;
			render_target->CreateSolidColorBrush(d2d_color, &solid_brush);
			return solid_brush;
		}

		//
		// グラデーションストップコレクションを作成して返します。
		//
		inline ComPtr<ID2D1GradientStopCollection> create_gradient_stop_collection(
			const D2D1_COLOR_F& start_color,
			const D2D1_COLOR_F& end_color)
		{
			D2D1_GRADIENT_STOP stops[2] = { { 0.0f, start_color }, { 1.0f, end_color } };

			ComPtr<ID2D1GradientStopCollection> gradient_stop_collection;
			render_target->CreateGradientStopCollection(stops, std::size(stops),
				D2D1_GAMMA_1_0, D2D1_EXTEND_MODE_CLAMP, &gradient_stop_collection);
			return gradient_stop_collection;
		}

		//
		// グラデーションブラシを作成して返します。
		//
		inline ComPtr<ID2D1LinearGradientBrush> create_gradient_brush(
			const D2D1_POINT_2F& start_point,
			const D2D1_POINT_2F& end_point,
			ID2D1GradientStopCollection* gradient_stop_collection)
		{
			D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = { start_point, end_point };

			ComPtr<ID2D1LinearGradientBrush> gradient_brush;
			render_target->CreateLinearGradientBrush(
				props, gradient_stop_collection, &gradient_brush);
			return gradient_brush;
		}

		//
		// グラデーションブラシを作成して返します。
		//
		inline ComPtr<ID2D1LinearGradientBrush> create_gradient_brush(
			const D2D1_COLOR_F& start_color,
			const D2D1_COLOR_F& end_color,
			const D2D1_POINT_2F& start_point,
			const D2D1_POINT_2F& end_point)
		{
			auto gradient_stop_collection = create_gradient_stop_collection(start_color, end_color);
			if (!gradient_stop_collection) return {};

			return create_gradient_brush(start_point, end_point, gradient_stop_collection.Get());
		}

		//
		// テキストフォーマットを作成して返します。
		//
		inline ComPtr<IDWriteTextFormat> create_text_format(const std::wstring& font_name, float font_size, UINT text_flags)
		{
			// テキストフォーマットを作成します。
			ComPtr<IDWriteTextFormat> text_format;
			auto hr = dw_factory->CreateTextFormat(
				font_name.c_str(),
				nullptr,
				DWRITE_FONT_WEIGHT_NORMAL,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				font_size,
				L"",
				&text_format);
			if (!text_format) return {};

			// 横方向のアラインを設定します。
			{
				auto text_alignment = DWRITE_TEXT_ALIGNMENT_LEADING;

				if (text_flags & DT_CENTER) text_alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
				else if (text_flags & DT_RIGHT) text_alignment = DWRITE_TEXT_ALIGNMENT_TRAILING;

				text_format->SetTextAlignment(text_alignment);
			}

			// 縦方向のアラインを設定します。
			{
				auto paragraph_alignment = DWRITE_PARAGRAPH_ALIGNMENT_NEAR;

				if (text_flags & DT_VCENTER) paragraph_alignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
				else if (text_flags & DT_BOTTOM) paragraph_alignment = DWRITE_PARAGRAPH_ALIGNMENT_FAR;

				text_format->SetParagraphAlignment(paragraph_alignment);
			}

			return text_format;
		}

		//
		// テキストレイアウトを作成して返します。
		//
		inline ComPtr<IDWriteTextLayout> create_text_layout(
			const std::wstring& text, UINT text_flags,
			IDWriteTextFormat* text_format, float w, float h)
		{
			// テキストレイアウトを作成します。
			ComPtr<IDWriteTextLayout> text_layout;
			dw_factory->CreateTextLayout(
				text.c_str(), (UINT32)text.length(), text_format, w, h, &text_layout);
			if (!text_layout) return {};

			// テキストを折り返さないように設定します。
			text_layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

			// テキストがクリップされるように設定します。
			text_layout->SetMaxWidth(w);
			text_layout->SetMaxHeight(h);

			return text_layout;
		}

		//
		// ビットマップを使用してグラデーション矩形を描画します。
		//
		inline BOOL draw_gradient_rectangle(const D2D1_RECT_F& rc, size_t item_node_index)
		{
			static constexpr auto src_rc = D2D1_RECT_F { 0.0f, 0.0f, (float)item.c_gradient_w, 1.0f };

			render_target->DrawBitmap(
				item.nodes[item_node_index].gradient_bitmap.Get(),
				rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src_rc);

			return TRUE;
		}

		//
		// ビットマップを使用してグラデーション矩形を描画します。
		//
		inline BOOL draw_gradient_rectangle(const D2D1_ROUNDED_RECT& rc, size_t item_node_index)
		{
			const auto& brush = item.nodes[item_node_index].gradient_brush;

			brush->SetTransform(D2D1::Matrix3x2F::Identity());
			brush->SetTransform(
				D2D1::Matrix3x2F::Scale((rc.rect.right - rc.rect.left) / item.c_gradient_w, 1.0f) *
				D2D1::Matrix3x2F::Translation(rc.rect.left, rc.rect.top));

			render_target->FillRoundedRectangle(rc, brush.Get());

			return TRUE;
		}
	} state;
}
