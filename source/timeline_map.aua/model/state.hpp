#pragma once

namespace apn::timeline_map::model
{
	//
	// このクラスはモデル層のステートです。
	// 主にDirectXのリソース管理を担当します。
	//
	inline struct state_t
	{
		//
		// リセット回数です。
		//
		UINT nb_resets = 0;

		//
		// 描画リソースです。
		//
		dx_resource_t resource;

		//
		// リソースをリセットします。
		//
		void reset()
		{
			// リソースをリセットします。
			resource = {};

			// リセット回数を増やします。
			nb_resets++;
		}

		//
		// 描画処理を開始します。
		//
		void begin_draw(HWND hwnd, render_target_t& render_target)
		{
			resource.d2d_device_context->SetTarget(nullptr);
			resource.d2d_device_context->SetTarget(render_target.d2d_target_bitmap.Get());
			resource.d2d_device_context->SetTransform(D2D1::Matrix3x2F::Identity());
			resource.d2d_device_context->BeginDraw();
			resource.d2d_device_context->Clear({ 0.0f, 0.0f, 0.0f, 1.0f });
		}

		//
		// 描画処理を終了します。
		//
		void end_draw(render_target_t& render_target)
		{
			resource.d2d_device_context->EndDraw();
			resource.d2d_device_context->SetTarget(nullptr);
			auto hr = render_target.dxgi_swap_chain->Present(0, 0);

			// デバイスがリセットされている場合は
			if (hr == DXGI_ERROR_DEVICE_REMOVED ||
				hr == DXGI_ERROR_DEVICE_RESET)
			{
				// リセットします。
				reset();
			}
		}

		//
		// 描画処理の準備をします。
		//
		BOOL prepare()
		{
			// D3Dデバイスが無効の場合は
			if (!resource.d3d_device)
			{
				auto flags = (UINT)D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
				flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
				D3D_FEATURE_LEVEL levels[] = {
					D3D_FEATURE_LEVEL_11_1,
					D3D_FEATURE_LEVEL_11_0,
				};
				D3D_FEATURE_LEVEL result_level = {};

				// D3Dデバイスを作成します。
				auto hr = ::D3D11CreateDevice(
					nullptr,
					D3D_DRIVER_TYPE_HARDWARE,
					nullptr,
					flags,
					levels,
					std::size(levels),
					D3D11_SDK_VERSION,
					&resource.d3d_device,
					&result_level,
					&resource.d3d_device_context);
				if (!resource.d3d_device) return FALSE;
			}

			// DXGIデバイスが無効の場合は
			if (!resource.dxgi_device)
			{
				// DXGIデバイスを取得します。
				auto hr = resource.d3d_device.As(&resource.dxgi_device);
				if (!resource.dxgi_device) return FALSE;
			}

			// D2Dファクトリが無効の場合は
			if (!resource.d2d_factory)
			{
				// D2Dファクトリを作成します。
				auto hr = ::D2D1CreateFactory(
					D2D1_FACTORY_TYPE_SINGLE_THREADED,
//					D2D1_FACTORY_TYPE_MULTI_THREADED,
					IID_PPV_ARGS(&resource.d2d_factory));
				if (!resource.d2d_factory) return FALSE;
			}

			// D2Dデバイスが無効の場合は
			if (!resource.d2d_device)
			{
				// D2Dデバイスを作成します。
				auto hr = resource.d2d_factory->CreateDevice(
					resource.dxgi_device.Get(), &resource.d2d_device);
				if (!resource.d2d_device) return FALSE;
			}

			// D2Dデバイスコンテキストが無効の場合は
			if (!resource.d2d_device_context)
			{
				// D2Dデバイスコンテキストを作成します。
				auto hr = resource.d2d_device->CreateDeviceContext(
					D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
					&resource.d2d_device_context);
				if (!resource.d2d_device_context) return FALSE;
			}

			// DWriteファクトリが無効の場合は
			if (!resource.dw_factory)
			{
				// DWriteファクトリを作成します。
				auto hr = ::DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
					__uuidof(IDWriteFactory), (IUnknown**)&resource.dw_factory);
				if (!resource.dw_factory) return FALSE;
			}

			// リソースを再作成します。
			return recreate_resources();
		}

		//
		// レンダーターゲットのバックバッファを作成します。
		// 内部的に使用されます。
		//
		BOOL create_render_target_back_buffer(render_target_t& render_target)
		{
			// バックバッファを取得します。
			ComPtr<IDXGISurface> dxgi_surface;
			{
				render_target.dxgi_swap_chain->GetBuffer(
					0,
					IID_PPV_ARGS(&dxgi_surface));
				if (!dxgi_surface) return FALSE;
			}

			// D2Dビットマップを作成します。
			{
				auto bitmap_props =
					D2D1::BitmapProperties1(
						D2D1_BITMAP_OPTIONS_TARGET |
						D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
						D2D1::PixelFormat(
							DXGI_FORMAT_B8G8R8A8_UNORM,
							D2D1_ALPHA_MODE_PREMULTIPLIED));

				resource.d2d_device_context->CreateBitmapFromDxgiSurface(
					dxgi_surface.Get(),
					&bitmap_props,
					&render_target.d2d_target_bitmap);
				if (!render_target.d2d_target_bitmap) return FALSE;
			}

			// リセット回数を更新します。
			render_target.nb_resets = nb_resets;

			return TRUE;
		}

