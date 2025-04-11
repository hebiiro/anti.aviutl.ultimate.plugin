#pragma once

namespace apn::dark::hook
{
	//
	// このクラスはコモンダイアログ関係のフックを担当します。
	//
	inline struct ComDlg
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto comdlg32 = ::GetModuleHandle(_T("comdlg32.dll"));
			MY_TRACE_HEX(comdlg32);

			my::hook::attach(GetOpenFileNameA, ::GetProcAddress(comdlg32, "GetOpenFileNameA"));
			my::hook::attach(GetOpenFileNameW, ::GetProcAddress(comdlg32, "GetOpenFileNameW"));
			my::hook::attach(GetSaveFileNameA, ::GetProcAddress(comdlg32, "GetSaveFileNameA"));
			my::hook::attach(GetSaveFileNameW, ::GetProcAddress(comdlg32, "GetSaveFileNameW"));

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

			return TRUE;
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
		// このクラスはレンダラを一時的にロックします。
		//
		struct RendererLocker
		{
			RendererLocker()
			{
				if (!hive.apply_file_dialog)
				{
					hive.renderer_locked = TRUE;
//					skin::manager.redraw();
				}
			}

			~RendererLocker()
			{
				if (!hive.apply_file_dialog)
				{
					hive.renderer_locked = FALSE;
					app->redraw();
				}
			}
		};

		//
		// このクラスは::GetOpenFileNameA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(LPOPENFILENAMEA ofn)
			{
				MY_TRACE_FUNC("{/hex}", ofn);

				RendererLocker locker;

				return orig_proc(ofn);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetOpenFileNameA;
		} GetOpenFileNameA;

		//
		// このクラスは::GetOpenFileNameW()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(LPOPENFILENAMEW ofn)
			{
				MY_TRACE_FUNC("{/hex}", ofn);

				RendererLocker locker;

				return orig_proc(ofn);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetOpenFileNameW;
		} GetOpenFileNameW;

		//
		// このクラスは::GetSaveFileNameA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(LPOPENFILENAMEA ofn)
			{
				MY_TRACE_FUNC("{/hex}", ofn);

				RendererLocker locker;

				return orig_proc(ofn);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetSaveFileNameA;
		} GetSaveFileNameA;

		//
		// このクラスは::GetSaveFileNameW()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(LPOPENFILENAMEW ofn)
			{
				MY_TRACE_FUNC("{/hex}", ofn);

				RendererLocker locker;

				return orig_proc(ofn);
			}
			inline static decltype(&hook_proc) orig_proc = ::GetSaveFileNameW;
		} GetSaveFileNameW;
	} comdlg;
}
