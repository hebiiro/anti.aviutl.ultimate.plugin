#pragma once

namespace apn::item_wave
{
	//
	// このクラスはタイムラインアイテム内に波形を表示します。
	//
	inline struct ItemWave : Addin
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

			if (!config_io.init()) return FALSE;
			if (!addin_window.init()) return FALSE;
			if (!share_manager.init()) return FALSE;
			if (!ui_process_manager.init()) return FALSE;
			if (!sub_thread_manager.init()) return FALSE;
			if (!hook_manager.init()) return FALSE;

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			::PostMessage(hive.ui_window, share::c_message.c_init_process, 0, 0);

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			config_io.write();

			::SendMessage(hive.ui_window, share::c_message.c_exit_process, 0, 0);

			hook_manager.exit();
			sub_thread_manager.exit();
			ui_process_manager.exit();
			share_manager.exit();
			addin_window.exit();
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
			MY_TRACE_FUNC("");

			return app->on_audio_proc(fp, fpip, proc_state);
		}
	} addin;
}
