#pragma once

namespace apn::item_list
{
	//
	// このクラスはスクリプトをリストから選択できるようにします。
	//
	inline struct addin_t : apn::Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.k_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.k_display_name;
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			// ここでaviutlウィンドウを取得しておきます。
			hive.aviutl_window = magi.fp->hwnd_parent;

			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウの初期化を実行するときに呼ばれます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			// 予め使用予定のコンフィグフォルダを作成しておきます。
			magi.create_config_folders(get_addin_name(), {});

			if (!addin_window.init()) return FALSE;
			if (!config_io.init()) return FALSE;

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			config_io.write();

			config_io.exit();
			addin_window.exit();

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

					// アドインウィンドウを表示します。
					if (::IsWindow(addin_window)) addin_window.show();

					break;
				}
			}

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウプロシージャを実行するときに呼ばれます。
		//
		virtual BOOL on_window_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", message, w_param, l_param);

			switch (message)
			{
			case AviUtl::FilterPlugin::WindowMessage::FileClose:
				{
					// アイテム一覧をリセットします。
					app->reset_list();

					break;
				}
			}

			return FALSE;
		}

		//
		// この仮想関数は、映像フィルタを実行するときに呼び出されます。
		//
		virtual BOOL on_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state) override
		{
			// プロジェクトファイルを開いていない場合は何もしません。
			if (!fp->exfunc->is_editing(fpip->editp)) return FALSE;

			// 保存中または再生中の場合は何もしません。
			if (proc_state.is_saving || magi.auin.is_playing()) return FALSE;

			// アイテム一覧をリフレッシュします。
			app->refresh_list();

			return FALSE;
		}
	} addin;
}
