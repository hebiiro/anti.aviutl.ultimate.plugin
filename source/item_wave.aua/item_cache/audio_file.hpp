#pragma once

namespace apn::item_wave::item_cache
{
	//
	// このクラスは音声ファイルアイテムのキャッシュです。
	//
	struct audio_file_t : node_t
	{
		//
		// このクラスはアイテムのプロパティです。
		//
		struct prop_t
		{
			int32_t frame_begin = 0;
			int32_t frame_end = 0;
			int32_t scene_set = 0;
			BOOL loop = FALSE;
			BOOL link = FALSE;
			std::filesystem::path file_name;
			float volume = 1.0f;
			float play_begin = 0.0f;
			float play_speed = 1.0f;

			std::shared_ptr<file_cache::node_t> file_cache;

			//
			// 指定されたアイテムのプロパティが等しい場合はTRUEを返します。
			//
			inline friend BOOL operator==(const prop_t& lhs, const prop_t& rhs)
			{
				return !operator!=(lhs, rhs);
			}

			//
			// 指定されたアイテムのプロパティが異なる場合はTRUEを返します。
			//
			inline friend BOOL operator!=(const prop_t& lhs, const prop_t& rhs)
			{
				if (lhs.loop != rhs.loop) return TRUE;
				if (lhs.link != rhs.link) return TRUE;
				if (lhs.volume != rhs.volume) return TRUE;
				if (lhs.play_begin != rhs.play_begin) return TRUE;
				if (lhs.play_speed != rhs.play_speed) return TRUE;
				if (lhs.frame_end != rhs.frame_end) return TRUE;
				if (lhs.file_cache != rhs.file_cache) return TRUE;

				return FALSE;
			}
		};

		//
		// アイテムのプロパティです。
		//
		std::optional<prop_t> prop;

		//
		// 指定されたアイテムのプロパティを返します。
		//
		inline static std::optional<prop_t> get_prop(const context_t& ctx, int32_t object_index)
		{
			// アイテムを取得します。
			auto object = magi.exin.get_object(object_index);

			// 音声ファイルフィルタのインデックスを取得します。
			auto audio_file_filter_index = magi.exin.get_filter_index(object, c_audio_file_filter_id);
			if (audio_file_filter_index == -1) return std::nullopt;

			// 標準再生フィルタのインデックスを取得します。
			auto standard_play_filter_index = magi.exin.get_filter_index(object, c_standard_play_filter_id);
			if (standard_play_filter_index == -1) return std::nullopt;

			auto volume_index = object->filter_param[standard_play_filter_index].track_begin + 0;
			auto pan_index = object->filter_param[standard_play_filter_index].track_begin + 1;
			auto play_begin_index = object->filter_param[audio_file_filter_index].track_begin + 0;
			auto play_speed_index = object->filter_param[audio_file_filter_index].track_begin + 1;

			auto prop = prop_t {};
			prop.loop = object->check_value[0];
			prop.link = object->check_value[1];

			// 音声ファイルアイテムが動画ファイルアイテムと連携している場合は
			if (prop.link)
			{
				// 音声ファイルアイテムと連携している動画ファイルアイテムを取得します。
				auto object2 = get_movie_file_item(ctx, object);
				if (!object2) return std::nullopt;

				auto play_begin_index = object2->filter_param[0].track_begin + 0;
				auto play_speed_index = object2->filter_param[0].track_begin + 1;

				// 動画ファイルアイテムから拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efMovieFile*)magi.exin.get_exdata(object2, 0);

				auto play_begin = (float)(object2->track_value_left[play_begin_index] - 1);
				auto fps = (float)exdata->video_rate / exdata->video_scale;

				prop.file_name = std::filesystem::absolute(exdata->file);
				prop.play_begin = play_begin / fps;
				prop.play_speed = object2->track_value_left[play_speed_index] / 1000.0f;
			}
			else
			{
				// 音声ファイルアイテムから拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efAudioFile*)magi.exin.get_exdata(object, 0);

				prop.file_name = std::filesystem::absolute(exdata->file);
				prop.play_begin = object->track_value_left[play_begin_index] / 100.0f;
				prop.play_speed = object->track_value_left[play_speed_index] / 1000.0f;
			}

			prop.frame_begin = object->frame_begin;
			prop.frame_end = object->frame_end;
			prop.scene_set = object->scene_set;
			prop.volume = object->track_value_left[volume_index] / 1000.0f;

			// ファイルキャッシュを取得します。
			prop.file_cache = file_cache::manager.get(prop.file_name);

			return prop;
		}

