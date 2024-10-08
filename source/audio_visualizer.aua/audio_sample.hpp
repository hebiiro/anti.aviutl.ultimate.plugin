#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスは音声サンプルです。
	//
	struct AudioSample
	{
		//
		// 音声信号です。
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
		// ファイル情報です。
		//
		AviUtl::FileInfo fi;

		//
		// 音声信号のフレーム番号です。
		//
		int32_t frame;

		//
		// 音声信号を鳴らす時間です。
		//
		DWORD time;

		//
		// オプションです。
		//
		share::Option option;

		//
		// コンストラクタです。
		// fpとfpipから音声データを取得します。
		//
		AudioSample(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, DWORD time)
			: audiop(new short[fpip->audio_n * fpip->audio_ch])
			, audio_n(fpip->audio_n)
			, audio_ch(fpip->audio_ch)
			, fi()
			, frame(fpip->frame)
			, time(time)
			, option(hive.option)
		{
			memcpy(audiop.get(), fpip->audiop, sizeof(short) * fpip->audio_n * fpip->audio_ch);

			fp->exfunc->get_file_info(fpip->editp, &fi);
		}
	};
}
