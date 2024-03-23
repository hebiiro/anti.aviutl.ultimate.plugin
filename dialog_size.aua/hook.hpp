#pragma once

namespace apn::dialog_size
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto user32 = ::GetModuleHandle(_T("user32.dll"));
			MY_TRACE_HEX(user32);

			my::hook::attach(DialogBoxParamA, ::GetProcAddress(user32, "DialogBoxParamA"));
			my::hook::attach(CreateDialogParamA, ::GetProcAddress(user32, "CreateDialogParamA"));

			auto comdlg32 = ::GetModuleHandle(_T("comdlg32.dll"));
			MY_TRACE_HEX(comdlg32);

			my::hook::attach(GetOpenFileNameA, ::GetProcAddress(comdlg32, "GetOpenFileNameA"));
			my::hook::attach(GetSaveFileNameA, ::GetProcAddress(comdlg32, "GetSaveFileNameA"));

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("拡張編集のフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("拡張編集のフックに失敗しました\n");

				return FALSE;
			}
		}

		//
		// 後始末処理を実行します。
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
			inline static INT_PTR WINAPI hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dialogFunc, LPARAM init_param)
			{
				if ((DWORD)template_name <= 0x0000FFFF)
				{
					MY_TRACE_FUNC("{:#010x}, {}", instance, template_name);
				}
				else
				{
					MY_TRACE_FUNC("{:#010x}, {}", instance, template_name);

					if (instance == ::GetModuleHandle(_T("exedit.auf")) ||
						instance == ::GetModuleHandle(0))
					{
						MY_TRACE("ダイアログを置き換えます\n");

						instance = hive.instance;
					}
				}

				return orig_proc(instance, template_name, parent, dialogFunc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} DialogBoxParamA;

		//
		// このクラスは::CreateDialogParamA()をフックします。
		//
		inline static struct {
			inline static HWND WINAPI hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dialogFunc, LPARAM init_param)
			{
				if ((DWORD)template_name <= 0x0000FFFF)
				{
					MY_TRACE_FUNC("{:#010x}, {}", instance, template_name);
				}
				else
				{
					MY_TRACE_FUNC("{:#010x}, {}", instance, template_name);

					if (instance == ::GetModuleHandle(_T("exedit.auf")) ||
						instance == ::GetModuleHandle(0))
					{
						MY_TRACE("ダイアログを置き換えます\n");

						instance = hive.instance;
					}
				}

				return orig_proc(instance, template_name, parent, dialogFunc, init_param);
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} CreateDialogParamA;

		//
		// このクラスは::GetOpenFileNameA()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(LPOPENFILENAMEA ofn)
			{
				MY_TRACE_FUNC("");

				if (ofn->hInstance == ::GetModuleHandle(0))
				{
					MY_TRACE("ダイアログを置き換えます\n");

					ofn->hInstance = hive.instance;
				}

				return orig_proc(ofn);
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} GetOpenFileNameA;

		//
		// このクラスは::GetSaveFileNameA()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(LPOPENFILENAMEA ofn)
			{
				MY_TRACE_FUNC("");

				if (ofn->hInstance == ::GetModuleHandle(0))
				{
					MY_TRACE("ダイアログを置き換えます\n");

					ofn->hInstance = hive.instance;
				}

				return orig_proc(ofn);
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} GetSaveFileNameA;
	} hook_manager;
}
