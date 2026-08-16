#pragma once

namespace apn::dark
{
	//
	// このクラスはコンフィグ関連のファイルを監視します。
	//
	inline struct config_observer_t : entry_t::node_t
	{
		//
		// コンフィグファイルを監視します。
		//
		std::unique_ptr<my::FileWatcherBase> config_file_watcher;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			// コンフィグファイルの監視をリセットします。
			reset_config_file_watcher();

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグファイルを再読み込みします。
		//
		BOOL reload_config_file()
		{
			// コンフィグファイルを読み込みます。
			if (!config_io.read()) return FALSE;

			// コンフィグファイルが再読み込みされたことをアプリケーションに通知します。
			return app->on_reload();
		}

		//
		// コンフィグファイルの監視をリセットします。
		//
		void reset_config_file_watcher()
		{
			config_file_watcher.reset(new my::FileWatcher(
				hive.config_file_name.c_str(), hive.theme_window, [&]() { reload_config_file(); }));
		}
	} config_observer;
}
