#include "pch.h"
#include "resource.h"
#include "utils.hpp"
#include "hive.hpp"
#include "act_ctx.hpp"
#include "painter.hpp"
#include "double_buffer.hpp"
#include "skin/config.hpp"
#include "skin/stuff.hpp"
#include "skin/dwm.hpp"
#include "skin/icon.hpp"
#include "skin/theme/state.hpp"
#include "skin/theme/part.hpp"
#include "skin/theme/vsclass.hpp"
#include "skin/theme/manager.hpp"
#include "skin/manager.hpp"
#include "python.hpp"
#include "gdi/renderer.hpp"
#include "gdi/renderer_nc.hpp"
#include "gdi/manager.hpp"
#include "gdi/element/dialog.hpp"
#include "gdi/element/static.hpp"
#include "gdi/element/button.hpp"
#include "gdi/element/editbox.hpp"
#include "gdi/element/combobox.hpp"
#include "gdi/element/comboboxex.hpp"
#include "gdi/element/listbox.hpp"
#include "gdi/element/tooltip.hpp"
#include "gdi/element/trackbar.hpp"
#include "gdi/element/spin.hpp"
#include "gdi/element/header.hpp"
#include "gdi/element/listview.hpp"
#include "gdi/element/treeview.hpp"
#include "gdi/element/toolbar.hpp"
#include "gdi/element/tab.hpp"
#include "gdi/element/aviutl.hpp"
#include "gdi/element/aviutl_button.hpp"
#include "gdi/element/setting_dialog.hpp"
#include "gdi/client.hpp"
#include "theme/renderer.hpp"
#include "theme/manager.hpp"
#include "theme/element/menu.hpp"
#include "theme/element/scrollbar.hpp"
#include "theme/element/static.hpp"
#include "theme/element/button.hpp"
#include "theme/element/editbox.hpp"
#include "theme/element/combobox.hpp"
#include "theme/element/listbox.hpp"
#include "theme/element/tooltip.hpp"
#include "theme/element/trackbar.hpp"
#include "theme/element/spin.hpp"
#include "theme/element/header.hpp"
#include "theme/element/listview.hpp"
#include "theme/element/treeview.hpp"
#include "theme/element/toolbar.hpp"
#include "theme/element/tab.hpp"
#include "theme/element/command_module.hpp"
#include "theme/element/preview_pane.hpp"
#include "theme/element/common_items_dialog.hpp"
#include "theme/element/window.hpp"
#include "theme/client.hpp"
#include "hook/program/exedit.hpp"
#include "hook/program/loudness.hpp"
#include "hook/draw.hpp"
#include "hook/theme.hpp"
#include "hook/icon.hpp"
#include "hook/window.hpp"
#include "hook/comdlg.hpp"
#include "hook/program.hpp"
#include "hook/double_buffer.hpp"
#include "hook.hpp"
#include "addin_dialog.hpp"
#include "addin_window.hpp"
#include "config_io.hpp"
#include "assets_io.hpp"
#include "exports.hpp"
#include "app.hpp"
#include "addin.hpp"

namespace apn::dark
{
	//
	// エクスポート関数です。
	// このDLLで実装したアドインオブジェクトを返します。
	//
	Addin* WINAPI core_get_addin(LPCWSTR _args)
	{
#if defined(_DEBUG) && 1
		// カスタムロガーを設定します。
		static struct Logger : my::Tracer::Logger {
			virtual void output(LPCTSTR raw, LPCTSTR text) override {
				if (::GetKeyState(VK_F1) < 0) ::OutputDebugString(text);
			}
		} logger;
		my::Tracer::logger = &logger;
#endif
		my::case_insensitive_wstring args(_args);
		if (args.find(L"debug") == args.npos) my::Tracer::logger = nullptr;

		return &addin;
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

				if (!config_io.init()) return FALSE;
				if (!assets_io.init()) return FALSE;
				if (!act_ctx.init()) return FALSE;
				if (!hook_manager.on_pre_init()) return FALSE;

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				MY_TRACE_FUNC("DLL_PROCESS_DETACH");

				hook_manager.on_post_exit();
				act_ctx.exit();
				assets_io.exit();
				config_io.exit();

				my::tracer_to_file::exit();

				break;
			}
		}

		return TRUE;
	}

	struct {
		GdiplusStartupInput si;
		GdiplusStartupOutput so;
		ULONG_PTR token;
		ULONG_PTR hook_token;
	} gdiplus = {};

	//
	// エクスポート関数です。
	// 外部プロセスで使用する場合は最初にこの関数を呼び出してください。
	//
	BOOL WINAPI dark_init(HWND hwnd)
	{
		MY_TRACE_FUNC("{:#010x}", hwnd);

		gdiplus.si.SuppressBackgroundThread = TRUE;
		GdiplusStartup(&gdiplus.token, &gdiplus.si, &gdiplus.so);
		gdiplus.so.NotificationHook(&gdiplus.hook_token);

		hive.main_window = hwnd;
		hive.theme_window = hwnd;

		return hive.app->init();
	}

	//
	// エクスポート関数です。
	// 外部プロセスで使用する場合は最後にこの関数を呼び出してください。
	//
	BOOL WINAPI dark_exit()
	{
		MY_TRACE_FUNC("");

		gdiplus.so.NotificationUnhook(gdiplus.hook_token);
		GdiplusShutdown(gdiplus.token);

		return TRUE;
	}

	//
	// エクスポート関数です。
	// 現在のスキンのIDを返します。
	//
	UINT WINAPI dark_get_current_id()
	{
		return app.current_skin_id;
	}

	//
	// エクスポート関数です。
	// 指定された名前のスタッフを返します。
	// 存在しない場合はnullptrを返します。
	//
	const Dark::Stuff* WINAPI dark_get_stuff(LPCWSTR name)
	{
		MY_TRACE_FUNC("{}", name);

		return exports.stuff_manager.get_stuff(name);
	}

	//
	// エクスポート関数です。
	// 指定された名前のスタッフを設定します。
	//
	void WINAPI dark_set_stuff(LPCWSTR name, const Dark::Stuff* stuff)
	{
		MY_TRACE_FUNC("{}", name);

		exports.stuff_manager.set_stuff(name, (const share::Stuff*)stuff);
	}
}
