#pragma once

namespace apn::volume_meter::sub_thread
{
	//
	// このクラスは音量メーターを描画します。
	//
	struct painter_t
	{
		RECT rc;
		int32_t w, h;
		RECT text_rc;
		RECT graph_rc;
		int32_t graph_h;
		my::PaintDC pdc;
		my::DoubleBufferDC dc;
		real_t gamma;

		//
		// モデルのキャッシュです。
		//
		inline static const auto& cache = model.paint_option.cache;

		//
		// ゾーンの数です。
		//
		inline static constexpr auto nb_zones = std::size(model.paint_option.zones);

		//
		// 各ゾーンのコンテキストです。
		//
		struct zone_context_t {
			decltype(model.paint_option.zones + 0) p;
			int32_t top, bottom;
		} zone_contexts[nb_zones] = {};

		//
		// チャンネルの数です。
		//
		inline static constexpr auto nb_channels = std::size(model.volume.channels);

		//
		// 各チャンネルのコンテキストです。
		//
		struct channel_context_t {
			decltype(model.volume.channels + 0) p;
			RECT rc;
			struct {
				int32_t y;
				std::wstring str;
			} peak, level;
		} channel_contexts[nb_channels] = {};

		//
		// コンストラクタです。
		//
		painter_t(HWND hwnd)
			: rc(my::get_client_rect(hwnd))
			, w(my::get_width(rc))
			, h(my::get_height(rc))
			, pdc(hwnd)
			, dc(pdc, &rc)
			, gamma(model.paint_option.gamma / 100.0f)
		{
			// 文字部分の描画矩形を算出します。
			text_rc = rc;
			text_rc.top = text_rc.bottom - model.paint_option.text_area_height;

			// グラフ部分の描画矩形を算出します。
			graph_rc = rc;
			graph_rc.bottom = text_rc.top;
			graph_h = my::get_height(graph_rc);

			// 各ゾーンのコンテキストを初期化します。
			{
				// ゾーンの座標です。
				auto y = graph_rc.top;

				// ゾーンコンテキストを走査します。
				for (size_t i = 0; i < nb_zones; i++)
				{
					zone_contexts[i].p = &model.paint_option.zones[i];
					zone_contexts[i].top = y;
					zone_contexts[i].bottom = db_to_pixel((real_t)model.paint_option.zones[i].db);

					// 次のゾーンの座標に変更します。
					y = zone_contexts[i].bottom;
				}
			}

			// チャンネルコンテキストを初期化します。
			{
				// チャンネルコンテキストを走査します。
				for (size_t i = 0; i < nb_channels; i++)
				{
					auto& channel = model.volume.channels[i];

					// チャンネルコンテキストの共通部分を初期化します。
					channel_contexts[i].p = &channel;
					channel_contexts[i].rc = rc;
					channel_contexts[i].peak.y = db_to_pixel(channel.peak);
					channel_contexts[i].peak.str = my::format(L"{/0.2f}", channel.peak);
					channel_contexts[i].level.y = db_to_pixel(channel.level);
					channel_contexts[i].level.str = my::format(L"{/0.2f}", channel.level);
				}

				// 左チャンネルコンテキストを初期化します。
				channel_contexts[0].rc.left = rc.left;
				channel_contexts[0].rc.right = rc.left + (w / nb_channels);

				// 右チャンネルコンテキストを初期化します。
				channel_contexts[1].rc.left = rc.right - (w / nb_channels);
				channel_contexts[1].rc.right = rc.right;
			}
		}

		//
		// 影付きの文字列を描画します。
		//
		inline static BOOL text_out(HDC dc, int32_t x, int32_t y, const std::wstring& s, size_t font_index)
		{
			const auto& font = model.paint_option.fonts[font_index];

			// フォント名が空の場合は描画しません。
			if (font.name.empty()) return FALSE;

			my::gdi::selector font_selector(dc, cache.fonts[font_index].get());

			x += font.text_offset.x;
			y += font.text_offset.y;

			if (font.shadow_offset.x || font.shadow_offset.y)
			{
				::SetTextColor(dc, font.shadow_color);
				::TextOutW(dc, x + font.shadow_offset.x, y + font.shadow_offset.y, s.c_str(), (int)s.length());
			}

			::SetTextColor(dc, font.text_color);
			::TextOutW(dc, x, y, s.c_str(), (int)s.length());

			return TRUE;
		}

