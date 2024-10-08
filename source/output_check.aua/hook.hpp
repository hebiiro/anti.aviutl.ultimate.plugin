#pragma once

namespace apn::output_check
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

			// AviUtlのモジュールハンドルを取得します。
			auto aviutl = (uintptr_t)::GetModuleHandle(0);

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(output, aviutl + 0x01A1A0);

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
		// このクラスはAviUtl内のoutput()の呼び出しをフックします。
		//
		inline static struct {
			inline static BOOL __fastcall hook_proc(AviUtl::EditHandle* editp, uint32_t flags)
			{
				// 「編集RAMプレビュー」の場合はデフォルトの処理を行います。
				if (flags == 0x10 && strcmp(editp->sav_3.name, "編集RAMプレビュー") == 0)
					return orig_proc(editp, flags);

				{
					// ここで動画を出力する前に設定が適正かどうかチェックします。

					Checker checker(magi.fp, editp); // この中でチェックに必要な変数を取得します。

					if (!checker.check()) // この中で設定をチェックしています。
						return FALSE; // チェックに引っかかった場合は動画の出力をキャンセルします。
				}

				// 通常通りの動画の出力を行います。
				// 本来なら、__fastcallで呼んで、__cdeclで返さないといけません。
				// しかし、引数が2個以下なら実質0個なので、スタックを調整する必要がありません。
				// よって、この関数の場合は__fastcallのまま返すことができます。
				return orig_proc(editp, flags);
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} output;
	} hook_manager;
}
