#include "pch.h"
#include "hive.hpp"
#include "model/property.hpp"
#include "model/state.hpp"
#include "model/context.hpp"
#include "view/listener/create_filter.hpp"
#include "view/listener/copy_filter.hpp"
#include "view/listener/paste_filter.hpp"
#include "view/setting_dialog.hpp"
#include "controller/command/create_filter.hpp"
#include "controller/command/copy_filter.hpp"
#include "controller/command/paste_filter.hpp"
#include "controller/hook.hpp"
#include "controller/config_io.hpp"
#include "controller/app.hpp"
#include "addin.hpp"

namespace apn::filter_copy
{
	//
	// エクスポート関数です。
	// このDLLで実装したアドインオブジェクトを返します。
	//
	Addin* WINAPI core_get_addin(LPCWSTR args)
	{
		if (!my::contains(args, L"debug")) my::Tracer::logger = nullptr;

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
