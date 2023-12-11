#pragma once
#include "Hive.h"

namespace fgo::dialog_size
{
	//
	// このクラスはフックを管理します。
	//
	inline struct Hook
	{
		//
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			return TRUE;
		}

		//
		// コンフィグファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			return TRUE;
		}

		//
		// 初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto user32 = ::GetModuleHandle(_T("user32.dll"));
			MY_TRACE_HEX(user32);

			Tools::attach(DialogBoxParamA, ::GetProcAddress(user32, "DialogBoxParamA"));
			Tools::attach(CreateDialogParamA, ::GetProcAddress(user32, "CreateDialogParamA"));

			auto comdlg32 = ::GetModuleHandle(_T("comdlg32.dll"));
			MY_TRACE_HEX(comdlg32);

			Tools::attach(GetOpenFileNameA, ::GetProcAddress(comdlg32, "GetOpenFileNameA"));
			Tools::attach(GetSaveFileNameA, ::GetProcAddress(comdlg32, "GetSaveFileNameA"));

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE(_T("APIフックに成功しました\n"));
			}
			else
			{
				MY_TRACE(_T("APIフックに失敗しました\n"));
			}

			return TRUE;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは::DialogBoxParamA()をフックします。
		//
		inline static struct {
			inline static INT_PTR WINAPI hook(HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogFunc, LPARAM initParam)
			{
				if ((DWORD)templateName <= 0x0000FFFF)
				{
					MY_TRACE_FUNC("0x%08X, %d", instance, templateName);
				}
				else
				{
					MY_TRACE_FUNC("0x%08X, %hs", instance, templateName);

					if (instance == ::GetModuleHandle(_T("exedit.auf")) ||
						instance == ::GetModuleHandle(0))
					{
						MY_TRACE(_T("ダイアログを置き換えます\n"));

						instance = hive.instance;
					}
				}

				return orig(instance, templateName, parent, dialogFunc, initParam);
			}
			inline static decltype(&hook) orig = 0;
		} DialogBoxParamA;

		//
		// このクラスは::CreateDialogParamA()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook(HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogFunc, LPARAM initParam)
			{
				if ((DWORD)templateName <= 0x0000FFFF)
				{
					MY_TRACE_FUNC("0x%08X, %d", instance, templateName);
				}
				else
				{
					MY_TRACE_FUNC("0x%08X, %hs", instance, templateName);

					if (instance == ::GetModuleHandle(_T("exedit.auf")) ||
						instance == ::GetModuleHandle(0))
					{
						MY_TRACE(_T("ダイアログを置き換えます\n"));

						instance = hive.instance;
					}
				}

				return orig(instance, templateName, parent, dialogFunc, initParam);
			}
			inline static decltype(&hook) orig = 0;
		} CreateDialogParamA;

		//
		// このクラスは::GetOpenFileNameA()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook(LPOPENFILENAMEA ofn)
			{
				MY_TRACE_FUNC("");

				if (ofn->hInstance == ::GetModuleHandle(0))
				{
					MY_TRACE(_T("ダイアログを置き換えます\n"));

					ofn->hInstance = hive.instance;
				}

				return orig(ofn);
			}
			inline static decltype(&hook) orig = 0;
		} GetOpenFileNameA;

		//
		// このクラスは::GetSaveFileNameA()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook(LPOPENFILENAMEA ofn)
			{
				MY_TRACE_FUNC("");

				if (ofn->hInstance == ::GetModuleHandle(0))
				{
					MY_TRACE(_T("ダイアログを置き換えます\n"));

					ofn->hInstance = hive.instance;
				}

				return orig(ofn);
			}
			inline static decltype(&hook) orig = 0;
		} GetSaveFileNameA;
	} hook;
}
