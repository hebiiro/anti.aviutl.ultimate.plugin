#pragma once

namespace apn::dark::kuro::paint::d2d
{
	//
	// このクラスはD2Dを使用した文字列の描画を担当します。
	//
	struct texter_t
	{
		//
		// 影の大きさを実数で返します。
		//
		inline static float get_text_shadow_size()
		{
			return hive.shadow.size / 10.0f;
		}

		//
		// 影の位置をD2D形式で返します。
		//
		inline static D2D1_POINT_2F get_text_shadow_offset()
		{
			return {
				hive.shadow.offset.x / 10.0f,
				hive.shadow.offset.y / 10.0f,
			};
		}

		//
		// プレフィックスを処理します。
		// 内部的に使用されます。
		//
		inline static std::wstring process_prefix(const std::wstring& str, std::vector<DWRITE_TEXT_RANGE>& underlines)
		{
			// 出力する文字列です。
			std::wstring out;

			// プレフィックス付きフラグです。
			bool prefixed = false;

			// すべての文字を走査します。
			for (auto ch : str)
			{
				// プレフィックス付きの場合は
				if (prefixed)
				{
					// 文字を出力に追加します。
					out.push_back(ch);

					// 文字がプレフィックスではない場合は
					if (ch != L'&')
					{
						// 最後に追加した文字をアンダーライン対象にします。
						underlines.emplace_back((UINT32)out.length() - 1, 1);
					}

					// プレフィックス付きフラグをリセットします。
					prefixed = false;
				}
				// 文字がプレフィックスの場合は
				else if (ch == L'&')
				{
					// プレフィックス付きフラグをセットします。
					prefixed = true;
				}
				// それ以外の場合は
				else
				{
					// 文字を出力に追加します。
					out.push_back(ch);
				}
			}

			// プレフィックス付きの場合は
			if (prefixed)
			{
				// 最後のプレフィックスはそのまま出力します。
				out.push_back(L'&');
			}

			return out;
		}

