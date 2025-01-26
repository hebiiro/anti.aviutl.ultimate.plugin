#pragma once

namespace apn::workspace::hook::win_event
{
	//
	// このクラスはウィンドウ非表示イベントをフックします。
	//
	inline struct HideWindow : Entity
	{
		my::win_event_hook::unique_ptr<> win_event_handle;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_dll_init() override
		{
			MY_TRACE_FUNC("");

			win_event_handle.reset(::SetWinEventHook(
				EVENT_OBJECT_HIDE,
				EVENT_OBJECT_HIDE,
				nullptr,
				hook_proc,
				::GetCurrentProcessId(),
				::GetCurrentThreadId(),
				WINEVENT_OUTOFCONTEXT));
			MY_TRACE_HEX(win_event_handle.get());

			return !!win_event_handle;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_dll_exit() override
		{
			MY_TRACE_FUNC("");

			win_event_handle.reset();

			return TRUE;
		}

		//
		// 指定されたウィンドウがフローティングウィンドウの場合はTRUEを返します。
		//
		inline static BOOL is_floating_container(HWND hwnd)
		{
			return Container::get_type(hwnd) == Container::c_type.c_float;
		}

		//
		// フックプロシージャです。
		//
		inline static void CALLBACK hook_proc(HWINEVENTHOOK hook, DWORD event,
			HWND hwnd, LONG object_id, LONG child_id, DWORD event_thread_id, DWORD event_time)
		{
			if (event == EVENT_OBJECT_HIDE && object_id == OBJID_WINDOW)
			{
				// ウィンドウがフローティングコンテナの場合は
				if (is_floating_container(hwnd))
				{
					// ドロワーマネージャに処理させます。
					drawer_manager.on_hide_floating_container(hwnd);
				}
			}
		}
	} hide_window;
}