		//
		// レンダーターゲットを作成します。
		//
		BOOL create_render_target(HWND hwnd, render_target_t& render_target)
		{
			// DXGIデバイスが無効の場合は何もしません。
			if (!resource.dxgi_device) return FALSE;

			// D2Dデバイスコンテキストが無効の場合は何もしません。
			if (!resource.d2d_device_context) return FALSE;

			// リセット回数が同じで、D2Dビットマップが作成済みの場合は
			if (nb_resets == render_target.nb_resets && render_target.d2d_target_bitmap)
			{
				// レンダーターゲットが既に作成済みなので
				// 何もせずにTRUEを返します。
				return TRUE;
			}

			// レンダーターゲットのリソースをリセットします。
			render_target.dxgi_swap_chain = nullptr;
			render_target.d2d_target_bitmap = nullptr;

			// DXGIファクトリを取得します。
			ComPtr<IDXGIFactory2> dxgi_factory;
			{
				ComPtr<IDXGIAdapter> dxgi_adapter;
				resource.dxgi_device->GetAdapter(&dxgi_adapter);
				if (!dxgi_adapter) return FALSE;

				dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory));
				if (!dxgi_factory) return FALSE;
			}

			// DXGIスワップチェーンを作成します。
			{
				DXGI_SWAP_CHAIN_DESC1 desc = {};
				desc.Width = 0;
				desc.Height = 0;
				desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				desc.BufferCount = 1;
				desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

				dxgi_factory->CreateSwapChainForHwnd(
					resource.d3d_device.Get(),
					hwnd,
					&desc,
					nullptr,
					nullptr,
					&render_target.dxgi_swap_chain);
				if (!render_target.dxgi_swap_chain) return FALSE;
			}

			// バックバッファを作成します。
			return create_render_target_back_buffer(render_target);
		}

		//
		// レンダーターゲットをリサイズします。
		//
		BOOL resize_render_target(HWND hwnd, render_target_t& render_target)
		{
			// DXGIスワップチェーンが無効の場合は何もしません。
			if (!render_target.dxgi_swap_chain) return FALSE;

			// D2Dデバイスコンテキストが無効の場合は何もしません。
			if (!resource.d2d_device_context) return FALSE;

			// D2Dデバイスコンテキストのターゲットをリセットします。
			resource.d2d_device_context->SetTarget(nullptr);

			// 既存のバックバッファを開放します。
			render_target.d2d_target_bitmap = nullptr;

			// DXGIスワップチェーンをリサイズします。
			auto hr = render_target.dxgi_swap_chain->ResizeBuffers(
				0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr)) return FALSE;

			// バックバッファを作成します。
			return create_render_target_back_buffer(render_target);
		}

		//
		// リソースを再作成します。
		//
		BOOL recreate_resources()
		{
			// D2Dデバイスコンテキストが無効の場合は何もしません。
			if (!resource.d2d_device_context) return FALSE;

			{
				// テキスト用のブラシを作成します。
				resource.text.brush = state.create_solid_brush(property.text.color);
				if (!resource.text.brush) return FALSE;

				// テキストシャドウ用のブラシを作成します。
				resource.text.shadow_brush = state.create_solid_brush(property.text.shadow_color);
				if (!resource.text.shadow_brush) return FALSE;
			}

			{
				// 奇数レイヤー用のブラシを作成します。
				resource.layer.odd_brush = state.create_solid_brush(property.layer.odd_color);
				if (!resource.layer.odd_brush) return FALSE;

				// 偶数レイヤー用のブラシを作成します。
				resource.layer.even_brush = state.create_solid_brush(property.layer.even_color);
				if (!resource.layer.even_brush) return FALSE;

				// 無効レイヤー用のブラシを作成します。
				resource.layer.undisp_brush = state.create_solid_brush(property.layer.undisp_color);
				if (!resource.layer.undisp_brush) return FALSE;

				// 無効レイヤー用のストロークブラシを作成します。
				resource.layer.undisp_stroke_brush = state.create_solid_brush(property.layer.undisp_stroke_color);
				if (!resource.layer.undisp_stroke_brush) return FALSE;

				// ロックレイヤー用のブラシを作成します。
				resource.layer.locked_stroke_brush = state.create_solid_brush(property.layer.locked_stroke_color);
				if (!resource.layer.locked_stroke_brush) return FALSE;

				// 座標リンクレイヤー用のブラシを作成します。
				resource.layer.coordlink_stroke_brush = state.create_solid_brush(property.layer.coordlink_stroke_color);
				if (!resource.layer.coordlink_stroke_brush) return FALSE;

				// クリップレイヤー用のブラシを作成します。
				resource.layer.clip_stroke_brush = state.create_solid_brush(property.layer.clip_stroke_color);
				if (!resource.layer.clip_stroke_brush) return FALSE;
			}

			{
				// アイテム枠用のブラシを作成します。
				resource.item.stroke_brush = state.create_solid_brush(property.item.stroke_color);
				if (!resource.item.stroke_brush) return FALSE;

				// アイテム用のグラデーションビットマップを作成します。
				{
					std::vector<uint32_t> gradient(resource.item.c_gradient_w);

					//
					// この関数はグラデーションビットマップを作成して返します。
					//
					const auto create_gradient_bitmap = [&](const auto& node)
					{
						for (auto x = 0; x < resource.item.c_gradient_w; x++)
						{
							auto t = (float)x / (resource.item.c_gradient_w - 1);
							auto s = 1.0f - t;;

							auto r = (uint8_t)(node.start_color.r * s + node.end_color.r * t);
							auto g = (uint8_t)(node.start_color.g * s + node.end_color.g * t);
							auto b = (uint8_t)(node.start_color.b * s + node.end_color.b * t);
							auto a = (uint8_t)(node.start_color.a * s + node.end_color.a * t);

							gradient[x] = (a << 24) | (r << 16) | (g << 8) | (b << 0);
						}

						ComPtr<ID2D1Bitmap> bitmap;
						resource.d2d_device_context->CreateBitmap(
							D2D1::SizeU(resource.item.c_gradient_w, resource.item.c_gradient_h),
							gradient.data(), resource.item.c_gradient_w * 4,
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
						resource.d2d_device_context->CreateBitmapBrush(node.gradient_bitmap.Get(),
							{ D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR },
							&gradient_bitmap_brush);
						return gradient_bitmap_brush;
					};

					for (size_t i = 0; i < std::size(resource.item.nodes); i++)
					{
						resource.item.nodes[i].gradient_bitmap = create_gradient_bitmap(property.item.nodes[i]);
						if (!resource.item.nodes[i].gradient_bitmap) return FALSE;

						resource.item.nodes[i].gradient_brush = create_gradient_bitmap_brush(resource.item.nodes[i]);
						if (!resource.item.nodes[i].gradient_brush) return FALSE;
					}
				}
			}

			{
				// 中間点図形用のブラシを作成します。
				resource.midpt.brush = state.create_solid_brush(property.midpt.color);
				if (!resource.midpt.brush) return FALSE;

				// 中間点位置用のブラシを作成します。
				resource.midpt.line_brush = state.create_solid_brush(property.midpt.line_color);
				if (!resource.midpt.line_brush) return FALSE;
			}

			{
				// 現在フレーム用のブラシを作成します。
				resource.current_frame.brush = state.create_solid_brush(property.current_frame.line_color);
				if (!resource.current_frame.brush) return FALSE;
			}

			{
				// 表示範囲用のブラシを作成します。
				resource.visible_area.brush = state.create_solid_brush(property.visible_area.color);
				if (!resource.visible_area.brush) return FALSE;

				// 表示範囲枠用のブラシを作成します。
				resource.visible_area.stroke_brush = state.create_solid_brush(property.visible_area.stroke_color);
				if (!resource.visible_area.stroke_brush) return FALSE;
			}

			{
				// 制御範囲用のブラシを作成します。
				resource.control_range.brush = state.create_solid_brush(property.control_range.color);
				if (!resource.control_range.brush) return FALSE;

				// 制御範囲枠用のブラシを作成します。
				resource.control_range.stroke_brush = state.create_solid_brush(property.control_range.stroke_color);
				if (!resource.control_range.stroke_brush) return FALSE;
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
			resource.d2d_device_context->CreateSolidColorBrush(d2d_color, &solid_brush);
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
			resource.d2d_device_context->CreateGradientStopCollection(stops, std::size(stops),
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
			resource.d2d_device_context->CreateLinearGradientBrush(
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
			auto hr = resource.dw_factory->CreateTextFormat(
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
			resource.dw_factory->CreateTextLayout(
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
			static constexpr auto src_rc = D2D1_RECT_F { 0.0f, 0.0f, (float)resource.item.c_gradient_w, 1.0f };

			resource.d2d_device_context->DrawBitmap(
				resource.item.nodes[item_node_index].gradient_bitmap.Get(),
				rc, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, src_rc);

			return TRUE;
		}

		//
		// ビットマップを使用してグラデーション矩形を描画します。
		//
		inline BOOL draw_gradient_rectangle(const D2D1_ROUNDED_RECT& rc, size_t item_node_index)
		{
			const auto& brush = resource.item.nodes[item_node_index].gradient_brush;

			brush->SetTransform(D2D1::Matrix3x2F::Identity());
			brush->SetTransform(
				D2D1::Matrix3x2F::Scale((rc.rect.right - rc.rect.left) / resource.item.c_gradient_w, 1.0f) *
				D2D1::Matrix3x2F::Translation(rc.rect.left, rc.rect.top));

			resource.d2d_device_context->FillRoundedRectangle(rc, brush.Get());

			return TRUE;
		}
	} state;
}