		//
		// テキストフォーマットを作成して返します。
		//
		inline static ComPtr<IDWriteTextFormat> create_text_format(HDC dc, UINT text_flags)
		{
			// フォントを取得します。
			auto font = (HFONT)::GetCurrentObject(dc, OBJ_FONT);

			// 論理フォントを取得します。
			LOGFONTW lf = {}; ::GetObjectW(font, sizeof(lf), &lf);

			// フォントの寸法を取得します。
			TEXTMETRICW tm = {};
			::GetTextMetricsW(dc, &tm);

			// フォントサイズを取得します。
			auto font_size = (float)(tm.tmHeight - tm.tmInternalLeading) - 0.5f;

			// テキストフォーマットを作成します。
			ComPtr<IDWriteTextFormat> text_format;
			core.dw_factory->CreateTextFormat(
				lf.lfFaceName,
				nullptr,
				(lf.lfWeight >= FW_BOLD) ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
				(lf.lfItalic) ? DWRITE_FONT_STYLE_ITALIC : DWRITE_FONT_STYLE_NORMAL,
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
		inline static ComPtr<IDWriteTextLayout> create_text_layout(
			std::wstring& s, HDC dc, UINT text_flags,
			IDWriteTextFormat* text_format, float w, float h)
		{
			// テキストからアンダーライン対象を取得します。
			auto underlines = std::vector<DWRITE_TEXT_RANGE> {};
			if (!(text_flags & DT_NOPREFIX)) s = process_prefix(s, underlines);

			// テキストレイアウトを作成します。
			ComPtr<IDWriteTextLayout> text_layout;
			core.dw_factory->CreateTextLayout(
				s.c_str(), (UINT32)s.length(), text_format, w, h, &text_layout);
			if (!text_layout) return {};

			// テキストを折り返さないように設定します。
			text_layout->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

			// テキストがクリップされるように設定します。
			text_layout->SetMaxWidth(w);
			text_layout->SetMaxHeight(h);

			// テキストにアンダーラインを適用します。
			for (const auto& range : underlines)
				text_layout->SetUnderline(TRUE, range);

			return text_layout;
		}

		//
		// ソリッドブラシを作成して返します。
		//
		inline static ComPtr<ID2D1SolidColorBrush> create_brush(
			ID2D1RenderTarget* render_target, const D2D1_COLOR_F& d2d_color)
		{
			// ブラシを作成します。
			ComPtr<ID2D1SolidColorBrush> brush;
			render_target->CreateSolidColorBrush(d2d_color, &brush);
			return brush;
		}

		//
		// テキスト用のブラシを作成して返します。
		//
		inline static ComPtr<ID2D1SolidColorBrush> create_text_brush(
			ID2D1RenderTarget* render_target, const pigment_t* pigment)
		{
			// テキスト用のカラーエントリを取得します。
			const auto& color_entry = pigment->text.color_entry;

			// テキストの色をD2D形式で取得します。
			auto d2d_text_color = to_d2d_color(color_entry.parts[0]);

			// ブラシを作成して返します。
			return create_brush(render_target, d2d_text_color);
		}

		//
		// 影用のブラシを作成して返します。
		//
		inline static ComPtr<ID2D1SolidColorBrush> create_text_shadow_brush(
			ID2D1RenderTarget* render_target, const pigment_t* pigment)
		{
			// 影用のカラーエントリを取得します。
			const auto& color_entry = pigment->text_shadow.color_entry;

			// 影の色をD2D形式で取得します。
			auto d2d_text_shadow_color = to_d2d_color(color_entry.parts[0]);

			// ブラシを作成して返します。
			return create_brush(render_target, d2d_text_shadow_color);
		}

		//
		// 描画に使用するプロパティです。
		//
		HDC dc = {};
		LPCRECT rc = {};
		UINT text_flags = {};
		const pigment_t* pigment = {};
		int iw = {}, ih = {};
		float w = {}, h = {};
		std::wstring s = {};

		//
		// コンストラクタです。
		//
		texter_t(HDC dc, LPCWSTR text, int c, LPCRECT rc, UINT text_flags, const pigment_t* pigment)
			: dc(dc), rc(rc), text_flags(text_flags), pigment(pigment)
		{
			// 引数が無効の場合は失敗します。
			if (!text || !rc || ::IsRectEmpty(rc)) return;

			// ピグメントが無効の場合は失敗します。
			if (!pigment->text.is_valid()) return;

			// 描画の準備に失敗した場合は失敗します。
			if (!core.prepare()) return;

			// 描画寸法を取得します。
			iw = my::get_width(*rc);
			ih = my::get_height(*rc);
			w = (float)iw;
			h = (float)ih;

			// 文字列を取得します。
			s = (c < 0) ? std::wstring(text) : std::wstring(text, c);
		}

		//
		// 初期化が正常に完了している場合はTRUEを返します。
		//
		BOOL is_initialized() const
		{
			return iw > 0 && ih > 0;
		}

		//
		// ぼかしありの文字列を描画します。
		//
		int draw_text_with_blur()
		{
			// エラーコードです。
			auto hr = HRESULT {};

			// WICビットマップを作成します。
			ComPtr<IWICBitmap> wic_bitmap;
			hr = core.wic_factory->CreateBitmap(iw, ih,
				GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnLoad, &wic_bitmap);
			if (!wic_bitmap) return 0;

			// WICビットマップのレンダーターゲットを作成します。
			ComPtr<ID2D1RenderTarget> wic_render_target;
			core.d2d_factory->CreateWicBitmapRenderTarget(
				wic_bitmap.Get(), core.c_render_target_props, &wic_render_target);

			// D2Dデバイスコンテキストを取得します。
			ComPtr<ID2D1DeviceContext> wic_device_context;
			hr = wic_render_target.As(&wic_device_context);
			if (!wic_device_context) return 0;

			// テキストフォーマットを作成します。
			auto text_format = create_text_format(dc, text_flags);
			if (!text_format) return 0;

			// テキストレイアウトを作成します。
			auto text_layout = create_text_layout(s, dc, text_flags, text_format.Get(), w, h);
			if (!text_layout) return 0;

			// テキスト用のブラシを作成します。
			auto text_brush = create_text_brush(wic_device_context.Get(), pigment);
			if (!text_brush) return 0;

			// 影用のブラシを取得します。
			auto text_shadow_brush = create_text_shadow_brush(wic_device_context.Get(), pigment);
			if (!text_shadow_brush) return 0;

			// オフスクリーンビットマップを作成します。
			auto offscreen_props = D2D1::BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET,
//				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				core.c_pixel_format);
			ComPtr<ID2D1Bitmap1> offscreen;
			hr = wic_device_context->CreateBitmap(
				D2D1::SizeU(iw, ih), nullptr, 0, &offscreen_props, &offscreen);
			if (!offscreen) return 0;

			// オフスクリーンビットマップに描画します。
			{
				ComPtr<ID2D1Image> old_target;
				wic_device_context->GetTarget(&old_target);
				wic_device_context->SetTarget(offscreen.Get());
				wic_device_context->BeginDraw();
				{
					wic_device_context->Clear();

					// 影を描画します。
					wic_device_context->DrawTextLayout(
						get_text_shadow_offset(),
						text_layout.Get(),
						text_shadow_brush.Get());
				}

				hr = wic_device_context->EndDraw();
				wic_device_context->SetTarget(old_target.Get());
			}

			// オフスクリーンビットマップにぼかしをかけて
			// WICビットマップに描画します。
			{
				wic_device_context->BeginDraw();

				wic_device_context->Clear();

				// ガウスぼかしエフェクトを作成します。
				ComPtr<ID2D1Effect> blur;
				hr = wic_device_context->CreateEffect(CLSID_D2D1GaussianBlur, &blur);
				if (!blur) return 0;

				// ぼかした影を描画します。
				blur->SetInput(0, offscreen.Get());
				blur->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, get_text_shadow_size());
//				blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED);
//				blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED);
				blur->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY);
				blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_SOFT);
//				blur->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
				wic_device_context->DrawImage(blur.Get());

				// 影の上にテキストを描画します。
				wic_device_context->DrawTextLayout(
					D2D1::Point2F(0.0f, 0.0f),
					text_layout.Get(),
					text_brush.Get(),
					D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

				hr = wic_device_context->EndDraw();
			}

			// WICビットマップへの描画が完了したので、
			// WICビットマップからビットマップを作成します。
			ComPtr<ID2D1Bitmap> bitmap;
			hr = core.render_target->CreateBitmapFromWicBitmap(wic_bitmap.Get(), &bitmap);

			// ビットマップを描画します。
			core.render_target->DrawBitmap(bitmap.Get());

			// テキストの高さを返します。
			auto text_metrics = DWRITE_TEXT_METRICS {};
			text_layout->GetMetrics(&text_metrics);
			return (int)text_metrics.height;
		}

