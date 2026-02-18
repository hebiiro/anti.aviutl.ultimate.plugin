#pragma once

namespace apn::timeline_map::hook
{
	//
	// このクラスは拡張編集ウィンドウをフックします。
	//
	inline struct exedit_window_t : my::Window
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

			my::hook::attach(redraw_exedit_window, magi.exin.address.function.redraw_exedit_window);

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
		// このクラスは拡張編集内のredraw_exedit_window()の呼び出しをフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(HWND exedit_window)
			{
				main_thread::view.redraw();

				return orig_proc(exedit_window);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} redraw_exedit_window;
	} exedit_window;
}
