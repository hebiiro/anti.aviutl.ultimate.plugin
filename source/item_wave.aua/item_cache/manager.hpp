#pragma once

namespace apn::item_wave::item_cache
{
	//
	// このクラスはアイテム毎のキャッシュを管理します。
	//
	inline struct Manager : Core, FileCacheManager::Listener
	{
		//
		// アイテムキャッシュのコレクションです。
		// キーはオブジェクトのインデックスです。
		//
		std::map<int32_t, std::shared_ptr<Node>> caches;

		//
		// コンストラクタです。
		//
		Manager() { core = this; }

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return file_cache_manager.init(this);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return file_cache_manager.exit();
		}

		//
		// 無効なアイテムキャッシュを削除します。
		//
		BOOL clean(Context& ctx)
		{
			MY_TRACE_FUNC("");

			// 全てのアイテムキャッシュを走査します。
			for (auto it = caches.begin(); it != caches.end();)
			{
				// アイテムのインデックスを取得します。
				auto object_index = it->first;

				// アイテムキャッシュを取得します。
				auto& cache = it->second;

				// アイテムキャッシュが無効状態の場合は
				if (cache->is_invalid(object_index))
				{
					// アイテムが配置されているレイヤーを再描画します。
					ctx.add_redraw_layer(object_index);

					// コレクションから削除して次に進めます。
					caches.erase(it++);
				}
				// アイテムキャッシュが無効状態ではない場合は
				else
				{
					// 何もせずに次に進めます。
					it++;
				}
			}

			return TRUE;
		}

		//
		// カレントシーンのアイテムキャッシュを更新します。
		//
		BOOL refresh(Context& ctx)
		{
			auto current_scene_index = magi.exin.get_current_scene_index();

			// カレントシーンのアイテムキャッシュを更新します。
			return refresh_scene(ctx, current_scene_index);
		}

		//
		// アイテムキャッシュを準備します。
		//
		BOOL prepare(Context& ctx)
		{
			MY_TRACE_FUNC("");

			// 全てのアイテムキャッシュを走査します。
			for (const auto& pair : caches)
			{
				// アイテムのインデックスを取得します。
				auto object_index = pair.first;

				// アイテムキャッシュを取得します。
				const auto& cache = pair.second;

				// アイテムキャッシュを準備します。
				cache->prepare(ctx, object_index);
			}

			return TRUE;
		}

		//
		// 指定されたアイテムに対応するアイテムキャシュを返します。
		//
		virtual std::shared_ptr<Node> get(int32_t object_index) override
		{
//			MY_TRACE_FUNC("{/}", object_index);

			auto it = caches.find(object_index);
			if (it == caches.end()) return nullptr;
			return it->second;
		}

		//
		// 指定されたアイテムに対応するアイテムキャッシュを作成して返します。
		//
		virtual std::shared_ptr<Node> create(int32_t object_index) override
		{
//			MY_TRACE_FUNC("{/}", object_index);

			// アイテムを取得します。
			auto object = magi.exin.get_object(object_index);

			// 先頭のフィルタIDで分岐します。
			switch (object->filter_param[0].id)
			{
			case c_audio_file_filter_id: return caches[object_index] = std::make_shared<AudioFile>();
			case c_scene_audio_filter_id: return caches[object_index] = std::make_shared<SceneAudio>();
			}

			return nullptr;
		}

		//
		// 指定されたアイテムに対応するアイテムキャッシュを削除します。
		//
		virtual BOOL erase(int32_t object_index) override
		{
//			MY_TRACE_FUNC("{/}", object_index);

			return caches.erase(object_index) != 0;
		}

		//
		// 対象シーンのアイテムキャッシュを更新します。
		//
		virtual BOOL refresh_scene(Context& ctx, int32_t target_scene_index) override
		{
			// この関数の処理結果です。
			// 全ての音量を用意できた場合はTRUE、それ以外の場合はFALSEになります。
			auto result = TRUE;

			// 全てのアイテムを走査します。
			for (auto object_index : ctx.object_indexes)
			{
				// アイテムを取得します。
				auto object = magi.exin.get_object(object_index);

				// 対象シーン以外のアイテムは無視します。
				if (object->scene_set != target_scene_index) continue;

				// アイテムキャッシュを取得します。
				auto cache = get(object_index);

				// アイテムキャッシュを取得できなかった場合は
				if (!cache)
				{
					// アイテムキャッシュを新規作成します。
					cache = create(object_index);
				}

				// アイテムキャッシュが無効の場合は無視します。
				if (!cache) continue;

				// 対象アイテムではない場合は無視します。
				if (!is_target_object(object)) continue;

				// 音量を用意できなかった場合は
				if (!cache->refresh(ctx, object_index))
				{
					// 結果をFALSEにします。
					result = FALSE;
				}
			}

			// 結果を返します。
			return result;
		}

		//
		// アイテムキャッシュを更新します。
		//
		virtual BOOL refresh() override
		{
			// コンテキストを作成します。
			auto ctx = Context { magi.exin.get_object_indexes() };

			// 不要なアイテムキャッシュを削除します。
			clean(ctx);

			// カレントシーンのアイテムキャッシュを更新します。
			auto result = refresh(ctx);

			// 次回更新用に、アイテムキャッシュを準備します。
			prepare(ctx);

			// レイヤーを再描画します。
			magi.exin.redraw_layers(ctx.redraw_layers);

			// ファイルキャッシュの作成を促します。
			file_cache_manager.yield();

			return result;
		}

		//
		// すべてのアイテムキャッシュをリセットします。
		//
		virtual BOOL reset() override
		{
			MY_TRACE_FUNC("");

			if (caches.empty()) return FALSE;

			return caches.clear(), TRUE;
		}

		//
		// すべてのキャッシュを消去します。
		//
		virtual BOOL clear() override
		{
			MY_TRACE_FUNC("");

			file_cache_manager.clear();

			return reset();
		}

		//
		// この仮想関数は、ファイルキャッシュが更新されたときに呼ばれます。
		//
		virtual BOOL on_file_cache_changed(const std::filesystem::path& file_name) override
		{
			MY_TRACE_FUNC("{/}", file_name);

			// アイテムキャッシュを更新します。
			refresh();

			return TRUE;
		}
	} manager;
}
