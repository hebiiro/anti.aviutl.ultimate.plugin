#pragma once
#include "Api/CreateWindow.h"
#include "Api/MoveWindow.h"
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
			MY_TRACE(_T("Api::init()\n"));

			HMODULE user32 = ::GetModuleHandle(_T("user32.dll"));

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach(CreateWindowExA, ::GetProcAddress(user32, "CreateWindowExA"));
			Tools::attach(CreateWindowExW, ::GetProcAddress(user32, "CreateWindowExW"));
			Tools::attach(MoveWindow);
			Tools::attach(SetWindowPos);
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

				hive.true_CreateWindowExA = CreateWindowExA.orig;
				hive.true_SetWindowPos = SetWindowPos.orig;

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
			MY_TRACE(_T("Api::exit()\n"));

			return TRUE;
		}
	} api;
}
