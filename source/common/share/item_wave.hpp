#pragma once

namespace apn::item_wave::share
{
	//
	// このクラスは音量です。
	//
	struct Volume
	{
		inline static constexpr int32_t c_resolution = 60; // 時間軸方向の分解能をFPSで指定します。

		//
		// 音量のレベルです。
		//
		uint8_t level;

		//
		// 音量を0.0～1.0の範囲で返します。
		//
		float to_real() const
		{
			return level / 255.0f;
		}

		//
		// 音量を乗算変更します。
		//
		void mul(float value)
		{
			level = (uint8_t)(level * value);
		}
	};
}
