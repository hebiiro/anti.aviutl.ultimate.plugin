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
		// 音量です。
		//
		volume_t volume;

		//
		// 描画設定をセットします。
		//
		BOOL set_paint_option(const auto& paint_option)
		{
			return this->paint_option.set(*paint_option);
		}

		//
		// 生の音声データを元に音量をセットします。
		//
		BOOL set_volume(const auto& raw_audio_data)
		{
			//
			// この関数は生の音声データの解析処理が遅れている場合はTRUEを返します。
			//
			const auto is_late = [&]() -> BOOL
			{
				// 生の音声データの時間が有効の場合は
				if (raw_audio_data->time)
				{
					// 現在時間を取得します。
					auto time = ::timeGetTime();

					// 許容範囲以上に遅れている場合は解析をスキップします。
					if (time > raw_audio_data->time && time - raw_audio_data->time > 5)
					{
						MY_TRACE("{/}, {/.3f}, {/.3f}, 解析が間に合っていないのでスキップします\n",
							raw_audio_data->frame, time / 1000.0, raw_audio_data->time / 1000.0);

						return TRUE;
					}
				}

				return FALSE;
			};

			// 生の音声データの解析処理が遅れている場合は何もしません。
			if (is_late()) return FALSE;

			volume.frame = raw_audio_data->frame;
			volume.time = raw_audio_data->time;

			for (decltype(raw_audio_data->audio_ch) ch = 0; ch < raw_audio_data->audio_ch; ch++)
				set_volume(raw_audio_data, ch);

			return TRUE;
		}

		//
		// 指定されたチャンネルの音量をセットします。
		//
		void set_volume(const auto& raw_audio_data, int32_t ch)
		{
			//
			// この関数は与えられた音声信号を-1.0～1.0に正規化して返します。
			//
			constexpr auto normalize = [](short pcm)
			{
				return pcm / 32768.0f;
			};

			auto audio_n = (size_t)raw_audio_data->audio_n;
			auto audio_ch = (size_t)raw_audio_data->audio_ch;
			auto audio_rate = (size_t)raw_audio_data->fi.audio_rate;

			// サンプルの配列を作成します。
			std::vector<float> samples(audio_n);
			for (decltype(audio_n) i = 0; i < audio_n; i++)
				samples[i] = normalize(raw_audio_data->audiop[i * audio_ch + ch]);

			// RMSとピークを算出します。
			auto rms = 0.0f;
			auto peak = 0.0f;

			for (auto sample : samples)
			{
				auto s = sample * sample;
				rms += s;
				peak = std::max(peak, s);
			}

			rms = sqrt(rms / samples.size());
			peak = sqrt(peak);

			volume.channels[ch].level = 20 * log10(rms);
			volume.channels[ch].peak = 20 * log10(peak);
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
