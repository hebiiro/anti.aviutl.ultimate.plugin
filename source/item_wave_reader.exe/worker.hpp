#pragma once

namespace apn::item_wave::reader
{
	//
	// このクラスは音声信号から音量を算出します。
	//
	struct Worker
	{
		//
		// 計算対象のフレーム番号です。
		//
		int i;

		//
		// 計算対象のフレームの音声信号です。
		//
		std::shared_ptr<std::vector<BYTE>> buffer;

		//
		// コンストラクタです。
		//
		Worker(int i, const std::shared_ptr<std::vector<BYTE>>& buffer)
			: i(i)
			, buffer(buffer)
		{
		}

		//
		// 音量を算出して出力バッファに格納します。
		//
		void calc_volume()
		{
			share::Volume volume = {};
			switch (hive.audio_format.wBitsPerSample)
			{
			case  8: volume.level = calc((const  int8_t*)buffer->data(), buffer->size() / sizeof( int8_t)); break;
			case 16: volume.level = calc((const int16_t*)buffer->data(), buffer->size() / sizeof(int16_t)); break;
			case 24: volume.level = calc((const int24_t*)buffer->data(), buffer->size() / sizeof(int24_t)); break;
			case 32: volume.level = calc((const int32_t*)buffer->data(), buffer->size() / sizeof(int32_t)); break;
			default: volume.level = 0.0f; break;
			}
			hive.buffer->volumes[i] = volume;

			MY_TRACE("i = {/}, level = {/}\n", i, volume.level);
		}

		//
		// 受け取った8bitの音声信号を-1.0～1.0に正規化して返します。
		//
		static float normalize(int8_t pcm)
		{
			return pcm / 128.0f;
		}

		//
		// 受け取った16bitの音声信号を-1.0～1.0に正規化して返します。
		//
		static float normalize(int16_t pcm)
		{
			return pcm / 32768.0f;
		}

		//
		// 受け取った24bitの音声信号を-1.0～1.0に正規化して返します。
		//
		static float normalize(int24_t pcm)
		{
			return pcm / 8388608.0f;
		}

		//
		// 受け取った32bitの音声信号を-1.0～1.0に正規化して返します。
		//
		static float normalize(int32_t pcm)
		{
			return pcm / 2147483648.0f;
		}

		//
		// 受け取った32bit浮動小数点数の音声信号をそのまま返します。
		//
		static float normalize(float pcm)
		{
			return pcm;
		}

		template <typename T>
		static float calc(const T* pcms, size_t count)
		{
			if (!count) return 0.0f;

			float level = 0.0f;

			for (size_t i = 0; i < count; i++)
			{
				T pcm = pcms[i];
				float n = normalize(pcm);
				level += n * n;
			}

			return sqrtf(level / (float)count);
		}
	};
}
