#pragma once

namespace apn::volume_meter
{
	//
	// このクラスは生の音声データです。
	//
	struct raw_audio_data_t
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
		// 音声信号のタイミングです。
		//
		timing_t audio_timing;

		//
		// 現在フレームです。
		//
		int32_t current_frame;

		//
		// TRUEの場合は再生中の音声信号です。
		//
		BOOL flag_is_playing;

		//
		// コンストラクタです。
		// fpとfpipから音声データを取得します。
		//
		raw_audio_data_t(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
			: audiop(std::make_unique<short[]>(fpip->audio_n * fpip->audio_ch))
			, audio_n(fpip->audio_n)
			, audio_ch(fpip->audio_ch)
			, audio_timing(fp, fpip)
			, current_frame(fp->exfunc->get_frame(fpip->editp))
			, flag_is_playing(magi.auin.is_playing())
		{
			memcpy(audiop.get(), fpip->audiop, sizeof(short) * fpip->audio_n * fpip->audio_ch);
		}
	};
}
