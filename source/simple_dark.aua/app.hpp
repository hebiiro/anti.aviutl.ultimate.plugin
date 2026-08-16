#pragma once

namespace apn::dark
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct app_t : app_interface_t
	{
		//
		// プラグインの初期化処理を実行します。
		//
		virtual BOOL on_initialize_plugin(DWORD version) override
		{
			MY_TRACE_FUNC("");

			// 内部プロセスの場合は、ここでテーマウィンドウをセットします。
			hive.theme_window = magi.exin.get_aviutl_window();

			// 初期化処理を実行します。
			if (!on_init()) return FALSE;

			// ホストプロセス専用エントリを構築します。
			host_process_entry.add(kuro::hook::exedit);
			host_process_entry.add(kuro::hook::loudness);
			host_process_entry.add(kuro::hook::darken_window);
			host_process_entry.add(addin_window);
			host_process_entry.add(exedit_state);

			// ホストプロセス専用の初期化処理を実行します。
			host_process_entry.init();

			// 新しいコンフィグで拡張編集などを更新します。
			return on_reload();
		}

		//
		// プラグインの後始末処理を実行します。
		//
		virtual BOOL on_uninitialize_plugin() override
		{
			MY_TRACE_FUNC("");

			// ホストプロセス専用の後始末処理を実行します。
			host_process_entry.exit();

			// 後始末処理を実行します。
			return on_exit();
		}

		//
		// 共用の初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			// 共有エントリを構築します。
			common_entry.add(kuro::style);
			common_entry.add(kuro::custom_style);
			common_entry.add(kuro::paint::manager);
			common_entry.add(kuro::gdi::manager);
			common_entry.add(kuro::theme::manager);
			common_entry.add(kuro::hook::cwpr);
			common_entry.add(config_observer);
			common_entry.add(style_observer);

			// 各種開始処理を実行します。
			common_entry.init();

			// コンフィグをファイルから読み込みます。
			config_io.read();

			// 初期化後メッセージをポストします。
			::PostMessage(hive.theme_window, hive.c_message.c_post_init, 0, 0);

#ifdef _DEBUG // テスト用コードです。
			{
				for (int i = 0; i < 50; i++)
					MY_TRACE("{/} => {/hex}\n", i, hive.orig.GetSysColor(i));

				auto window_color = hive.orig.GetSysColor(COLOR_WINDOW);
				auto button_color = hive.orig.GetSysColor(COLOR_BTNFACE);
				auto background_color = hive.orig.GetSysColor(COLOR_BACKGROUND);
				auto menu_color = hive.orig.GetSysColor(COLOR_MENU);
				auto menu_hilight_color = hive.orig.GetSysColor(COLOR_MENUHILIGHT);
				auto menu_bar_color = hive.orig.GetSysColor(COLOR_MENUBAR);
				auto hilight_color = hive.orig.GetSysColor(COLOR_HIGHLIGHT);
				auto button_hilight_color = hive.orig.GetSysColor(COLOR_BTNHIGHLIGHT);
				auto hot_light_color = hive.orig.GetSysColor(COLOR_HOTLIGHT);

				int break_point = 0; // ここでシステムカラーを確認します。
			}
#endif
			return TRUE;
		}

		//
		// 共用の後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			// 各種終了処理を実行します。
			common_entry.exit();

			return TRUE;
		}

		//
		// 初期化後処理を実行します。
		//
		virtual BOOL on_post_init() override
		{
			MY_TRACE_FUNC("");
#if 0
			::DrawMenuBar(hive.theme_window);
#endif
			return TRUE;
		}

		//
		// 主にダイアログのコントロールによって
		// コンフィグが変更された時に呼び出されます。
		//
		virtual BOOL on_update() override
		{
			MY_TRACE_FUNC("");

			// コンフィグをファイルに書き込みます。
			config_io.write();

			// 新しいコンフィグで拡張編集などを更新します。
			return on_reload();
		}

		//
		// 新しいコンフィグで拡張編集などを更新します。
		//
		virtual BOOL on_reload() override
		{
			MY_TRACE_FUNC("");

			// ホストプロセスの場合は
			if (addin_dialog)
			{
				// 拡張編集の状態を更新します。
				exedit_state.update();

				// コントロールを更新します。
				addin_dialog.update_controls();
			}

			// すべてのウィンドウを再描画します。
			return redraw_windows();
		}

		//
		// スタイルファイルのパスをセットします。
		//
		virtual BOOL on_set_style_file_name(const std::wstring& style_file_name) override
		{
			// スタイルファイルのパスをセットします。
			hive.dark.style_file_name = style_file_name;

			// ダイアログコントロールを更新します。
			addin_dialog.set_text(idc_dark_style_file_name, hive.dark.style_file_name);

			// スタイルファイルを再読み込みします。
			return style_observer.set_style_file_name(style_file_name);
		}

		//
		// カスタムカラーファイルのパスをセットします。
		//
		virtual BOOL on_set_custom_color_file_name(const std::wstring& custom_color_file_name) override
		{
			// カスタムカラーファイルのパスをセットします。
			hive.dark.custom_color_file_name = custom_color_file_name;

			// ダイアログコントロールを更新します。
//			config_dialog.set_text(IDC_JD_CUSTOM_COLOR_FILE_NAME, hive.dark.custom_color_file_name);

			// カスタムカラーファイルを再読み込みします。
			return style_observer.set_custom_color_file_name(custom_color_file_name);
		}
	} app_impl;
}
