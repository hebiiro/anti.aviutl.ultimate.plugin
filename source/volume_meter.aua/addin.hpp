#pragma once

namespace apn::volume_meter
{
	//
	// このクラスは音声を視覚化して表示します。
	//
	inline struct volume_meter_t : Addin
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
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// コンフィグ入出力を初期化します。
			if (!config_io.init())
			{
				MY_TRACE("コンフィグ入出力の初期化に失敗しました\n");
			}

			// メインスレッドのコントローラーを初期化します。
			if (!main_thread::controller.init())
			{
				hive.message_box(L"メインスレッドのコントローラーの初期化に失敗しました\n");

				return FALSE;
			}

			// サブスレッドのコントローラーを初期化します。
			if (!sub_thread::controller.init(main_thread::view))
			{
				hive.message_box(L"サブスレッドのコントローラーの初期化に失敗しました\n");

				return FALSE;
			}

			// コンフィグを読み込みます。
			config_io.read();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// コンフィグを書き込みます。
			config_io.write();

			// サブスレッドのコントローラーを終了します。
			sub_thread::controller.exit();

			// メインスレッドのコントローラーを終了します。
			main_thread::controller.exit();

			// コンフィグ入出力を終了します。
			config_io.exit();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウコマンドを実行するときに呼ばれます。
		//
		virtual BOOL on_window_command(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (w_param)
			{
			case magi.c_command_id.c_addin.c_command:
				{
					MY_TRACE_FUNC("magi.c_command_id.c_addin.c_command");

					// ビューを表示します。
					if (::IsWindow(main_thread::view)) main_thread::view.show();

					break;
				}
			}

			return FALSE;
		}

		virtual BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state) override
		{
			MY_TRACE_FUNC("frame = {/}, audio_n = {/}", fpip->frame, fpip->audio_n);

			switch (hive.mode)
			{
			case hive.c_mode.c_off:
				{
					return FALSE; // 無効化されている場合は何もしません。
				}
			case hive.c_mode.c_on_without_playing:
				{
					if (magi.auin.is_playing())
						return FALSE; // 再生中の場合は何もしません。

					break;
				}
			}

			if (proc_state.is_saving)
				return FALSE; // 保存中の場合は何もしません。

			if (!::IsWindowVisible(main_thread::view))
				return FALSE; // ビューが非表示の場合は何もしません。

			// 音声信号を処理します。
			return sub_thread::controller.set_raw_audio_data(fp, fpip);
		}
	} addin;
}
