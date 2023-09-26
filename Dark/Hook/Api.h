#pragma once
#include "Api/CreateWindow.h"
#include "Api/GetModuleHandle.h"

namespace fgo::dark::hook
{
	//
	// このクラスはAPIをフックします。
	//
	inline static struct Api
	{
		//
		// APIフックの初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			HMODULE kernel32 = ::GetModuleHandle(_T("kernel32.dll"));
			MY_TRACE_HEX(kernel32);

			Tools::attach(CreateWindowExA);
			Tools::attach(CreateWindowExW);
			Tools::attach(CreateDialogParamA);
			Tools::attach(CreateDialogParamW);
			Tools::attach(CreateDialogIndirectParamA);
			Tools::attach(CreateDialogIndirectParamW);
#if 0
			Tools::attach(DialogBoxParamA);
			Tools::attach(DialogBoxParamW);
			Tools::attach(DialogBoxIndirectParamA);
			Tools::attach(DialogBoxIndirectParamW);
#endif
			Tools::attach(EnumProcessModules, ::GetProcAddress(kernel32, "K32EnumProcessModules"));

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE(_T("APIフックに成功しました\n"));

				return TRUE;
			}
			else
			{
				MY_TRACE(_T("APIフックに失敗しました\n"));

				return FALSE;
			}
		}

		//
		// APIフックの後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} api;
}
