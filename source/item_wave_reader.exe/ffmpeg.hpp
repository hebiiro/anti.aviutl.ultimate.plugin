#pragma once

namespace apn::item_wave::reader
{
	template <typename T, auto constructor, auto destructor>
	struct ctx_utils {
		T ctx = {};
		ctx_utils(auto... args) { constructor(ctx, args...); }
		~ctx_utils() { destructor(ctx); }
		T operator->() const { return ctx; }
		operator T() const { return ctx; }
		T* operator&() { return &ctx; }
		const T* operator&() const { return &ctx; }
	};

	//
	// このクラスはフォーマットのコンテキストです。
	//
	using avformat_ctx = ctx_utils<AVFormatContext*,
		[](auto& ctx, const std::string& file_name) {
			if (avformat_open_input(&ctx, file_name.c_str(), nullptr, nullptr) < 0)
				throw std::exception("avformat_open_input() failed");
		},
		[](auto& ctx) {
			avformat_close_input(&ctx);
		}>;

	//
	// このクラスはコーデックのコンテキストです。
	//
	using avcodec_ctx = ctx_utils<AVCodecContext*,
		[](auto& ctx, auto codec) {
			ctx = avcodec_alloc_context3(codec);
		},
		[](auto& ctx) {
			avcodec_free_context(&ctx);
		}>;

	//
	// このクラスはSWRのコンテキストです。
	//
	using swr_ctx = ctx_utils<SwrContext*,
		[](auto& ctx) { ctx = swr_alloc(); },
		[](auto& ctx) { swr_free(&ctx); }>;

	//
	// このクラスはチャンネルレイアウトのコンテキストです。
	//
	using avchannel_layout = ctx_utils<AVChannelLayout,
		[](auto& ctx, const AVChannelLayout& default_value = {}) { ctx = default_value; },
		[](auto& ctx) { av_channel_layout_uninit(&ctx); }>;

	//
	// このクラスはパケットのコンテキストです。
	//
	using avpacket = ctx_utils<AVPacket*,
		[](auto& ctx) { ctx = av_packet_alloc(); },
		[](auto& ctx) { av_packet_free(&ctx); }>;

	//
	// このクラスはフレームのコンテキストです。
	//
	using avframe = ctx_utils<AVFrame*,
		[](auto& ctx) { ctx = av_frame_alloc(); },
		[](auto& ctx) { av_frame_free(&ctx); }>;

	//
	// 指定された実数を音量範囲にマップします。
	//
	uint8_t map(float value)
	{
		auto mapped = (int)(value * 255.0f);
		mapped = std::max(mapped, 0);
		mapped = std::min(mapped, 255);

		return (uint8_t)mapped;
	}

	//
	// 指定されたサンプルからRMSを算出して返します。
	//
	uint8_t compute_rms(const float* samples, size_t size)
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
	uint8_t compute_peak(const float* samples, size_t size)
	{
		auto peak = 0.0f;
		for (size_t i = 0; i < size; i++)
			peak = std::max(peak, std::abs(samples[i]));
		return map(peak);
	}

	//
	// 指定された音声ファイルからフレーム(60FPS)毎のRMSを算出して返します。
	//
	std::vector<uint8_t> extract_rms(const std::string& file_name, auto compute_xxx)
	{
		constexpr auto TARGET_SAMPLE_RATE = 48000;
		constexpr auto TARGET_CHANNELS = 1;
		constexpr auto TARGET_SAMPLE_FMT = AV_SAMPLE_FMT_FLT;
		constexpr auto SAMPLES_PER_FRAME = TARGET_SAMPLE_RATE / 60;

		std::vector<uint8_t> rms_values;
		std::vector<float> samples;

		avformat_network_init();

		avformat_ctx fmt_ctx(file_name);
		if (!fmt_ctx)
			return std::cerr << "avformat_open_input() failed" << std::endl, rms_values;

		if (avformat_find_stream_info(fmt_ctx, nullptr) < 0)
			return std::cerr << "avformat_find_stream_info() failed" << std::endl, rms_values;

		auto audio_stream_index = av_find_best_stream(fmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
		if (audio_stream_index < 0)
			return std::cerr << "Failed to find audio stream" << std::endl, rms_values;

		auto audio_stream = fmt_ctx->streams[audio_stream_index];
		auto codecpar = audio_stream->codecpar;
		auto codec = avcodec_find_decoder(codecpar->codec_id);
		avcodec_ctx codec_ctx(codec);
		avcodec_parameters_to_context(codec_ctx, codecpar);
		avcodec_open2(codec_ctx, codec, nullptr);

		swr_ctx swr;

		avchannel_layout in_ch_layout(codec_ctx->ch_layout);
		avchannel_layout out_ch_layout;
		av_channel_layout_default(&out_ch_layout, TARGET_CHANNELS);

		av_opt_set_chlayout(swr, "in_chlayout", &in_ch_layout, 0);
		av_opt_set_int(swr, "in_sample_rate", codec_ctx->sample_rate, 0);
		av_opt_set_sample_fmt(swr, "in_sample_fmt", codec_ctx->sample_fmt, 0);

		av_opt_set_chlayout(swr, "out_chlayout", &out_ch_layout, 0);
		av_opt_set_int(swr, "out_sample_rate", TARGET_SAMPLE_RATE, 0);
		av_opt_set_sample_fmt(swr, "out_sample_fmt", TARGET_SAMPLE_FMT, 0);

		swr_init(swr);

		avpacket pkt;
		avframe frame;

		while (av_read_frame(fmt_ctx, pkt) >= 0)
		{
			if (pkt->stream_index != audio_stream_index)
				continue;

			if (avcodec_send_packet(codec_ctx, pkt) != 0)
				continue;

			while (avcodec_receive_frame(codec_ctx, frame) == 0)
			{
				auto out_samples = (int)av_rescale_rnd(
					swr_get_delay(swr, codec_ctx->sample_rate) + frame->nb_samples,
					TARGET_SAMPLE_RATE, codec_ctx->sample_rate, AV_ROUND_UP);

				uint8_t** converted = nullptr;
				av_samples_alloc_array_and_samples(&converted, nullptr,
					TARGET_CHANNELS, out_samples, TARGET_SAMPLE_FMT, 0);

				auto samples_converted = swr_convert(swr, converted,
					out_samples, frame->extended_data, frame->nb_samples);

				if (samples_converted > 0)
				{
					auto float_data = (float*)converted[0];
					samples.insert(samples.end(), float_data, float_data + samples_converted);

					while (samples.size() >= SAMPLES_PER_FRAME)
					{
						rms_values.push_back(compute_xxx(samples.data(), SAMPLES_PER_FRAME));
						samples.erase(samples.begin(), samples.begin() + SAMPLES_PER_FRAME);
					}
				}

				av_freep(&converted[0]);
				av_freep(&converted);
			}

			av_packet_unref(pkt);
		}

		// 残りのサンプルを処理します。
		if (samples.size())
			rms_values.push_back(compute_xxx(samples.data(), samples.size()));

		return rms_values;
	}
}
