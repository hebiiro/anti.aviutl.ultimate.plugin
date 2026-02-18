#include "pch.h"
#include "resource.h"
#include "font_dialog.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "main_thread/property.hpp"
#include "main_thread/state.hpp"
#include "main_thread/context.hpp"
#include "main_thread/model.hpp"
#include "main_thread/paint_option_dialog.hpp"
#include "main_thread/view.hpp"
#include "main_thread/controller.hpp"
#include "hook/exedit_window.hpp"
#include "config_io.hpp"
#include "addin.hpp"

namespace apn::timeline_map
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
