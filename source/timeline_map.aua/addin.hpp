#pragma once

namespace apn::timeline_map
{
	//
	// このクラスはaviutlにタイムラインの全体図を追加します。
	//
	inline struct timeline_map_t : Addin
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

			// フックを初期化します。
			if (!hook::exedit_window.init())
			{
				hive.message_box(L"フックの初期化に失敗しました\n");

				return FALSE;
			}

			// メインスレッドのコントローラーを初期化します。
			if (!main_thread::controller.init())
			{
				hive.message_box(L"メインスレッドのコントローラーの初期化に失敗しました\n");

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

			// メインスレッドのコントローラーを終了します。
			main_thread::controller.exit();

			// フックを終了します。
			hook::exedit_window.exit();

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
					main_thread::view.show();

					break;
				}
			}

			return FALSE;
		}

		virtual BOOL on_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state) override
		{
			MY_TRACE_FUNC("frame = {/}", fpip->frame);

			if (magi.auin.is_playing())
				return FALSE; // 再生中の場合は何もしません。

			if (proc_state.is_saving)
				return FALSE; // 保存中の場合は何もしません。

			return main_thread::view.redraw();
		}
	} addin;
}
