#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスは音声を視覚化して表示します。
	//
	inline struct AudioVisualizer : Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.c_display_name;
		}

		//
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// コンフィグ入出力を初期化します。
			if (!config_io.init())
			{
				MY_TRACE("コンフィグ入出力の初期化に失敗しました\n");
			}

			// ワークスペース化アドインから補助関数をインポートします。
			if (!hive.workspace.init())
			{
				MY_TRACE("ワークスペース化アドインの初期化に失敗しました\n");
			}

			// アドインウィンドウを初期化します。
			if (!addin_window.init())
			{
				hive.message_box(L"アドインウィンドウの初期化に失敗しました\n");

				return FALSE;
			}

			// コウィンドウマネージャを初期化します。
			if (!co_window_manager.init())
			{
				hive.message_box(L"コウィンドウマネージャの初期化に失敗しました\n");

				return FALSE;
			}

			// 外部プロセスを初期化します。
			if (!outer.init())
			{
				hive.message_box(L"外部プロセスの初期化に失敗しました\n");

				return FALSE;
			}

			// ワーカーを初期化します。
			if (!worker.init())
			{
				hive.message_box(L"ワーカーの初期化に失敗しました\n");

				return FALSE;
			}

			// コンフィグを読み込みます。
			if (!app->read_config())
			{
				MY_TRACE("コンフィグの読み込みに失敗しました\n");
			}

			// 外部プロセスに初期化の完了を通知します。
			::PostMessage(hive.ui_window, share::c_message.c_init_process, 0, 0);

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// コンフィグを書き込みます。
			app->write_config();

			// 外部プロセスに終了処理中であることを通知します。
			::SendMessage(hive.ui_window, share::c_message.c_exit_process, 0, 0);

			// ワーカーを終了します。
			worker.exit();

			// 外部プロセスを終了します。
			outer.exit();

			// コウィンドウマネージャを終了します。
			co_window_manager.exit();

			// アドインウィンドウを終了します。
			addin_window.exit();

			// コンフィグ入出力を終了します。
			config_io.exit();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウコマンドを実行するときに呼ばれます。
		//
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
			{
			case magi.c_command_id.c_addin.c_command:
				{
					MY_TRACE_FUNC("magi.c_command_id.c_addin.c_command");

					// アドインウィンドウを表示します。
					if (::IsWindow(addin_window)) addin_window.show();

					break;
				}
			}

			return FALSE;
		}

		virtual BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state) override
		{
			MY_TRACE_FUNC("frame = {}, audio_n = {}", fpip->frame, fpip->audio_n);

			switch (hive.mode)
			{
			case hive.c_mode.c_off:
				{
					return FALSE; // 無効化されているときは何もしません。
				}
			case hive.c_mode.c_on_without_playing:
				{
					if (proc_state.is_playing)
						return FALSE; // 再生中のときは何もしません。

					break;
				}
			}

			if (proc_state.is_saving)
				return FALSE; // 保存中のときは何もしません。

			if (!co_window_manager.is_visible())
				return FALSE; // コウィンドウが表示されていないときは何もしません。

			// 音声信号を解析します。
			return worker.process_audio(fp, fpip, proc_state);
		}
	} addin;
}