		//
		// 音声ファイルアイテムと連携している動画ファイルアイテムを返します。
		//
		inline static ExEdit::Object* get_movie_file_item(const context_t& ctx, ExEdit::Object* object)
		{
			auto result = (ExEdit::Object*)nullptr;
			auto distance = 100;

			// 同じ開始位置で最も近いレイヤーにある動画アイテムを探します。
			for (auto object_index : ctx.object_indexes)
			{
				// アイテムを取得します。
				auto object2 = magi.exin.get_object(object_index);

				// シーンが異なるアイテムは無視します。
				if (object2->scene_set != object->scene_set) continue;

				// 開始位置が異なるアイテムは無視します。
				if (object2->frame_begin != object->frame_begin) continue;

				// レイヤー位置が下のアイテムは無視します。
				if (object2->layer_set >= object->layer_set) continue;

				// レイヤー位置が遠い場合は無視します。
				auto distance2 = object->layer_set - object2->layer_set;
				if (distance2 > distance) continue;
#if 0
				// 音声ファイルアイテムの場合は
				if (object2->filter_param[0].id == c_audio_file_filter_id)
				{
					// この音声ファイルアイテムより下にある動画ファイルアイテムのみを連携対象とみなします。
					result = nullptr;
					distance = distance2;
				}
				else
#endif
				{
					// 動画ファイル以外のアイテムは無視します。
					if (object2->filter_param[0].id != c_movie_file_filter_id) continue;

					// この動画ファイルアイテムを連携対象とします。
					result = object2;
					distance = distance2;
				}
			}

			return result;
		}

		//
		// アイテムキャッシュが無効の場合はTRUEを返します。
		//
		virtual BOOL is_invalid(int32_t self_object_index) override
		{
			return !prop;
		}

		//
		// 音量を用意できた場合はTRUEを返します。
		//
		virtual BOOL refresh(context_t& ctx, int32_t self_object_index) override
		{
			{
				// アイテムのプロパティを取得します。
				auto prop = get_prop(ctx, self_object_index);

				// アイテムのプロパティに変更がない場合は何もしません。
				if (this->prop == prop) return !!this->prop;

				// アイテムのプロパティを更新します。
				this->prop = prop;

				// アイテムが配置されているレイヤーを再描画予定にします。
				ctx.add_redraw_layer(self_object_index);

				// 音量を消去します。
				volumes.clear();

				// アイテムのプロパティが無効の場合はFALSEを返します。
				if (!this->prop) return FALSE;
			}

			// アイテムのプロパティへの参照を取得します。
			const auto& prop = this->prop.value();

			// 対象フォルダが指定されている場合は
			if (hive.include_folder.size())
			{
				// 対象フォルダではない場合はTRUEを返します。
				if (!::StrStrIW(prop.file_name.c_str(), hive.include_folder.c_str()))
					return TRUE;
			}

			// 除外フォルダが指定されている場合は
			if (hive.exclude_folder.size())
			{
				// 除外フォルダの場合はTRUEを返します。
				if (::StrStrIW(prop.file_name.c_str(), hive.exclude_folder.c_str()))
					return TRUE;
			}

			// ファイルキャシュを取得します。
			const auto& file_cache = prop.file_cache;

			// ファイルキャシュを取得できなかった場合は
			if (!file_cache)
			{
				// ファイルキャッシュの作成を予約します。
				file_cache::manager.reserve(prop.file_name);

				// アイテムキャッシュを用意できなかったので
				// アイテムのプロパティをリセットしてFALSEを返します。
				return this->prop.reset(), FALSE;
			}

			// アイテムを取得します。
			auto object = magi.exin.get_object(self_object_index);

			// 現在のプロジェクトのフレームレートを算出します。
			auto fps = (float)hive.current_fi.video_rate / hive.current_fi.video_scale;

			// フレームレート変換倍率を算出します。
			auto scale = share::volume_t::c_resolution / fps;

			// アイテムのフレーム数を取得します。
			auto c = object->frame_end + 1 - object->frame_begin;

			// 音量の配列をリサイズします。
			volumes.resize(c);

			// アイテムの全フレームを走査します。
			for (decltype(c) i = 0; i < c; i++)
			{
				// 音声パラメータからアイテムフレームを算出します。
				auto item_frame = (prop.play_speed * i) + (prop.play_begin * fps);

				// ファイルフレームを算出します。
				auto file_frame = (int32_t)(item_frame * scale);

				// ファイルフレームが無効の場合は
				if (file_frame >= (int32_t)file_cache->volumes.size())
					break; // 走査を終了します。

				// ファイルキャッシュの音量を取得します。
				auto volume = file_cache->volumes[file_frame];

				// 音量を調整します。(アイテムの音量を乗算します)
				volume.mul(prop.volume);

				// 音量を格納します。
				volumes[i] = volume;
			}

			// アイテムが配置されているレイヤーを再描画します。
			ctx.add_redraw_layer(self_object_index);

			// アイテムキャッシュを用意できたのでTRUEを返します。
			return TRUE;
		}

		//
		// アイテムキャッシュを準備します。
		//
		virtual BOOL prepare(context_t& ctx, int32_t self_object_index) override
		{
			// アイテムのプロパティが無効の場合は何もしません。
			if (!prop) return FALSE;

			return TRUE;
		}
	};
}
