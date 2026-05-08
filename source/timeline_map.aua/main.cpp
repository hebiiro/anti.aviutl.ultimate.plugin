#include "pch.h"
#include "resource.h"
#include "utils.hpp"
#include "model/property.hpp"
#include "model/state.hpp"
#include "model/context.hpp"
#include "view/font_dialog.hpp"
#include "view/paint_option_dialog.hpp"
#include "view/overview.hpp"
#include "controller/hook/exedit_window.hpp"
#include "controller/config_io.hpp"
#include "controller/app.hpp"
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
