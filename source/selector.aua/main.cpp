#include "pch.h"
#include "resource.h"
#include "app_interface.hpp"
#include "addin_dialog_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "etc/dragger.hpp"
#include "etc/drop_source.hpp"
#include "etc/selector_window.hpp"
#include "etc/list_view.hpp"
#include "etc/tree_view.hpp"
#include "part/part.hpp"
#include "part/custom_object/agit.hpp"
#include "part/custom_object/action_interface.hpp"
#include "part/custom_object/spec_reader.hpp"
#include "part/custom_object/exedit_reader.hpp"
#include "part/custom_object/embedded_tree_builder.hpp"
#include "part/custom_object/list_dialog.hpp"
#include "part/custom_object/list_window.hpp"
#include "part/custom_object/tree_node_config_dialog.hpp"
#include "part/custom_object/tree_dialog.hpp"
#include "part/custom_object/tree_window.hpp"
#include "part/custom_object/action.hpp"
#include "part/custom_object/part.hpp"
#include "addin_dialog.hpp"
#include "addin_window.hpp"
#include "config_io.hpp"
#include "app.hpp"
#include "addin.hpp"

namespace apn::selector
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
