#pragma once

namespace apn::item_wave
{
	//
	// このクラスは音声パラメータです。
	//
	struct AudioParams {
		uint32_t id = 0;
		uint32_t flag = 0;
		int32_t frame_begin = 0;
		int32_t frame_end = 0;
		int32_t scene_set = 0;
		std::filesystem::path file_name;
		float volume = 1.0f;
		float play_begin = 0.0f;
		float play_speed = 1.0f;
		uint32_t layer_flag = 0;
	};

	//
	// このクラスはアイテム毎のキャッシュです。
	//
	struct ItemCache {
		std::vector<share::Volume> volumes;
		std::shared_ptr<AudioParams> audio_params;
	};
}
