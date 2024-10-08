#pragma once

namespace apn::dialog_position
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

			my::hook::attach(DialogBoxParamA);

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
			inline static INT_PTR WINAPI hook_proc(HINSTANCE instance, LPCSTR template_name, HWND parent, DLGPROC dlg_proc, LPARAM init_param)
			{
				if ((DWORD)template_name <= 0x0000FFFF)
					MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}, {:#010x}", instance, template_name, parent, dlg_proc, init_param);
				else
					MY_TRACE_FUNC("{:#010x}, {}, {:#010x}, {:#010x}, {:#010x}", instance, template_name, parent, dlg_proc, init_param);

				// ダイアログをサブクラス化します。
				Dialog dialog;
				dialog.associator.start(&dialog);
				auto result = orig_proc(instance, template_name, parent, dlg_proc, init_param);
				dialog.associator.stop();

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = ::DialogBoxParamA;
		} DialogBoxParamA;
	} hook_manager;
}