		//
		// 影付きの文字列を描画します。
		//
		inline static BOOL draw_text(HDC dc, const std::wstring& s, LPRECT rc, UINT format, size_t font_index)
		{
			const auto& font = model.paint_option.fonts[font_index];

			// フォント名が空の場合は描画しません。
			if (font.name.empty()) return FALSE;

			my::gdi::selector font_selector(dc, cache.fonts[font_index].get());

			auto text_rc = *rc;
			::OffsetRect(&text_rc, font.text_offset.x, font.text_offset.y);

			if (font.shadow_offset.x || font.shadow_offset.y)
			{
				auto shadow_rc = text_rc;
				::OffsetRect(&shadow_rc, font.shadow_offset.x, font.shadow_offset.y);

				::SetTextColor(dc, font.shadow_color);
				::DrawTextW(dc, s.c_str(), s.length(), &shadow_rc, format);
			}

			::SetTextColor(dc, font.text_color);
			::DrawTextW(dc, s.c_str(), s.length(), &text_rc, format);

			return TRUE;
		}

		//
		// 与えられたdBをピクセル単位に変換して返します。
		//
		int32_t db_to_pixel(float db)
		{
			auto bar_h = (int32_t)(pow(model.normalize(db), gamma) * graph_h);

			return graph_rc.bottom - bar_h;
		};

		//
		// ゾーンの背景を描画します。
		//
		void draw_zone_background()
		{
			// ゾーンの描画位置です。
			auto rc = graph_rc;

			// ゾーンコンテキストを走査します。
			for (size_t i = 0; i < nb_zones; i++)
			{
				// ゾーンコンテキストを取得します。
				const auto& zone_context = zone_contexts[i];

				// ゾーンの描画位置を算出します。
				rc.top = zone_context.top;
				rc.bottom = zone_context.bottom;

				// ゾーンの背景ブラシでゾーンを塗りつぶします。
				::FillRect(dc, &rc, cache.zones[i].background_brush.get());
			}
		}

		//
		// ピーク(バー)を描画します。
		//
		void draw_bar()
		{
			// チャンネルコンテキストを走査します。
			for (size_t i = 0; i < nb_channels; i++)
			{
				// チャンネルコンテキストを取得します。
				const auto& channel_context = channel_contexts[i];

				// ゾーンの描画位置です。
				auto rc = channel_context.rc;

				// ゾーンコンテキストを走査します。
				for (size_t i = 0; i < nb_zones; i++)
				{
					// ゾーンコンテキストを取得します。
					const auto& zone_context = zone_contexts[i];

					// 音量がゾーンまで到達していない場合は何もしません。
					if (channel_context.peak.y >= zone_context.bottom) continue;

					// ゾーンの描画位置を算出します。
					rc.top = std::max<int32_t>(channel_context.peak.y, zone_context.top);
					rc.bottom = zone_context.bottom;

					// ゾーンのブラシでバーを描画します。
					::FillRect(dc, &rc, cache.zones[i].foreground_brush.get());
				}
			}
		}

		//
		// セパレータを描画します。
		//
		void draw_separator()
		{
			// デバイスコンテキストにペンをセットします。
			my::gdi::selector pen_selector(dc, cache.separator_pen.get());

			// チャンネル間のセパレータを描画します。
			{
				// セパレータがある座標を算出します。
				auto x = graph_rc.left + (w / nb_channels);

				// セパレータのラインを描画します。
				::MoveToEx(dc, x, graph_rc.top, nullptr);
				::LineTo(dc, x, graph_rc.bottom);
			}

			// dBの範囲と基準を算出します。
			auto range_db = (real_t)(model.paint_option.max_db - model.paint_option.min_db);
			auto base_db = (real_t)model.paint_option.min_db;

			// セパレータの数 - 1 だけループします。
			for (int32_t i = 0; i < model.paint_option.nb_separators - 1; i++)
			{
				// セパレータがあるdBを算出します。
				auto db = range_db * (i + 1) / model.paint_option.nb_separators + base_db;

				// セパレータがある座標を算出します。
				auto y = graph_rc.top + db_to_pixel(db);

				// セパレータのラインを描画します。
				::MoveToEx(dc, graph_rc.left, y, nullptr);
				::LineTo(dc, graph_rc.right, y);
			}
		}

