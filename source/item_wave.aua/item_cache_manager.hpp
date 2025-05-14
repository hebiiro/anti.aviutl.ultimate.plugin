#pragma once

namespace apn::item_wave
{
	//
	// このクラスはアイテム毎のキャッシュを管理します。
	//
	inline struct ItemCacheManager
	{
		//
		// 動画ファイルフィルタのインデックスです。
		//
		inline static constexpr auto c_movie_file_filter_index = 0;

		//
		// 動画ファイルフィルタのIDです。
		//
		inline static constexpr auto c_movie_file_filter_id = 0;

		//
		// 音声ファイルフィルタのインデックスです。
		//
		inline static constexpr auto c_audio_file_filter_index = 0;

		//
		// 音声ファイルフィルタのIDです。
		//
		inline static constexpr auto c_audio_file_filter_id = 2;

		//
		// 標準再生フィルタのIDです。
		//
		inline static constexpr auto c_standard_play_filter_id = 12;

		//
		// アイテムキャッシュのコレクションです。
		// キーはオブジェクトのインデックスです。
		//
		std::map<int32_t, std::shared_ptr<ItemCache>> caches;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 指定されたアイテムに対応するアイテムキャシュを返します。
		//
		std::shared_ptr<ItemCache> get(int32_t object_index)
		{
//			MY_TRACE_FUNC("{/}", object_index);

			auto it = caches.find(object_index);
			if (it == caches.end()) return nullptr;
			return it->second;
		}

		//
		// 無効なアイテムキャッシュを削除します。
		//
		BOOL clean()
		{
			MY_TRACE_FUNC("");

			// 全てのアイテムキャッシュを走査します。
			for (auto it = caches.begin(); it != caches.end();)
			{
				// 音声パラメータを取得できる場合は
				if (auto audio_param = get_audio_params(it->first))
				{
					// 何もせずに次に進めます。
					it++;
				}
				// 音声パラメータを取得できない場合は
				else
				{
					// アイテムを取得します。
					auto object = magi.exin.get_object(it->first);

					// コレクションから削除して次に進めます。
					caches.erase(it++);

					// アイテムが配置されているレイヤーを再描画します。
					magi.exin.redraw_layer(object->layer_set);
				}
			}

			return TRUE;
		}

		//
		// すべてのアイテムキャッシュを更新します。
		//
		BOOL update()
		{
			MY_TRACE_FUNC("");

			// ソート済みアイテムを走査します。
			auto c = magi.exin.get_sorted_object_count();
			for (decltype(c) i = 0; i < c; i++)
			{
				// アイテムを取得します。
				auto object = magi.exin.get_sorted_object(i);

				// アイテムが無効の場合は何もしません。
				if (!(object->flag & ExEdit::Object::Flag::Exist)) continue;

				// アイテムが音声アイテムではない場合は何もしません。
				if (!(object->flag & ExEdit::Object::Flag::Sound)) continue;

				// アイテムのインデックスを取得します。
				auto object_index = magi.exin.get_object_index(object);

				// 既存のアイテムキャッシュを取得できた場合は
				if (auto cache = get(object_index))
				{
					// アイテムキャッシュが無効ではない場合は何もしません。
					if (!is_item_cache_invalid(cache, object_index)) continue;
				}

				// 音声パラメータを取得できた場合は
				if (auto audio_params = get_audio_params(object_index))
				{
					// このアイテムのキャッシュが要求されていない場合は何もしません。
					if (!is_item_cache_required(object, audio_params)) continue;

					// アイテムキャッシュを更新します。
					if (auto cache = update_item_cache(object_index, audio_params))
					{
						// アイテムが配置されているレイヤーを再描画します。
						magi.exin.redraw_layer(object->layer_set);
					}
				}
			}

			// ファイルキャッシュの作成を促します。
			file_cache_manager.yield();

			return TRUE;
		}

		//
		// 指定されたアイテムに対応するアイテムキャッシュを更新します。
		//
		std::shared_ptr<ItemCache> update_item_cache(int32_t object_index, const std::shared_ptr<AudioParams>& audio_params)
		{
			MY_TRACE_FUNC("{/}", object_index);

			// アイテムを取得します。
			auto object = magi.exin.get_object(object_index);

			// ファイルキャシュを取得します。
			auto file_cache = file_cache_manager.get(audio_params->file_name);
			if (!file_cache)
			{
				// ファイルキャッシュの作成を予約します。
				file_cache_manager.reserve(audio_params->file_name);

				return nullptr;
			}

			// アイテムキャッシュを作成します。
			auto item_cache = std::make_shared<ItemCache>();

			// アイテムキャッシュをコレクションに追加します。
			caches[object_index] = item_cache;

			// 音声パラメータを取得しておきます。
			item_cache->audio_params = audio_params;

			// 現在のフレームレートを算出します。
			auto fps = (double)hive.current_fi.video_rate / hive.current_fi.video_scale;

			// フレームレート変換倍率を算出します。
			auto scale = share::Volume::c_resolution / fps;

			// アイテムの全フレームを走査します。
			auto range = object->frame_end - object->frame_begin;
			for (decltype(range) i = 0; i <= range; i++)
			{
				// 音声パラメータからアイテムフレームを算出します。
				auto item_frame = (audio_params->play_speed * i) + (audio_params->play_begin * fps);

				// ファイルフレームを算出します。
				auto file_frame = (int32_t)(item_frame * scale);

				// ファイルフレームが無効の場合は
				if (file_frame >= (int32_t)file_cache->volumes.size())
					break; // 走査を終了します。

				// 現在のフレームレートの1フレーム毎の音量に変換します。
				auto volume = file_cache->volumes[file_frame];
				volume.mul(audio_params->volume);
				item_cache->volumes.emplace_back(volume);
			}

			return item_cache;
		}

		//
		// すべてのファイルキャッシュを消去します。
		//
		BOOL clear()
		{
			MY_TRACE_FUNC("");

			caches.clear();

			return TRUE;
		}

		//
		// 指定されたアイテムから音声パラメータを作成して返します。
		//
		inline static std::shared_ptr<AudioParams> get_audio_params(int32_t object_index)
		{
			// アイテムを取得します。
			auto object = magi.exin.get_object(object_index);

			auto audio_file_filter_index = magi.exin.get_filter_index(object, c_audio_file_filter_id); // 音声ファイル
			if (audio_file_filter_index == -1) return nullptr;

			auto standard_play_filter_index = magi.exin.get_filter_index(object, c_standard_play_filter_id); // 標準再生
			if (standard_play_filter_index == -1) return nullptr;

			auto volume_index = object->filter_param[standard_play_filter_index].track_begin + 0;
			auto pan_index = object->filter_param[standard_play_filter_index].track_begin + 1;
			auto play_begin_index = object->filter_param[audio_file_filter_index].track_begin + 0;
			auto play_speed_index = object->filter_param[audio_file_filter_index].track_begin + 1;

			auto audio_params = std::make_shared<AudioParams>();
			audio_params->id = (uint32_t)object_index;
			audio_params->flag = (uint32_t)object->flag;
			audio_params->frame_begin = object->frame_begin;
			audio_params->frame_end = object->frame_end;
			audio_params->scene_set = object->scene_set;
			audio_params->volume = object->track_value_left[volume_index] / 1000.0f;

			auto layer = magi.exin.get_layer_setting(object->layer_set);
			audio_params->layer_flag = (uint32_t)layer->flag;

			// 音声ファイルアイテムが動画ファイルアイテムと連携している場合は
			if (object->check_value[1])
			{
				// 音声ファイルアイテムと連携している動画ファイルアイテムを取得します。
				auto object2 = get_movie_file_item(object);
				if (!object2) return nullptr; // 取得できなかった場合はnullptrを返します。

				auto play_begin_index = object2->filter_param[c_movie_file_filter_index].track_begin + 0;
				auto play_speed_index = object2->filter_param[c_movie_file_filter_index].track_begin + 1;

				// 動画ファイルアイテムから拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efMovieFile*)magi.exin.get_exdata(object2, 0);

				auto play_begin = (float)(object2->track_value_left[play_begin_index] - 1);
				auto fps = (float)exdata->video_rate / exdata->video_scale;

				audio_params->file_name = std::filesystem::absolute(exdata->file);
				audio_params->play_begin = play_begin / fps;
				audio_params->play_speed = object2->track_value_left[play_speed_index] / 1000.0f;
			}
			else
			{
				// 音声ファイルアイテムから拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efAudioFile*)magi.exin.get_exdata(object, 0);

				audio_params->file_name = std::filesystem::absolute(exdata->file);
				audio_params->play_begin = object->track_value_left[play_begin_index] / 100.0f;
				audio_params->play_speed = object->track_value_left[play_speed_index] / 1000.0f;
			}

			return audio_params;
		}

		//
		// 音声ファイルアイテムと連携している動画ファイルアイテムを返します。
		//
		inline static ExEdit::Object* get_movie_file_item(ExEdit::Object* object)
		{
			auto result = (ExEdit::Object*)nullptr;
			auto distance = 100;

			// 同じ開始位置で最も近いレイヤーにある動画アイテムを探します。
			auto c = magi.exin.get_object_count();
			for (decltype(c) i = 0; i < c; i++)
			{
				// アイテムを取得します。
				auto object2 = magi.exin.get_object(i);

				// アイテムが無効の場合は無視します。
				if (!(object2->flag & ExEdit::Object::Flag::Exist)) continue;

				// シーンが異なるアイテムは無視します。
				if (object2->scene_set != object->scene_set) continue;

				// 開始位置が異なるアイテムは無視します。
				if (object2->frame_begin != object->frame_begin) continue;

				// レイヤー位置が下のアイテムは無視します。
				if (object2->layer_set >= object->layer_set) continue;

				// レイヤー位置が遠い場合は無視します。
				auto distance2 = object->layer_set - object2->layer_set;
				if (distance2 > distance) continue;

				// 音声ファイルアイテムの場合は
				if (object2->filter_param[c_audio_file_filter_index].id == c_audio_file_filter_id)
				{
					// この音声ファイルアイテムより下にある動画ファイルアイテムのみを連携対象とみなします。
					result = nullptr;
					distance = distance2;
				}
				else
				{
					// 動画ファイル以外のアイテムは無視します。
					if (object2->filter_param[c_movie_file_filter_index].id != c_movie_file_filter_id) continue;

					// この動画ファイルアイテムを連携対象とします。
					result = object2;
					distance = distance2;
				}
			}

			return result;
		}

		//
		// アイテムキャッシュが要求されている場合はTRUEを返します。
		//
		inline static BOOL is_item_cache_required(ExEdit::Object* object, const std::shared_ptr<AudioParams>& audio_params)
		{
			if (!hive.include_layers.empty())
			{
				// 対象レイヤーではない場合はFALSEを返します。
				if (!hive.include_layers.contains(object->layer_set))
					return FALSE;
			}

			if (!hive.exclude_layers.empty())
			{
				// 除外レイヤーの場合はFALSEを返します。
				if (hive.exclude_layers.contains(object->layer_set))
					return FALSE;
			}

			auto path = std::filesystem::path(audio_params->file_name);
			if (path.is_relative()) path = std::filesystem::current_path() / path;
			path = path.lexically_normal();

			if (!hive.include_folder.empty())
			{
				// 対象フォルダではない場合はFALSEを返します。
				if (!::StrStrIW(path.c_str(), hive.include_folder.c_str()))
					return FALSE;
			}

			if (!hive.exclude_folder.empty())
			{
				// 除外フォルダの場合はFALSEを返します。
				if (::StrStrIW(path.c_str(), hive.exclude_folder.c_str()))
					return FALSE;
			}

			return TRUE;
		}

		//
		// キャッシュが無効の場合はTRUEを返します。
		//
		inline static BOOL is_item_cache_invalid(const std::shared_ptr<ItemCache>& cache, int32_t object_index)
		{
			//
			// 指定されたパスが同一の場合はTRUEを返します。
			//
			const auto eq = [](const auto& lhs, const auto& rhs) -> BOOL
			{
				try
				{
					return std::filesystem::equivalent(lhs, rhs);
				}
				catch (...)
				{
				}

				return FALSE;
			};

			// 音声パラメータを取得できた場合は
			if (auto audio_params = get_audio_params(object_index))
			{
				// 音声パラメータが異なる場合はTRUEを返します。
				if (!eq(cache->audio_params->file_name, audio_params->file_name)) return TRUE;
				if (cache->audio_params->volume != audio_params->volume) return TRUE;
				if (cache->audio_params->play_begin != audio_params->play_begin) return TRUE;
				if (cache->audio_params->play_speed != audio_params->play_speed) return TRUE;
				if (cache->audio_params->frame_end != audio_params->frame_end) return TRUE;
				if (cache->audio_params->layer_flag != audio_params->layer_flag) return TRUE;
			}

			return FALSE;
		}
	} item_cache_manager;
}
