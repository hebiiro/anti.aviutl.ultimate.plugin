#pragma once

namespace apn::item_wave::item_cache
{
	//
	// 動画ファイルフィルタのIDです。
	//
	inline constexpr auto c_movie_file_filter_id = 0;

	//
	// 音声ファイルフィルタのIDです。
	//
	inline constexpr auto c_audio_file_filter_id = 2;

	//
	// シーンフィルタのIDです。
	//
	inline constexpr auto c_scene_filter_id = 7;

	//
	// シーン(音声)フィルタのIDです。
	//
	inline constexpr auto c_scene_audio_filter_id = 8;

	//
	// 標準再生フィルタのIDです。
	//
	inline constexpr auto c_standard_play_filter_id = 12;

	//
	// 指定されたアイテムが処理対象の場合はTRUEを返します。
	//
	inline static BOOL is_target_object(ExEdit::Object* object)
	{
		// カレントシーン内のアイテムの場合は
		if (object->scene_set == magi.exin.get_current_scene_index())
		{
			// 対象レイヤーが指定されている場合は
			if (hive.include_layers.size())
			{
				// アイテムが対象レイヤーにない場合はFALSEを返します。
				if (!hive.include_layers.contains(object->layer_set)) return FALSE;
			}

			// 除外レイヤーが指定されている場合は
			if (hive.exclude_layers.size())
			{
				// アイテムが除外レイヤーにある場合はFALSEを返します。
				if (hive.exclude_layers.contains(object->layer_set)) return FALSE;
			}
		}

		return TRUE;
	}

	//
	// このクラスはアイテム毎のキャッシュです。
	//
	struct node_t
	{
		//
		// フレーム毎の音量の配列です。
		//
		std::vector<share::volume_t> volumes;

		//
		// この仮想関数は、アイテムキャッシュが有効かどうか確認するために呼び出されます。
		//
		virtual BOOL is_invalid(int32_t self_object_index) = 0;

		//
		// この仮想関数は、アイテムキャッシュを更新するために呼び出されます。
		//
		virtual BOOL refresh(context_t& ctx, int32_t self_object_index) = 0;

		//
		// この仮想関数は、アイテムキャッシュを準備するために呼び出されます。
		//
		virtual BOOL prepare(context_t& ctx, int32_t self_object_index) = 0;
	};
}
