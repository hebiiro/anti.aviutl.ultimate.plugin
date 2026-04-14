#pragma once

namespace apn::volume_meter
{
	//
	// 実数の型です。
	//
	using real_t = float;

	//
	// このクラスは音量です。
	//
	struct volume_t
	{
		inline static constexpr size_t c_nb_max_channels = 2;

		int32_t frame;
		struct channel_t {
			real_t level;
			real_t peak;
		} channels[c_nb_max_channels];

		//
		// デフォルトコンストラクタです。
		//
		volume_t ()
			: frame()
			, channels()
		{
		}

		//
		// コンストラクタです。
		//
		volume_t (const auto& raw_audio_data)
			: frame(raw_audio_data->audio_timing.frame)
		{
			const auto c = std::min(std::size(channels), (size_t)raw_audio_data->audio_ch);
			for (size_t ch = 0; ch < c; ch++) calc(raw_audio_data, ch);
		}

		//
		// 指定されたチャンネルの音量を算出します。
		//
		void calc(const auto& raw_audio_data, size_t ch)
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
//			auto audio_rate = (size_t)raw_audio_data->fi.audio_rate;

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

			channels[ch].level = 20 * log10(rms);
			channels[ch].peak = 20 * log10(peak);
		}
	};
}
