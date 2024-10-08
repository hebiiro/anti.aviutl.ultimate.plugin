﻿#include "pch.h"
#include "hive.hpp"
#include "layout.hpp"
#include "aim.hpp"
#include "sight.hpp"
#include "keyboard_hook.hpp"
#include "hook.hpp"
#include "config_io.hpp"
#include "config_checker.hpp"
#include "addin.hpp"

namespace apn::filter_drag
{
	//
	// エクスポート関数です。
	// このDLLで実装したアドインオブジェクトを返します。
	//
	Addin* WINAPI core_get_addin(LPCWSTR args)
	{
		if (!::StrStrIW(args, L"debug")) my::Tracer::logger = 0;

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
