#pragma once

namespace apn::item_wave
{
	//
	// このクラスはファイル毎のキャッシュです。
	//
	struct FileCache
	{
		int32_t video_scale;
		int32_t video_rate;
		std::vector<share::Volume> volumes;
	};

	//
	// このクラスはファイル毎のキャッシュを管理します。
	//
	inline struct FileCacheManager
	{
		//
		// キャッシュのコレクションです。
		//
		std::map<std::string, std::shared_ptr<FileCache>> collection;

		//
		// すべてのキャッシュを消去します。
		//
		void clear_caches()
		{
			collection.clear();
		}

		//
		// 指定されたファイルに対応したキャッシュを返します。
		// createがTRUEの場合、キャッシュが存在しない場合は作成して返します。
		//
		std::shared_ptr<FileCache> get_cache(LPCSTR file_name, BOOL create)
		{
			// ファイル名が無効の場合は
			if (strlen(file_name) == 0)
			{
				// 0を返します。
				return 0;
			}

			// キャッシュを検索します。
			auto it = collection.find(file_name);

			// キャッシュが見つかった場合は
			if (it != collection.end())
			{
				// キャッシュが有効の場合は
				if (it->second->video_scale == hive.fi.video_scale &&
					it->second->video_rate == hive.fi.video_rate)
				{
					return it->second; // キャッシュが見つかったので返します。
				}
			}

			// 作成フラグが指定されている場合は
			if (create)
			{
				// キャッシュが存在しないので作成します。
				// キャッシュの作成には時間がかかるので、ここで返すことはできません。
				create_cache(file_name);
			}

			// 0を返します。
			return 0;
		}

		//
		// 指定されたファイルに対応したキャッシュを作成します。
		//
		BOOL create_cache(LPCSTR file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			// キャッシュを作成します。
			return app->create_cache(file_name);
		}

		//
		// 作成されたキャッシュを受け取ります。
		//
		BOOL receive_cache()
		{
			MY_TRACE_FUNC("");

			// 共有メモリを取得します。
			auto shared = share_manager.main_buffer.get_buffer();
			if (!shared) return FALSE;

			MY_TRACE_STR(shared->wave_result.file_name);
			MY_TRACE_INT(shared->wave_result.volume_count);

			// キャッシュを更新します。
			auto cache = std::make_shared<FileCache>();
			cache->video_scale = hive.fi.video_scale;
			cache->video_rate = hive.fi.video_rate;
			cache->volumes.insert(cache->volumes.end(),
				shared->wave_result.volumes, shared->wave_result.volumes + shared->wave_result.volume_count);
			collection[shared->wave_result.file_name] = cache;

			return TRUE;
		}
	} file_cache_manager;
}
