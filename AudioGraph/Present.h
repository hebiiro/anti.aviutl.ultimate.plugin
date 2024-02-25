#pragma once

namespace fgo::audio_graph
{
	//
	// このクラスは受け取った音声信号から音量を算出し、保持します。
	//
	struct Present
	{
		//
		// 音量の算出に使用する音声信号です。
		//
		std::unique_ptr<short[]> audiop;

		//
		// 音声信号の数です。
		//
		int32_t audio_n;

		//
		// 音声信号のチャンネル数です。
		//
		int32_t audio_ch;

		//
		// 算出された音量です。
		//
		Share::AudioGraph::Volume volume;

		//
		// コンストラクタです。
		// fpとfpipから必要なデータを取得して、このオブジェクトで保持します。
		// メインスレッド側から呼ばれます。
		//
		Present(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, DWORD time)
			: audiop(new short[fpip->audio_n * fpip->audio_ch])
			, audio_n(fpip->audio_n)
			, audio_ch(fpip->audio_ch)
			, volume({ time, fpip->frame })
		{
			memcpy(audiop.get(), fpip->audiop, sizeof(short) * fpip->audio_n * fpip->audio_ch);
		}

		//
		// 音量レベルとピークを算出します。
		// サブスレッド側から呼ばれます。
		//
		void calc()
		{
			for (int ch = 0; ch < audio_ch; ch++)
				calc(ch);
		}

		//
		// 指定されたチャンネルの音量レベルとピークを算出します。
		//
		void calc(int ch)
		{
			float level = 0.0f;
			float peak = 0.0f;

			for (int i = 0; i < audio_n * audio_ch; i += audio_ch)
			{
				float n = normalize(audiop[i + ch]);

				level += n * n;
				peak = std::max(peak, std::fabs(n));
			}

			level = std::sqrt(level / audio_n);

			volume.level[ch] = 20 * std::log10(level);
			volume.peak[ch] = 20 * std::log10(peak);
		}

		//
		// 受け取った音声信号を-1.0～1.0に正規化して返します。
		//
		inline static float normalize(short pcm)
		{
			return pcm / 32768.0f;
		}
	};
}
