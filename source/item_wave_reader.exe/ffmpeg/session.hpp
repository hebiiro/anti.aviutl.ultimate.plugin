#pragma once

namespace ffmpeg
{
	//
	// このクラスはセッションです。
	//
	struct Session
	{
		avformat_context format_context = {};
		avcodec_context codec_context = {};
		avpacket packet = {};
		avframe frame = {};
		int stream_index = -1;
		AVStream* stream = {};

		size_t sample_rate = {};
		swr_context swr = {};

		//
		// 初期化処理を実行します。
		//
		bool init(const std::string& file_path)
		{
			MY_TRACE_FUNC("");

			// フォーマットコンテキストを初期化します。
			avformat_open_input(&format_context, file_path.c_str(), nullptr, nullptr);
			if (!format_context)
				return MY_TRACE("avformat_open_input()が失敗しました\n"), false;

			// フォーマットコンテキストを使用して、ストリームを検索します。
			if (avformat_find_stream_info(format_context, nullptr) < 0)
				return MY_TRACE("avformat_find_stream_info()が失敗しました\n"), false;

			// 音声ストリームのインデックスを取得します。
			stream_index = av_find_best_stream(format_context, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
			if (stream_index < 0)
				return MY_TRACE("av_find_best_stream()が失敗しました\n"), false;

			// 音声ストリームを取得します。
			stream = format_context->streams[stream_index];

			// コーデックを取得します。
			auto codecpar = stream->codecpar;
			auto codec = avcodec_find_decoder(codecpar->codec_id);

			// コーデックコンテキストを作成します。
			codec_context.reset(avcodec_alloc_context3(codec));

			// コーデックパラメータをコーデックコンテキストのパラメータに変換します。
			if (avcodec_parameters_to_context(codec_context, stream->codecpar) < 0)
				return MY_TRACE("avcodec_parameters_to_context()が失敗しました\n"), false;

			// コーデックコンテキストをマルチスレッド対応にします。
			codec_context->thread_count = std::thread::hardware_concurrency();
			codec_context->thread_type = FF_THREAD_FRAME;

			// コーデックコンテキストを初期化します。
			if (avcodec_open2(codec_context, codec, nullptr) < 0)
				return MY_TRACE("avcodec_open2()が失敗しました\n"), false;

			// サンプルレートを取得します。
			sample_rate = codec_context->sample_rate;
			MY_TRACE_INT(sample_rate);

			// 音声フォーマット変換用コンテキストを初期化します。
			{
				swr.reset(swr_alloc());

				auto in_ch_layout = codec_context->ch_layout;
				auto out_ch_layout = decltype(in_ch_layout) {};
				av_channel_layout_default(&out_ch_layout, 1);

				av_opt_set_chlayout(swr, "in_chlayout", &in_ch_layout, 0);
				av_opt_set_int(swr, "in_sample_rate", codec_context->sample_rate, 0);
				av_opt_set_sample_fmt(swr, "in_sample_fmt", codec_context->sample_fmt, 0);

				av_opt_set_chlayout(swr, "out_chlayout", &out_ch_layout, 0); // モノラルに変換します。
				av_opt_set_int(swr, "out_sample_rate", codec_context->sample_rate, 0); // サンプルレートは変換しません。
				av_opt_set_sample_fmt(swr, "out_sample_fmt", AV_SAMPLE_FMT_FLT, 0); // floatに変換します。

				swr_init(swr);

				av_channel_layout_uninit(&out_ch_layout);
			}

			// パケットを作成します。
			packet.reset(av_packet_alloc());

			// フレームを作成します。
			frame.reset(av_frame_alloc());

			MY_TRACE("初期化が完了しました\n");

			return true;
		}

		//
		// 後始末処理を実行します。
		//
		bool reset()
		{
			MY_TRACE_FUNC("");

			return true;
		}

		//
		// 指定された実数を音量範囲にマップします。
		//
		inline static uint8_t map(float value)
		{
			auto mapped = (int)(value * 255.0f);
			mapped = std::max(mapped, 0);
			mapped = std::min(mapped, 255);

			return (uint8_t)mapped;
		}

		//
		// 指定されたサンプルからRMSを算出して返します。
		//
		inline static uint8_t compute_rms(const float* samples, size_t size)
		{
			auto sum_squares = 0.0f;
			for (size_t i = 0; i < size; i++) {
				auto s = samples[i];
				sum_squares += s * s;
			}
			auto rms = std::sqrt(sum_squares / size);
			return map(rms);
		}

		//
		// 指定されたサンプルからピークを算出して返します。
		//
		inline static uint8_t compute_peak(const float* samples, size_t size)
		{
			auto peak = 0.0f;
			for (size_t i = 0; i < size; i++)
				peak = std::max(peak, std::abs(samples[i]));
			return map(peak);
		}

		//
		// 指定されたフレームのサンプル番号を返します。
		//
		inline size_t get_sample_num(size_t frame_num)
		{
			return frame_num * sample_rate / 60;
		};

		//
		// 指定されたフレームのサンプル数を返します。
		//
		inline size_t get_nb_samples_per_frame(size_t current_frame_num)
		{
			// 次のフレーム番号を取得します。
			auto next_frame_num = current_frame_num + 1;

			// 現在のサンプル番号を取得します。
			auto current_sample_num = get_sample_num(current_frame_num);

			// 次のサンプル番号を取得します。
			auto next_sample_num = get_sample_num(next_frame_num);

			// フレーム番号の差分を返します。
			return next_sample_num - current_sample_num;
		};

		//
		// 指定された音声ファイルからフレーム(60FPS)毎の音量を算出して返します。
		//
		std::vector<uint8_t> extract_volumes(const std::string& compute_mode)
		{
			MY_TRACE_FUNC("");

			auto compute_func = compute_peak;
			if (compute_mode == "rms") compute_func = compute_rms;

			auto volumes = std::vector<uint8_t> {};
			auto samples = std::vector<float> {};
			auto nb_samples_per_frame = get_nb_samples_per_frame(0);
			MY_TRACE_INT(nb_samples_per_frame);

			while (av_read_frame(format_context, packet) >= 0)
			{
				// スコープ終了時にパケットの参照を解放します。
				scope_exit packet_exit([this](){ av_packet_unref(packet); });

				if (packet->stream_index != stream_index) continue;
				if (avcodec_send_packet(codec_context, packet) != 0) continue;

				// デコーダからフレームを受け取ります。
				while (avcodec_receive_frame(codec_context, frame) == 0)
				{
					// スコープ終了時にフレームの参照を解放します。
					scope_exit frame_exit([this](){ av_frame_unref(frame); });

					// 蓄積されているサンプル数を取得します。
					auto c = samples.size();

					// 取得予定のサンプル数の分だけ領域を拡張します。
					samples.resize(c + frame->nb_samples);

					// サンプルを取得します。
					auto p = (uint8_t*)(samples.data() + c);
					auto nb_converted_samples = swr_convert(
						swr, &p, frame->nb_samples,
						frame->extended_data, frame->nb_samples);

					// 取得したサンプル数の分だけに領域を縮小します。
					samples.resize(c + nb_converted_samples);

					// サンプルを取得できなかった場合はループを終了します。
					if (nb_converted_samples <= 0) break;

					// 音量を算出可能な数のサンプルが蓄積されている場合は
					while (samples.size() >= nb_samples_per_frame)
					{
						// サンプルから音量を算出します。
						volumes.emplace_back(compute_func(samples.data(), nb_samples_per_frame));

						// 計算が終わったサンプルを取り除きます。
						samples.erase(samples.begin(), samples.begin() + nb_samples_per_frame);

						// フレームあたりのサンプル数を更新します。
						nb_samples_per_frame = get_nb_samples_per_frame(volumes.size());
//						MY_TRACE_INT(nb_samples_per_frame);
					}
				}
			}

			// 残りのサンプルから音量を算出します。
			if (samples.size())
				volumes.emplace_back(compute_func(samples.data(), samples.size()));

			MY_TRACE_INT(volumes.size());

			// 算出した音量を返します。
			return volumes;
		}
	};
}