		//
		// ぼかしなしの文字列を描画します。
		//
		int draw_text_without_blur()
		{
			// テキストフォーマットを作成します。
			auto text_format = create_text_format(dc, text_flags);
			if (!text_format) return 0;

			// テキストレイアウトを作成します。
			auto text_layout = create_text_layout(s, dc, text_flags, text_format.Get(), w, h);
			if (!text_layout) return 0;

			// テキスト用のブラシを作成します。
			auto text_brush = create_text_brush(core.render_target.Get(), pigment);
			if (!text_brush) return 0;

			// 影を描画する場合は
			if (hive.shadow.flag_use)
			{
				// 影用のブラシを取得します。
				auto text_shadow_brush = create_text_shadow_brush(core.render_target.Get(), pigment);
				if (!text_shadow_brush) return 0;

				// 影を描画します。
				core.render_target->DrawTextLayout(
					get_text_shadow_offset(),
					text_layout.Get(),
					text_shadow_brush.Get());
			}

			// テキストを描画します。
			core.render_target->DrawTextLayout(
				D2D1::Point2F(0.0f, 0.0f),
				text_layout.Get(),
				text_brush.Get(),
				D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

			// テキストの高さを返します。
			auto text_metrics = DWRITE_TEXT_METRICS {};
			text_layout->GetMetrics(&text_metrics);
			return (int)text_metrics.height;
		}

		//
		// ::DrawTextW()方式で文字列を描画します。
		//
		int draw_text()
		{
			MY_TRACE_FUNC("{/hex}, {/}, ({/}), {/hex}", dc, s, safe_string(rc), text_flags);

			// 初期化が正常に完了していない場合は何もしません。
			if (!is_initialized()) return 0;

			// レンダーターゲットとデバイスコンテキストをバインドします。
			core_t::binder_t binder(dc, rc);

			if (hive.shadow.flag_use && hive.shadow.flag_blur)
				return draw_text_with_blur();
			else
				return draw_text_without_blur();
		}
	};
}
