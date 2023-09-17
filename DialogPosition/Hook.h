#pragma once
#include "Hive.h"
#include "Dialog.h"

namespace fgo::dialog_position
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

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.auin.GetExEdit();
			if (!exedit) return FALSE;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach(DialogBoxParamA);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE(_T("拡張編集のフックに成功しました\n"));
			}
			else
			{
				MY_TRACE(_T("拡張編集のフックに失敗しました\n"));
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
		// このクラスは拡張編集内の::DialogBoxParamA()の呼び出しをフックします。
		//
		inline static struct {
			inline static INT_PTR WINAPI hook(HINSTANCE instance, LPCSTR templateName, HWND parent, DLGPROC dialogProc, LPARAM initParam)
			{
				if ((DWORD)templateName <= 0x0000FFFF)
					MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X", instance, templateName, parent, dialogProc, initParam);
				else
					MY_TRACE_FUNC("0x%08X, %s, 0x%08X, 0x%08X, 0x%08X", instance, templateName, parent, dialogProc, initParam);

				// ダイアログをサブクラス化します。
				Dialog dialog;
				dialog.associator.start(&dialog);
				INT_PTR result = orig(instance, templateName, parent, dialogProc, initParam);
				dialog.associator.stop();

				return result;
			}
			inline static decltype(&hook) orig = ::DialogBoxParamA;
		} DialogBoxParamA;
	} hook;
}
