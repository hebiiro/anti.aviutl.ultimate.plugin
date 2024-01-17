#pragma once
#include "Hive.h"

namespace fgo::namecage
{
	//
	// このクラスはフックを管理します。
	//
	inline struct Hook
	{
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

			Tools::attach_abs_call(ExtTextOutA, exedit + 0x0003794B);

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
		// このクラスは拡張編集内の::ExtTextOutA()の呼び出しをフックします。
		//
		inline static struct ExtTextOutA_Type {
			inline static BOOL WINAPI hook(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
				MY_TRACE_FUNC("0x%08X, %d, %d, 0x%08X", dc, x, y, options);
				MY_TRACE_RECT2(rc[0]); // クリッピング矩形です。
				MY_TRACE_RECT2(rc[1]); // アイテム全体の矩形です。

				// 70は拡張編集内の定数です。
				// それにユーザーが指定できるオフセットを加えます。
				x = std::max<int>(70 + hive.offset, x);

				return orig(dc, x, y, options, rc, text, c, dx);
			}
			inline static decltype(&hook) orig = ::ExtTextOutA;
		} ExtTextOutA;
	} hook;
}
