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
		DWORD time;
		int32_t frame;
		struct channel_t {
			real_t level;
			real_t peak;
		} channels[2];
	};
}
