#include "pch.h"
#include "resource.h"
#include "hive.hpp"
#include "utils.hpp"
#include "addin_dialog.hpp"
#include "addin_window.hpp"
#include "config_io.hpp"
#include "action/action.hpp"
#include "action/align_left.hpp"
#include "action/align_right.hpp"
#include "action/stretch_left.hpp"
#include "action/stretch_right.hpp"
#include "action/relative_space_left.hpp"
#include "action/relative_space_right.hpp"
#include "action/absolute_space_left.hpp"
#include "action/absolute_space_right.hpp"
#include "action/shift_down.hpp"
#include "action/shift_up.hpp"
#include "action/move_vert_down.hpp"
#include "action/move_vert_up.hpp"
#include "action/fix_bpm.hpp"
#include "action/erase_midpt.hpp"
#include "app.hpp"
#include "addin.hpp"

namespace apn::item_align
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
