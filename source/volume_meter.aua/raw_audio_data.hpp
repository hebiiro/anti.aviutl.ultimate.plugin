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
		// コンストラクタです。
		// fpとfpipから音声データを取得します。
		//
		raw_audio_data_t(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const AviUtl::FileInfo& fi, DWORD time)
			: audiop(std::make_unique<short[]>(fpip->audio_n * fpip->audio_ch))
			, audio_n(fpip->audio_n)
			, audio_ch(fpip->audio_ch)
			, fi(fi)
			, frame(fpip->frame)
			, time(time)
		{
			memcpy(audiop.get(), fpip->audiop, sizeof(short) * fpip->audio_n * fpip->audio_ch);
		}
	};
}
