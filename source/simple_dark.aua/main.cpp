#include "pch.h"
#include "resource.h"
#include "app_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "entry.hpp"
#include "act_ctx.hpp"
#include "kuro/color.hpp"
#include "kuro/style.hpp"
#include "kuro/custom_style.hpp"
#include "kuro/paint/utils.hpp"
#include "kuro/paint/pigment.hpp"
#include "kuro/paint/palette.hpp"
#include "kuro/paint/material.hpp"
#include "kuro/paint/d2d/core.hpp"
#include "kuro/paint/d2d/utils.hpp"
#include "kuro/paint/d2d/recter.hpp"
#include "kuro/paint/d2d/texter.hpp"
#include "kuro/paint/stylus.hpp"
#include "kuro/paint/material/sys_color.hpp"
#include "kuro/paint/material/window.hpp"
#include "kuro/paint/material/menu.hpp"
#include "kuro/paint/material/scrollbar.hpp"
#include "kuro/paint/material/dialog.hpp"
#include "kuro/paint/material/button.hpp"
#include "kuro/paint/material/editbox.hpp"
#include "kuro/paint/material/combobox.hpp"
#include "kuro/paint/material/listbox.hpp"
#include "kuro/paint/material/tooltip.hpp"
#include "kuro/paint/material/trackbar.hpp"
#include "kuro/paint/material/spin.hpp"
#include "kuro/paint/material/tab.hpp"
#include "kuro/paint/material/toolbar.hpp"
#include "kuro/paint/material/header.hpp"
#include "kuro/paint/material/listview.hpp"
#include "kuro/paint/material/treeview.hpp"
#include "kuro/paint/material/command_module.hpp"
#include "kuro/paint/material/address_band.hpp"
#include "kuro/paint/material/exedit.hpp"
#include "kuro/paint/manager.hpp"
#include "kuro/gdi/renderer.hpp"
#include "kuro/gdi/renderer_nc.hpp"
#include "kuro/gdi/renderer/menu.hpp"
#include "kuro/gdi/renderer/dialog.hpp"
#include "kuro/gdi/renderer/static.hpp"
#include "kuro/gdi/renderer/button.hpp"
#include "kuro/gdi/renderer/editbox.hpp"
#include "kuro/gdi/renderer/richedit.hpp"
#include "kuro/gdi/renderer/combobox.hpp"
#include "kuro/gdi/renderer/comboboxex.hpp"
#include "kuro/gdi/renderer/listbox.hpp"
#include "kuro/gdi/renderer/tooltip.hpp"
#include "kuro/gdi/renderer/trackbar.hpp"
#include "kuro/gdi/renderer/spin.hpp"
#include "kuro/gdi/renderer/tab.hpp"
#include "kuro/gdi/renderer/toolbar.hpp"
#include "kuro/gdi/renderer/header.hpp"
#include "kuro/gdi/renderer/listview.hpp"
#include "kuro/gdi/renderer/treeview.hpp"
#include "kuro/gdi/renderer/aviutl.hpp"
#include "kuro/gdi/renderer/aviutl_button.hpp"
#include "kuro/gdi/renderer/setting_dialog.hpp"
#include "kuro/gdi/renderer/comdlg32/dialog.hpp"
#include "kuro/gdi/renderer/comdlg32/direct_ui_hwnd.hpp"
#include "kuro/gdi/renderer/comdlg32/dui_view.hpp"
#include "kuro/gdi/renderer/rigaya/dialog.hpp"
#include "kuro/gdi/manager.hpp"
#include "kuro/theme/name.hpp"
#include "kuro/theme/renderer.hpp"
#include "kuro/theme/renderer/base/menu.hpp"
#include "kuro/theme/renderer/menu.hpp"
#include "kuro/theme/renderer/scrollbar.hpp"
#include "kuro/theme/renderer/static.hpp"
#include "kuro/theme/renderer/button.hpp"
#include "kuro/theme/renderer/editbox.hpp"
#include "kuro/theme/renderer/combobox.hpp"
#include "kuro/theme/renderer/listbox.hpp"
#include "kuro/theme/renderer/tooltip.hpp"
#include "kuro/theme/renderer/trackbar.hpp"
#include "kuro/theme/renderer/spin.hpp"
#include "kuro/theme/renderer/tab.hpp"
#include "kuro/theme/renderer/toolbar.hpp"
#include "kuro/theme/renderer/header.hpp"
#include "kuro/theme/renderer/listview.hpp"
#include "kuro/theme/renderer/treeview.hpp"
#include "kuro/theme/renderer/rebar.hpp"
#include "kuro/theme/renderer/window.hpp"
#include "kuro/theme/renderer/itemsview.hpp"
#include "kuro/theme/renderer/command_module.hpp"
#include "kuro/theme/renderer/common_items_dialog.hpp"
#include "kuro/theme/renderer/preview_pane.hpp"
#include "kuro/theme/renderer/explorer_nav_pane.hpp"
#include "kuro/theme/renderer/address_band.hpp"
#include "kuro/theme/renderer/search_band/search_box.hpp"
#include "kuro/theme/renderer/search_band/search_editbox.hpp"
#include "kuro/theme/renderer/immersive_start/menu.hpp"
#include "kuro/theme/from_handle.hpp"
#include "kuro/theme/from_vsclass.hpp"
#include "kuro/theme/manager.hpp"
#include "kuro/hook/entry/gdi.hpp"
#include "kuro/hook/entry/theme.hpp"
#include "kuro/hook/entry/icon.hpp"
#include "kuro/hook/entry/window.hpp"
#include "kuro/hook/entry/darken_window.hpp"
//#include "kuro/hook/entry/double_buffer.hpp"
#include "kuro/hook/entry/call_wnd_proc_ret.hpp"
#include "kuro/hook/entry/module/exedit.hpp"
#include "kuro/hook/entry/module/loudness.hpp"
#include "addin_dialog.hpp"
#include "addin_window.hpp"
#include "config_io.hpp"
#include "config_observer.hpp"
#include "style_observer.hpp"
#include "exedit_state.hpp"
#include "app.hpp"
#include "addin.hpp"

