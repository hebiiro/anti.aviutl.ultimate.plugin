#include "pch.h"
#include "resource.h"
#include "hive.hpp"
#include "painter.hpp"
#include "container/container.hpp"
#include "container/spread_container.hpp"
#include "container/scroll_container.hpp"
#include "shuttle/shuttle.hpp"
#include "shuttle/aviutl_window.hpp"
#include "shuttle/exedit_window.hpp"
#include "shuttle/setting_dialog.hpp"
#include "shuttle/shuttle_manager.hpp"
#include "pane/tab.hpp"
#include "pane/pane.hpp"
#include "pane/root_pane.hpp"
#include "dock_site.hpp"
#include "sub_window.hpp"
#include "sub_process/sub_process.hpp"
#include "sub_process/psdtoolkit.hpp"
#include "sub_process/bouyomisan.hpp"
#include "sub_process/console.hpp"
#include "layout_list.hpp"
#include "main_window.hpp"
#include "hook/menu.hpp"
#include "hook/window.hpp"
#include "hook/get_message.hpp"
#include "hook/program/aviutl.hpp"
#include "hook/program/exedit.hpp"
#include "hook/program/vsthost.hpp"
#include "hook/program/color_palette.hpp"
#include "hook/program/extoolbar.hpp"
#include "hook/program/z_ram_preview.hpp"
#include "hook/hook.hpp"
#include "preference_loader.hpp"
#include "preference_saver.hpp"
#include "preference_io.hpp"
#include "config_io.hpp"
#include "app.hpp"
#include "addin.hpp"

namespace apn::workspace
{
	//
	// エクスポート関数です。
	// このDLLで実装したアドインオブジェクトを返します。
	//
	Addin* WINAPI core_get_addin(LPCWSTR args)
	{
		if (!::StrStrIW(args, L"debug")) my::Tracer::logger = 0;

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
				MY_TRACE_FUNC("DLL_PROCESS_ATTACH");

				::DisableThreadLibraryCalls(hive.instance = instance);

				break;
			}
		case DLL_PROCESS_DETACH:
			{
				MY_TRACE_FUNC("DLL_PROCESS_DETACH");

				break;
			}
		}

		return TRUE;
	}
}
