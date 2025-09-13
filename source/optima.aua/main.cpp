#include "pch.h"
#include "resource.h"
#include "hive.hpp"
#include "utils.hpp"
#include "ult_combobox/utils.hpp"
#include "ult_combobox/node.hpp"
#include "ult_combobox/manager.hpp"
#include "ult_combobox/core.hpp"
#include "ult_combobox/popup/listbox.hpp"
#include "ult_combobox/drop_down_list/combobox.hpp"
#include "part/node_base.hpp"
#include "part/manager.hpp"
#include "part/node.hpp"
#include "part/node/create_window_hook.hpp"
#include "part/node/script_sort_patch.hpp"
#include "part/node/set_current_object.hpp"
#include "part/node/test.hpp"
#include "addin_dialog.hpp"
#include "addin_window.hpp"
#include "config_io.hpp"
#include "addin.hpp"

namespace apn::optima
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
