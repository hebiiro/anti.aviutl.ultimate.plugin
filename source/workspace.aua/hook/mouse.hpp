#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはWH_MOUSE_LLをフックします。
	//
	inline struct Mouse
	{
		my::win_event_hook::unique_ptr<> hook;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hook.reset(::SetWinEventHook(
				EVENT_OBJECT_LOCATIONCHANGE,
				EVENT_OBJECT_LOCATIONCHANGE,
				nullptr,
				hook_proc,
				0, 0,
				WINEVENT_OUTOFCONTEXT));
			MY_TRACE_HEX(hook.get());

			return !!hook;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			hook.reset();

			return TRUE;
		}

		//
		// フックプロシージャです。
		//
		static void CALLBACK hook_proc(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
			LONG object_id, LONG child_id, DWORD event_thread_id, DWORD event_time)
		{
			if (event == EVENT_OBJECT_LOCATIONCHANGE && object_id == OBJID_CURSOR)
			{
#if 1
				// マウスカーソルの座標を取得します。
				auto point = my::get_cursor_pos();
//				MY_TRACE_POINT(point);

				// マウスカーソルの下にあるウィンドウを取得します。
				auto hwnd = ::WindowFromPoint(point);
//				MY_TRACE_HWND(hwnd);

				// ウィンドウがタブコントロールの場合は
				if (auto pane = Pane::get_pane(hwnd))
				{
//					MY_TRACE_HEX(pane);

					// そのタブコントロールに処理させます。
					pane->tav.on_global_mouse_move(point);
				}
				// ウィンドウがタブコントロールではない場合は
				else
				{
					// すべてのタブコントロールに処理させます。
					Tav::process_global_mouse_move(point);
				}
#else
				long x = 0, y = 0, w = 0, h = 0;

				IAccessible* acc = nullptr;
				VARIANT child = {};
				auto hr = ::AccessibleObjectFromEvent(hwnd, object_id, child_id, &acc, &child);
				if (acc)
				{
					hr = acc->accLocation(&x, &y, &w, &h, child);

					acc->Release();
				}

				Tav::process_global_mouse_move(hwnd,
					SUCCEEDED(hr) ? POINT { x, y } : my::get_cursor_pos());
#endif
			}
		}
	} mouse;
}
