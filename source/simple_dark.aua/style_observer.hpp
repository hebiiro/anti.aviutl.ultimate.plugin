#pragma once

namespace apn::dark
{
	//
	// このクラスはスタイル関連ファイルを監視します。
	//
	inline struct style_observer_t : entry_t::node_t
	{
		//
		// スタイルファイルを監視します。
		//
		std::unique_ptr<my::FileWatcherBase> style_file_watcher;

		//
		// カスタムカラーファイルを監視します。
		//
		std::unique_ptr<my::FileWatcherBase> custom_color_file_watcher;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			// スタイルファイルの監視をリセットします。
			reset_style_file_watcher();

			// カスタムカラーファイルの監視をリセットします。
			reset_custom_color_file_watcher();

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
		// スタイルファイルを再読み込みします。
		//
		BOOL reload_style_file()
		{
			// スタイルファイルを読み込みます。
			if (!kuro::style.read_file(hive.dark.style_file_name.c_str())) return FALSE;

			// マテリアルをリロードします。
			if (!kuro::paint::manager.reload()) return FALSE;

			// すべてのウィンドウを再描画します。
			return redraw_windows();
		}

		//
		// スタイルファイルの監視をリセットします。
		//
		void reset_style_file_watcher()
		{
			style_file_watcher.reset(new my::FileWatcher(
				hive.dark.style_file_name.c_str(), hive.theme_window, [&]() { reload_style_file(); }));
		}

		//
		// スタイルファイルのパスをセットします。
		//
		BOOL set_style_file_name(const std::wstring& style_file_name)
		{
			// スタイルファイルの監視をリセットします。
			reset_style_file_watcher();

			// スタイルファイルを再読み込みします。
			return reload_style_file();
		}

		//
		// カスタムカラーファイルを再読み込みします。
		//
		BOOL reload_custom_color_file()
		{
			// カスタムカラーファイルを読み込みます。
			if (!kuro::custom_style.read_custom_color_file(hive.dark.custom_color_file_name.c_str())) return FALSE;

			// マテリアルをリロードします。
			if (!kuro::paint::manager.reload()) return FALSE;

			// すべてのウィンドウを再描画します。
			return redraw_windows();
		}

		//
		// カスタムカラーファイルの監視をリセットします。
		//
		void reset_custom_color_file_watcher()
		{
			custom_color_file_watcher.reset(new my::FileWatcher(
				hive.dark.custom_color_file_name.c_str(), hive.theme_window, [&]() { reload_custom_color_file(); }));
		}

		//
		// カスタムカラーファイルのパスをセットします。
		//
		BOOL set_custom_color_file_name(const std::wstring& custom_color_file_name)
		{
			// カスタムカラーファイルの監視をリセットします。
			reset_custom_color_file_watcher();

			// カスタムカラーファイルを再読み込みします。
			return reload_custom_color_file();
		}
	} style_observer;
}