namespace apn::dark
{
	//
	// エクスポート関数です。
	// このDLLで実装したアドインオブジェクトを返します。
	//
	Addin* WINAPI core_get_addin(LPCWSTR args)
	{
		set_logger(args);

		// 黒窓のダミーを読み込みます。
		// 本物より先に読み込む必要があります。
		hive.darken_window = ::LoadLibrary(magi.get_module_file_name(_T("DarkenWindow.aul")).c_str());
		MY_TRACE_HEX(hive.darken_window);

		return &addin;
	}

	//
	// エクスポート関数です。
	// 外部プロセスで使用する場合は最初にこの関数を呼び出してください。
	//
	BOOL WINAPI dark_init(HWND hwnd)
	{
#ifdef _DEBUG
		// テスト用コードです。
		set_logger(L"debug f1");
#endif
		MY_TRACE_FUNC("{/hex}", hwnd);

		// 外部プロセスの場合は、ここでテーマウィンドウをセットします。
		hive.main_window = hwnd;
		hive.theme_window = hwnd;

		return app->on_init();
	}

	//
	// エクスポート関数です。
	// 外部プロセスで使用する場合は最後にこの関数を呼び出してください。
	//
	BOOL WINAPI dark_exit()
	{
		MY_TRACE_FUNC("");

		return app->on_exit();
	}

	//
	// エクスポート関数です。
	// 現在のスキンのIDを返します。
	//
	UINT WINAPI dark_get_current_id()
	{
		return 0;
	}

	//
	// エクスポート関数です。
	// 指定された名前のスタッフを返します。
	// 存在しない場合はnullptrを返します。
	//
	const Dark::Stuff* WINAPI dark_get_stuff(LPCWSTR name)
	{
		MY_TRACE_FUNC("{/}", name);

		return nullptr;
	}

	//
	// エクスポート関数です。
	// 指定された名前のスタッフを設定します。
	//
	void WINAPI dark_set_stuff(LPCWSTR name, const Dark::Stuff* stuff)
	{
		MY_TRACE_FUNC("{/}", name);
	}

	//
	// エントリポイントです。
	//
	EXTERN_C BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
	{
		switch (reason)
		{
		case DLL_PROCESS_ATTACH:
			{
				my::tracer_to_file::init(instance);

				MY_TRACE_FUNC("DLL_PROCESS_ATTACH");

				::DisableThreadLibraryCalls(hive.instance = instance);

				prior_entry.add(config_io);
				prior_entry.add(act_ctx);
				prior_entry.add(kuro::hook::gdi);
				prior_entry.add(kuro::hook::theme);
				prior_entry.add(kuro::hook::icon);
				prior_entry.add(kuro::hook::window);

				if (!prior_entry.init()) return FALSE;

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				MY_TRACE_FUNC("DLL_PROCESS_DETACH");

				prior_entry.exit();

				my::tracer_to_file::exit();

				break;
			}
		}

		return TRUE;
	}
}
