﻿#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはマウスカーソル移動イベントをフックします。
	//
	inline struct MoveCursor
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
		inline static void CALLBACK hook_proc(HWINEVENTHOOK hook, DWORD event,
			HWND hwnd, LONG object_id, LONG child_id, DWORD event_thread_id, DWORD event_time)
		{
			if (event == EVENT_OBJECT_LOCATIONCHANGE && object_id == OBJID_CURSOR)
			{
				// マウスカーソルの座標を取得します。
				auto point = my::get_cursor_pos();
//				MY_TRACE_POINT(point);

				// マウスカーソルの下にあるウィンドウを取得します。
				auto hwnd = ::WindowFromPoint(point);
//				MY_TRACE_HWND(hwnd);

				// ウィンドウのプロセスIDを取得します。
				auto pid = DWORD {};
				auto tid = ::GetWindowThreadProcessId(hwnd, &pid);
//				MY_TRACE_INT(pid);

				// プロセス外のウィンドウの場合は何もしません。
				if (pid != ::GetCurrentProcessId())
				{
//					MY_TRACE("プロセス外ウィンドウなので無視します\n");

					return;
				}

				// ウィンドウがタブコントロールの場合は
				if (auto pane = Pane::get_pane(hwnd))
				{
//					MY_TRACE_HEX(pane);

					// そのタブコントロールに処理させます。
					pane->on_global_mouse_move(hwnd, point);
				}
				// ウィンドウがタブコントロールではない場合は
				else
				{
					// すべてのタブコントロールに処理させます。
					Tav::process_global_mouse_move(point);
				}
			}
		}
	} move_cursor;
}
