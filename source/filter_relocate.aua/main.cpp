#include "pch.h"
#include "resource.h"
#include "model/property.hpp"
#include "view/main_dialog.hpp"
#include "view/main_window.hpp"
#include "controller/hook/entry.hpp"
#include "controller/hook/manager.hpp"
#include "controller/hook/entry/exedit.hpp"
#include "controller/config_io.hpp"
#include "controller/ini_file.hpp"
#include "controller/path.hpp"
#include "controller/exo.hpp"
#include "controller/action/base.hpp"
#include "controller/action/split_item_below.hpp"
#include "controller/action/split_item_above.hpp"
#include "controller/app.hpp"
#include "addin.hpp"

namespace apn::filter_relocate
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

				::DisableThreadLibraryCalls(model::property.instance = instance);

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
