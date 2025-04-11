#pragma once

namespace apn::item_copy
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

			// 誤作動の原因になるのでフックしないようにします。
			return TRUE;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(GetMessageA);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("APIフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("APIフックに失敗しました\n");

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
		// このクラスは拡張編集内の::GetMessageA()の呼び出しをフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(MSG* msg, HWND hwnd, UINT msg_filter_min, UINT msg_filter_max)
			{
				auto result = orig_proc(msg, hwnd, msg_filter_min, msg_filter_max);
				if (result > 0 && addin_dialog.shell_view &&
					S_OK == addin_dialog.shell_view->TranslateAcceleratorW(msg))
				{
					// シェルビューがメッセージを処理した場合は
					// メッセージを無効化して処理できないようにします。
					return *msg = {}, result;
				}
				return result;
			}
			inline static decltype(&hook_proc) orig_proc = ::GetMessageA;
		} GetMessageA;
	} hook_manager;
}
