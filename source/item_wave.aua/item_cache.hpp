#pragma once

namespace apn::item_wave
{
	//
	// このクラスはアイテム毎のキャッシュです。
	//
	struct ItemCache
	{
		std::vector<share::Volume> volumes;
		std::shared_ptr<share::Audio> audio;
	};

	//
	// このクラスはアイテム毎のキャッシュを管理します。
	// ファイルキャッシュ、サブスレッドに依存しています。
	//
	inline struct ItemCacheManager
	{
		//
		// キャッシュのコレクションです。
		// キーはオブジェクトのインデックスです。
		//
		std::map<int32_t, std::shared_ptr<ItemCache>> collection;

		//
		// ファイル名をノーマライズしてコピーします。
		//
		inline static void copy_file_name(LPSTR dst, size_t size, LPCSTR src)
		{
			// フルパスを取得します。
			::GetFullPathNameA(src, size, dst, nullptr);

			// 小文字に変換します。
			size_t length = strlen(dst);
			for (size_t  i = 0; i < length; i++)
			{
				if (::IsDBCSLeadByte(dst[i]))
					i++;
				else
					dst[i] = tolower(dst[i]);
			}
		}

		//
		// キャッシュが要求されている場合はTRUEを返します。
		//
		inline static BOOL is_cache_required(ExEdit::Object* object, const std::shared_ptr<share::Audio>& audio)
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

			std::filesystem::path path(audio->file_name);
			if (path.is_relative())
				path = std::filesystem::current_path() / path;
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
		// すべてのキャッシュを消去します。
		//
		void clear_caches()
		{
			collection.clear();
		}

		//
		// アイテムに対応するキャシュを返します。
		//
		std::shared_ptr<ItemCache> get_cache(int32_t object_index)
		{
			auto it = collection.find(object_index);
			if (it == collection.end()) return nullptr;
			return it->second;
		}

		//
		// すべてのアイテムキャッシュを更新します。
		//
		BOOL update(BOOL send)
		{
			MY_TRACE_FUNC("{}", send);

			// 無効なキャッシュを削除します。
			for (auto it = collection.begin(); it != collection.end();)
			{
				auto object = magi.exin.get_object(it->first);

				// アイテムが存在しない、または音声ではない場合は
				if (!(object->flag & ExEdit::Object::Flag::Exist) ||
					!(object->flag & ExEdit::Object::Flag::Sound))
				{
					// 音声パラメータのフラグを更新します。
					it->second->audio->flag = (uint32_t)object->flag;

					// コレクションから削除します。
					collection.erase(it++);
				}
				else
				{
					it++;
				}
			}

			// ソート済みアイテムを走査します。
			auto c = magi.exin.get_current_scene_object_count();
			for (decltype(c) i = 0; i < c; i++)
			{
				// アイテムを取得します。
				auto object = magi.exin.get_sorted_object(i);

				// アイテムが音声アイテムかチェックします。
				if (!(object->flag & ExEdit::Object::Flag::Sound))
					continue; // 音声波形を取得できるのは音声アイテムのみです。

				// アイテムのインデックスを取得します。
				auto object_index = magi.exin.get_object_index(object);

				// 作成済みのキャッシュを取得します。
				auto it = collection.find(object_index);

				// すでにキャッシュが作成済みの場合は
				if (it != collection.end())
				{
					auto cache = it->second;

					// キャッシュとアイテムのパラメータを比較します。
					if (!is_changed(cache, object))
						continue; // アイテムのパラメータに変化がないときは何もしません。
				}

				// キャッシュを更新します。
				if (auto cache = update(send, object_index, object))
				{
					// アイテムが配置されているレイヤーを再描画します。
					magi.exin.redraw_layer(object->layer_set);
				}
			}

			return TRUE;
		}

		//
		// 指定されたアイテムに対応するキャッシュを更新します。
		//
		std::shared_ptr<ItemCache> update(BOOL send, int32_t object_index, ExEdit::Object* object)
		{
			MY_TRACE_FUNC("{}, {}, {:#010x}", send, object_index, object);

			// 音声を取得します。
			auto audio = get_audio(object);
			if (!audio) return nullptr;

			// このアイテムのキャッシュが要求されているか確認します。
			if (!is_cache_required(object, audio)) return nullptr;

			// ファイルキャシュを取得します。
			auto file_cache = file_cache_manager.get_cache(audio->file_name, send);
			if (!file_cache) return nullptr; // ファイルキャッシュが作成されるまでは何もできません。

			// アイテムキャッシュを作成します。
			auto item_cache = std::make_shared<ItemCache>();
			collection[object_index] = item_cache;

			// アイテムのパラメータを保存しておきます。
			item_cache->audio = audio;

			// 現在のフレームレートを算出します。
			auto fps = (double)hive.fi.video_rate / hive.fi.video_scale;

			// フレームレート変換倍率を算出します。
			auto scale = share::Volume::c_resolution / fps;

			// アイテムの全フレームを走査します。
			auto range = object->frame_end - object->frame_begin;
			for (decltype(range) i = 0; i <= range; i++)
			{
				// アイテムの設定から音声のフレーム番号を算出します。
				auto dst_frame = (audio->play_speed * i) + (audio->play_begin * fps);

				// ソースフレームを算出します。
				auto src_frame = (int32_t)(dst_frame * scale);

				// ソースフレームが無効の場合は
				if (src_frame >= (int32_t)file_cache->volumes.size())
					break; // 走査を終了します。

				// 現在のフレームレートの1フレーム毎の音量に変換します。
				auto volume = file_cache->volumes[src_frame];
				volume.level *= audio->volume;
				item_cache->volumes.emplace_back(volume);
			}

			return item_cache;
		}

		//
		// 指定されたアイテムから音声アイテムパラメータを作成して返します。
		//
		std::shared_ptr<share::Audio> get_audio(ExEdit::Object* object)
		{
			auto audio_filer_index = magi.exin.get_filter_index(object, 2); // 音声ファイル
			if (audio_filer_index == -1) return nullptr;

			auto standard_play_index = magi.exin.get_filter_index(object, 12); // 標準再生
			if (standard_play_index == -1) return nullptr;

			auto volume_index = object->filter_param[standard_play_index].track_begin + 0;
			auto pan_index = object->filter_param[standard_play_index].track_begin + 1;
			auto play_begin_index = object->filter_param[audio_filer_index].track_begin + 0;
			auto play_speed_index = object->filter_param[audio_filer_index].track_begin + 1;

			auto audio = std::make_shared<share::Audio>();
			audio->id = (uint32_t)object;
			audio->flag = (uint32_t)object->flag;
			audio->frame_begin = object->frame_begin;
			audio->frame_end = object->frame_end;
			audio->scene_set = object->scene_set;
			audio->volume = object->track_value_left[volume_index] / 1000.0f;

			auto layer = magi.exin.get_layer_setting(object->layer_set);
			audio->layer_flag = (uint32_t)layer->flag;

			// 音声アイテムが動画アイテムと連携している場合は
			if (object->check_value[1])
			{
				// 同じグループの動画アイテムを探します。
				auto c = magi.exin.get_object_count();
				for (decltype(c) i = 0; i < c; i++)
				{
					auto object2 = magi.exin.get_object(i);
					if (!object2) continue;
					if (object2->group_belong != object->group_belong) continue;
					if (object2->scene_set != object->scene_set) continue;
					if (object2->exdata_size != 288) continue;

					auto movie_file_index = magi.exin.get_filter_index(object2, 0); // 動画ファイル
					if (movie_file_index == -1) continue;

					auto play_begin_index = object2->filter_param[movie_file_index].track_begin + 0;
					auto play_speed_index = object2->filter_param[movie_file_index].track_begin + 1;

					// 動画アイテムから拡張データを取得します。
					auto exdata = (ExEdit::Exdata::efMovieFile*)magi.exin.get_exdata(object2, 0);

					auto play_begin = (float)(object2->track_value_left[play_begin_index] - 1);
					auto fps = (float)exdata->video_rate / exdata->video_scale;

					copy_file_name(audio->file_name, sizeof(audio->file_name), exdata->file);
					audio->play_begin = play_begin / fps;
					audio->play_speed = object2->track_value_left[play_speed_index] / 1000.0f;

					break;
				}
			}
			else
			{
				// 音声アイテムから拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efAudioFile*)magi.exin.get_exdata(object, 0);

				copy_file_name(audio->file_name, sizeof(audio->file_name), exdata->file);
				audio->play_begin = object->track_value_left[play_begin_index] / 100.0f;
				audio->play_speed = object->track_value_left[play_speed_index] / 1000.0f;
			}

			return audio;
		}

		//
		// キャッシュが古くなっていて更新が必要な場合はTRUEを返します。
		//
		BOOL is_changed(const std::shared_ptr<ItemCache>& cache, ExEdit::Object* object)
		{
			auto audio = get_audio(object);
			if (!audio) return FALSE; // アイテムから音声設定を取得できない場合はFALSEを返します。

			if (strcmp(cache->audio->file_name, audio->file_name) != 0) return TRUE;
			if (cache->audio->volume != audio->volume) return TRUE;
			if (cache->audio->play_begin != audio->play_begin) return TRUE;
			if (cache->audio->play_speed != audio->play_speed) return TRUE;
			if (cache->audio->frame_end != audio->frame_end) return TRUE;
			if (cache->audio->layer_flag != audio->layer_flag) return TRUE;

			return FALSE;
		}
	} item_cache_manager;
}
