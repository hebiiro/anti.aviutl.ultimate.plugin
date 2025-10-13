#include "pch.h"
#include "resource.h"
#include "hive.hpp"
#include "utils.hpp"
#include "painter.hpp"
#include "container/container.hpp"
#include "container/spread_container.hpp"
#include "container/fixed_spread_container.hpp"
#include "container/scroll_container.hpp"
#include "shuttle/shuttle.hpp"
#include "shuttle/aviutl_window.hpp"
#include "shuttle/exedit_window.hpp"
#include "shuttle/setting_dialog.hpp"
#include "shuttle/player_window.hpp"
#include "shuttle/shuttle_manager.hpp"
#include "pane/tab.hpp"
#include "pane/tav.hpp"
#include "pane/drawer.hpp"
#include "pane/pane.hpp"
#include "pane/root_pane.hpp"
#include "pane/pane_config_dialog.hpp"
#include "pane/drawer_config_dialog.hpp"
#include "sub_process/sub_process.hpp"
#include "sub_process/console.hpp"
#include "sub_process/psdtoolkit.hpp"
#include "sub_process/sub_process_manager.hpp"
#include "dock/drawer_manager.hpp"
#include "dock/dock_site.hpp"
#include "dock/sub_window.hpp"
#include "dock/layout_list.hpp"
#include "dock/main_window.hpp"
#include "hook/entity.hpp"
#include "hook/manager.hpp"
#include "hook/win_hook/get_message.hpp"
#include "hook/win_event/move_cursor.hpp"
#include "hook/win_event/hide_window.hpp"
#include "hook/global/menu.hpp"
#include "hook/global/window.hpp"
#include "hook/global/window_pos.hpp"
#include "hook/local/aviutl.hpp"
#include "hook/local/exedit.hpp"
#include "hook/local/vsthost.hpp"
#include "hook/local/color_palette.hpp"
#include "hook/local/extoolbar.hpp"
#include "hook/local/z_ram_preview.hpp"
#include "hook/local/jumpbarx.hpp"
#include "hook/local/cutedit.hpp"
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
		set_logger(args);

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
