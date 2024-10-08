#pragma once

namespace apn::item_wave::reader
{
#pragma pack(1)
	//
	// この構造体は24bit整数計算を行うために使用されます。
	//
	struct int24_t {
		BYTE n[3];
		operator int32_t() const {
			struct {
				int32_t n:24;
			} s = { *(int32_t*)n };
			return s.n;
		}
	};
#pragma pack()

	//
	// このクラスは計算に必要な変数を保持します。
	//
	inline struct Hive
	{
		//
		// 計算結果を格納するための変数です。
		// 計算を実行する前にこの関数を呼び出して
		// 計算結果を格納するバッファをセットしてください。
		//
		share::ReaderBuffer* buffer = nullptr;

		//
		// 計算に必要な変数です。
		// 計算を実行する前にこの関数を呼び出して
		// 計算に必要なオーディオフォーマットをセットしてください。
		//
		WAVEFORMATEX audio_format = {};
	} hive;
}
