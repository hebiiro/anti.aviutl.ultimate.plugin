#pragma once

namespace apn::filter_drag
{
	//
	// このクラスは拡張編集の設定ダイアログにフィルタのドラッグ機能を追加します。
	//
	inline struct FilterDrag : Addin
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
			if (!aim_src.init()) return FALSE;
			if (!aim_dst.init()) return FALSE;
			if (!sight.init()) return FALSE;
			if (!hook_manager.init()) return FALSE;
			if (!keyboard_hook.init()) return FALSE;
			if (!config_checker.init()) return FALSE;

			aim_src.available = TRUE;
			aim_src.alpha = 100;
			aim_src.color = RGB(0x00, 0x00, 0xff);

			aim_dst.available = TRUE;
			aim_dst.alpha = 100;
			aim_dst.color = RGB(0xff, 0x00, 0x00);

			if (!config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末を実行するときに呼ばれます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			config_io.write();

			config_checker.exit();
			keyboard_hook.exit();
			hook_manager.exit();
			sight.exit();
			aim_dst.exit();
			aim_src.exit();
			config_io.exit();

			return FALSE;
		}
	} addin;
}
