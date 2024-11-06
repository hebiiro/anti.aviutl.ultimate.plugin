#pragma once

namespace apn::clipboard_viewer
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

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.exin.get_exedit();
			if (!exedit) return FALSE;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach_abs_call(ExtTextOutA, exedit + 0x0003794B);

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
		// このクラスは拡張編集内の::ExtTextOutA()の呼び出しをフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCSTR text, UINT c, CONST INT* dx)
			{
				MY_TRACE_FUNC("{:#010x}, {}, {}, {:#010x}", dc, x, y, options);
				MY_TRACE_RECT2(rc[0]); // クリッピング矩形です。
				MY_TRACE_RECT2(rc[1]); // アイテム全体の矩形です。

				// 70は拡張編集内の定数です。
				// それにユーザーが指定できるオフセットを加えます。
				x = std::max<int>(70 + hive.offset, x);

				return orig_proc(dc, x, y, options, rc, text, c, dx);
			}
			inline static decltype(&hook_proc) orig_proc = ::ExtTextOutA;
		} ExtTextOutA;
	} hook_manager;
}
