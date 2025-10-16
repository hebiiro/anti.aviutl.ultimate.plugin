#include "pch.h"
#include "resource.h"
#include "app_interface.hpp"
#include "addin_dialog_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "addin_dialog.hpp"
#include "addin_window.hpp"
#include "hook.hpp"
#include "config_io.hpp"
#include "app.hpp"
#include "addin.hpp"

namespace apn::slimming
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
