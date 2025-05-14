#pragma once

namespace apn::item_wave
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface, FileCacheManager::Listener
	{
		//
		// コンストラクタです。
		//
		App() { app = this; }

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			if (!config_io.init()) return FALSE;
			if (!addin_window.init()) return FALSE;
			if (!file_cache_manager.init(this)) return FALSE;
			if (!item_cache_manager.init()) return FALSE;
			if (!hook_manager.init()) return FALSE;

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			config_io.write();

			hook_manager.exit();
			item_cache_manager.exit();
			file_cache_manager.exit();
			addin_window.exit();
			config_io.exit();

			return TRUE;
		}

		//
		// すべてのキャッシュを消去します。
		//
		virtual BOOL clear_caches() override
		{
			MY_TRACE_FUNC("");

			// すべてのキャッシュを消去します。
			file_cache_manager.clear();
			item_cache_manager.clear();

			magi.exin.invalidate();

			return TRUE;
		}

		//
		// この仮想関数は、音声処理を実行するときに呼ばれます。
		//
		virtual BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const Addin::ProcState& proc_state) override
		{
			MY_TRACE_FUNC("");

			// 編集中ではないときは何もしません。
			if (!fp->exfunc->is_editing(fpip->editp)) return FALSE;

			// 音声を保存するときは何もしません。
			if (fp->exfunc->is_saving(fpip->editp)) return FALSE;

			// 現在のファイル情報を取得しておきます。
			fp->exfunc->get_file_info(fpip->editp, &hive.current_fi);

			switch (hive.update_mode)
			{
			case hive.c_update_mode.c_off:
				{
					return FALSE; // キャッシュの更新が無効化されている場合は何もしません。
				}
			case hive.c_update_mode.c_on_without_playing:
				{
					// 再生中の場合は何もしません。
					if (proc_state.is_playing) return FALSE;

					break;
				}
			}

			// 不要なアイテムキャッシュを削除します。
			item_cache_manager.clean();

			// アイテムキャッシュを更新します。
			item_cache_manager.update();

			return FALSE;
		}

		//
		// この仮想関数は、ファイルキャッシュが更新されたときに呼ばれます。
		//
		virtual BOOL on_file_cache_changed(const std::filesystem::path& file_name) override
		{
			MY_TRACE_FUNC("{/}", file_name);

			// アイテムキャッシュを更新します。
			item_cache_manager.update();

			return TRUE;
		}
	} app_impl;
}
