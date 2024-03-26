#pragma once

namespace apn::filter_copy
{
	//
	// このクラスは拡張編集の設定ダイアログに
	// フィルタのコピー・アンド・ペースト機能を追加します。
	//
	inline struct FilterCopy : Addin
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
		// この仮想関数は、ウィンドウの初期化のタイミングで呼び出されます。
		//
		virtual BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			if (!config_io.init()) return FALSE;
			if (!hook_manager.init()) return FALSE;
			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼び出されます。
		//
		virtual BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			config_io.exit();
			hook_manager.exit();
			return TRUE;
		}
	} addin;
}
