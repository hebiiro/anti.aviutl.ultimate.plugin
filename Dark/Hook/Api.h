#pragma once
#include "Api/CreateWindow.h"

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

			HMODULE user32 = ::GetModuleHandle(_T("user32.dll"));

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach(CreateWindowExA);
			Tools::attach(CreateWindowExW);
			Tools::attach(CreateDialogParamA);
			Tools::attach(CreateDialogParamW);
			Tools::attach(CreateDialogIndirectParamA);
			Tools::attach(CreateDialogIndirectParamW);

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
