﻿#include "pch.h"
#include "resource.h"
#include "app_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "preview.hpp"
#include "favorite.hpp"
#include "recent.hpp"
#include "option_dialog.hpp"
#include "addin_dialog.hpp"
#include "addin_window.hpp"
#include "config_io.hpp"
#include "hook.hpp"
#include "app.hpp"
#include "addin.hpp"

namespace apn::font_tree
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
