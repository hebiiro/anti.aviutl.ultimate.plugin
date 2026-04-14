#pragma once

namespace apn::volume_meter::sub_thread
{
	//
	// このクラスはサブスレッドのモデルです。
	//
	inline struct model_t
	{
		//
		// このクラスは音量メーターの描画設定です。
		// ペンやブラシのキャッシュも保持します。
		//
		struct cached_paint_option_t : paint_option_t
		{
			//
			// このクラスは描画用のキャッシュです。
			// 主にGDIオブジェクトを保持します。
			//
			struct cache_t
			{
				//
				// 背景色のブラシです。
				//
				my::gdi::unique_ptr<HBRUSH> background_brush;

				//
				// 警告色のブラシです。
				//
				my::gdi::unique_ptr<HBRUSH> warning_brush;

				//
				// セパレータ用のペンです。
				//
				my::gdi::unique_ptr<HPEN> separator_pen;

				//
				// レベル用のブラシです。
				//
				my::gdi::unique_ptr<HBRUSH> level_brush;

				//
				// 各ゾーンのキャッシュ(ブラシ)です。
				//
				struct zone_t {
					my::gdi::unique_ptr<HBRUSH> background_brush;
					my::gdi::unique_ptr<HBRUSH> foreground_brush;
				} zones[c_zone.c_max_size];

				//
				// 各フォントのキャッシュです。
				//
				my::gdi::unique_ptr<HFONT> fonts[c_font.c_max_size];
			} cache;

			//
			// 描画設定をセットします。
			//
			BOOL set(const paint_option_t& paint_option)
			{
				// 与えられた描画設定で上書きします。
				*(paint_option_t*)this = paint_option;

				// 描画設定が更新されたので、キャッシュを再作成します。
				{
					//
					// この関数は二つの色を混ぜ合わせて返します。
					//
					constexpr auto lerp = [](COLORREF color1, COLORREF color2)
					{
						return RGB(
							(GetRValue(color1) + GetRValue(color2)) / 2,
							(GetGValue(color1) + GetGValue(color2)) / 2,
							(GetBValue(color1) + GetBValue(color2)) / 2);
					};

					cache.background_brush.reset(::CreateSolidBrush(background_color));
					cache.warning_brush.reset(::CreateSolidBrush(warning_color));
					cache.separator_pen.reset(::CreatePen(PS_SOLID, separator_width, background_color));
					cache.level_brush.reset(::CreateSolidBrush(level_color));

					for (size_t i = 0; i < std::size(zones); i++)
					{
						cache.zones[i].background_brush.reset(::CreateSolidBrush(lerp(zones[i].color, background_color)));
						cache.zones[i].foreground_brush.reset(::CreateSolidBrush(zones[i].color));
					}

					for (size_t i = 0; i < std::size(fonts); i++)
					{
						const auto& font = fonts[i];

						auto angle = font.vertical ? 2700 : 0;

						cache.fonts[i].reset(::CreateFontW(
							font.height, font.width, angle, angle,
							FW_BLACK, FALSE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH, font.name.c_str()));
					}
				}

				return TRUE;
			}
		} paint_option;

		//
		// 描画設定をセットします。
		//
		BOOL set_paint_option(const auto& paint_option)
		{
			return this->paint_option.set(*paint_option);
		}

		//
		// 値を正規化して返します。
		//
		inline static real_t normalize(real_t value, real_t min, real_t max)
		{
			value = std::max(value, min);
			value = std::min(value, max);
			return (value - min) / (max - min);
		};

		//
		// dBを正規化して返します。
		//
		inline real_t normalize(real_t db)
		{
			return normalize(db, (real_t)paint_option.min_db, (real_t)paint_option.max_db);
		}
	} model;
}
