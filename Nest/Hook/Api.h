#pragma once
#include "Api/Menu.h"
#include "Api/FindWindow.h"

namespace fgo::nest::hook
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

			Tools::attach(GetMenu);
			Tools::attach(SetMenu);
			Tools::attach(DrawMenuBar);
			Tools::attach(FindWindowA);
			Tools::attach(FindWindowW);
			Tools::attach(FindWindowExA);
			Tools::attach(GetWindow);
			Tools::attach(EnumThreadWindows);
			Tools::attach(EnumWindows);
			Tools::attach(SetWindowLongA);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE(_T("API フックに成功しました\n"));

				return TRUE;
			}
			else
			{
				MY_TRACE(_T("API フックに失敗しました\n"));

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