		//
		// ゾーンの閾値(文字列)を描画します。
		//
		void draw_zone_string()
		{
			// デバイスコンテキストにテキスト配置モードをセットします。
			auto old_text_align = ::SetTextAlign(dc, TA_RIGHT | TA_BOTTOM);

			// ゾーンコンテキストを走査します。
			for (size_t i = 0; i < nb_zones; i++)
			{
				// ゾーンコンテキストを取得します。
				const auto& zone_context = zone_contexts[i];

				// 閾値の描画位置を算出します。
				auto x = graph_rc.right;
				auto y = zone_context.bottom;

				// 閾値を文字列に変換します。
				auto text = my::format(L"{/}",
					std::max(zone_context.p->db, model.paint_option.min_db));

				// 文字列に変換した閾値を描画します。
				text_out(dc, x, y, text, model.paint_option.c_font.c_zone);
			}

			// デバイスコンテキストのテキスト配置モードを元に戻します。
			::SetTextAlign(dc, old_text_align);
		}

		//
		// レベル(rms)を描画します。
		//
		void draw_level()
		{
			// レベルの幅が無効の場合は描画しません。
			if (model.paint_option.level_width <= 0) return;

			// チャンネルコンテキストを走査します。
			for (size_t i = 0; i < nb_channels; i++)
			{
				// チャンネルコンテキストを取得します。
				const auto& channel_context = channel_contexts[i];

				// レベルの描画位置を算出します。
				auto rc = channel_context.rc;
				rc.top = channel_context.level.y - model.paint_option.level_width / 2;
				rc.bottom = rc.top + model.paint_option.level_width;

				// レベルを表す矩形を描画します。
				::FillRect(dc, &rc, cache.level_brush.get());
			}
		}

		//
		// ピークとレベルを文字列として描画します。
		//
		void draw_label()
		{
			// デバイスコンテキストにテキスト配置モードをセットします。
			auto old_text_align = ::SetTextAlign(dc, TA_LEFT | TA_BOTTOM);

			// チャンネルコンテキストを走査します。
			for (size_t i = 0; i < nb_channels; i++)
			{
				// チャンネルコンテキストを取得します。
				const auto& channel_context = channel_contexts[i];

				// レベルの数値を描画します。
				{
					// 数値の描画位置を算出します。
					auto text_x = channel_context.rc.left;
					auto text_y = channel_context.level.y + model.paint_option.level_width / 2;

					// 文字列に変換したレベルを描画します。
					text_out(dc, text_x, text_y, channel_context.level.str, model.paint_option.c_font.c_level);
				}

				// ピークの数値を描画します。
				{
					// 数値の描画位置を算出します。
					auto text_x = channel_context.rc.left;
					auto text_y = channel_context.peak.y;

					// 文字列に変換したピークを描画します。
					text_out(dc, text_x, text_y, channel_context.peak.str, model.paint_option.c_font.c_peak);
				}
			}

			// デバイスコンテキストのテキスト配置モードを元に戻します。
			::SetTextAlign(dc, old_text_align);
		}

		//
		// 全体のピークを描画します。
		//
		void draw_total_peak()
		{
			// 全体ピークの矩形が無効の場合は描画しません。
			if (my::get_height(text_rc) <= 0) return;

			// 全体のピークを算出します。
			auto peak = -100.0f;
			for (size_t i = 0; i < nb_channels; i++)
				peak = std::max(peak, model.volume.channels[i].peak);

			// テキストの背景を描画します。
			::FillRect(dc, &text_rc, (peak < -0.001f) ?
				cache.background_brush.get():  cache.warning_brush.get());

			// 全体のピークを文字列に変換します。
			auto text = my::format(L"{/0.2f}", peak);

			// 文字列に変換した全体のピークを描画します。
			draw_text(dc, text.c_str(), &text_rc,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE,
				model.paint_option.c_font.c_total);
		}

		//
		// 音量メーターを描画します。
		//
		void operator()()
		{
			// デバイスコンテキストに共通設定をセットします。
			::SetBkMode(dc, TRANSPARENT);

			draw_zone_background();
			draw_bar();
			draw_separator();
			draw_zone_string();
			draw_level();
			draw_label();
			draw_total_peak();
		}
	};
}
