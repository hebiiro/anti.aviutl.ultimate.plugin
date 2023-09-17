#pragma once
#include "Checker.h"

namespace fgo::encode_guard
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

			// AviUtlのモジュールハンドルを取得します。
			auto aviutl = (uintptr_t)::GetModuleHandle(0);

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach(Encode, aviutl + 0x01A1A0);

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
		// このクラスはAviUtl内のEncode()の呼び出しをフックします。
		//
		inline static struct Encode_Type {
			inline static BOOL __fastcall hook(AviUtl::EditHandle* editp, uint32_t flags)
			{
				// 「編集RAMプレビュー」の場合はデフォルトの処理を行う。
				if (flags == 0x10 && strcmp(editp->sav_3.name, "編集RAMプレビュー") == 0)
					return orig(editp, flags);

				// チェックに必要な必要な変数を取得する。
				// ただし、Checker のコンストラクタでも必要な変数を取得しているので、
				// 全部コンストラクタでやってもいいかもしれない。
				int32_t s = 0, e = 0;
				magi.fp->exfunc->get_select_frame(editp, &s, &e);

				{
					// 動画を出力する前に設定が適正かどうかチェックする。

					Checker checker(magi.fp, s, e, editp); // チェックに必要な変数を取得する。

					if (!checker.check()) // この中でチェックしている。
						return FALSE; // 動画の出力をキャンセルする。
				}

				// 通常通りの動画の出力を行う。
				// 本来なら __fastcall で呼んだあと __cdecl で返さなければならない。
				// しかし、引数が 2 個以下なら実質 0 個でそのまま返せるのでそうしている。
				return orig(editp, flags);
			}
			inline static decltype(&hook) orig = 0;
		} Encode;
	} hook;
}
