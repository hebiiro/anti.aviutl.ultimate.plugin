#pragma once

namespace apn::item_wave::ui
{
	//
	// このクラスは音量キャッシュです。
	//
	struct Cache
	{
		std::string file_name;
		std::vector<share::Volume> volumes;
	};

	//
	// このクラスは音量キャッシュを管理します。
	//
	struct CacheManager
	{
		//
		// 音量キャッシュのコレクションです。
		//
		std::map<std::string, std::shared_ptr<Cache>> collection;

		//
		// 指定されたファイルに対応した音量キャッシュを返します。
		//
		std::shared_ptr<Cache> get_cache(LPCSTR file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			auto it = collection.find(file_name);
			if (it == collection.end()) return nullptr;
			return it->second;
		}

		//
		// リーダーバッファから音量キャッシュを作成して返します。
		//
		std::shared_ptr<Cache> create_cache(const std::shared_ptr<Reader>& reader)
		{
			MY_TRACE_FUNC("");

			auto shared = reader->get_buffer();
			if (!shared) return nullptr;

			MY_TRACE_STR(shared->file_name);

			auto cache = std::make_shared<Cache>();
			cache->file_name = shared->file_name;
			cache->volumes.insert(cache->volumes.end(),
				shared->volumes, shared->volumes + shared->volume_count);
			collection[cache->file_name] = cache;
			return cache;
		}

		//
		// 音量キャッシュを消去します。
		//
		void clear()
		{
			MY_TRACE_FUNC("");

			collection.clear();
		}
	} cache_manager;
}
