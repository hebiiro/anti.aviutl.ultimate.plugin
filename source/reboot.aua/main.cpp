#include "pch.h"
#include "resource.h"
#include "app_interface.hpp"
#include "addin_dialog_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "part.hpp"
#include "addin_dialog.hpp"
#include "addin_window.hpp"
#include "config_io.hpp"
#include "part/spreader/agit.hpp"
#include "part/spreader/player.hpp"
#include "part/spreader/hook.hpp"
#include "part/spreader/option_dialog.hpp"
#include "part/spreader/part.hpp"
#include "part/outputs/agit.hpp"
#include "part/outputs/part.hpp"
#include "part/manipulator/agit.hpp"
#include "part/manipulator/part.hpp"
#include "part/reloader/agit.hpp"
#include "part/reloader/part.hpp"
#include "part/launcher/agit.hpp"
#include "part/launcher/duty_option_dialog.hpp"
#include "part/launcher/part.hpp"
#include "app.hpp"
#include "addin.hpp"

namespace apn::reboot
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
