#pragma once
#include "GetParamSmallDialog.h"

namespace fgo::easing_select
{
	//
	// このクラスはフック処理を担当します。
	//
	struct Hook
	{
		//
		// コンストラクタです。
		//
		Hook()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のアドレスを取得します。
			auto exedit = magi.auin.GetExEdit();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach_import(DialogBoxParamA, (HMODULE)exedit, "DialogBoxParamA");

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE(_T("拡張編集のフックに成功しました\n"));
			}
			else
			{
				MY_TRACE(_T("拡張編集のフックに失敗しました\n"));
			}
		}

		//
		// デストラクタです。
		//
		~Hook()
		{
			MY_TRACE_FUNC("");
		}

		//
		// このクラスは拡張編集内の::DialogBoxParamA()の呼び出しをフックします。
		//
		inline static struct {
			inline static INT_PTR WINAPI hook(HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogProc, LPARAM initParam)
			{
				if ((DWORD)templateName > 0x0000FFFF)
				{
					if (strcmp(templateName, "GET_PARAM_SMALL") == 0)
					{
						MY_TRACE(_T("ダイアログをサブクラス化します\n"));

						GetParamSmallDialog dialog;
						dialog.associator.start(&dialog);
						INT_PTR result = orig(instance, templateName, parent, dialogProc, initParam);
						dialog.associator.stop();

						return result;
					}
				}

				return orig(instance, templateName, parent, dialogProc, initParam);
			}
			inline static decltype(&hook) orig = ::DialogBoxParamA;
		} DialogBoxParamA;
	};
}
