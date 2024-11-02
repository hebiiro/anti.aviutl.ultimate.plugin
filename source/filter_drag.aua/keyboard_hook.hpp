#pragma once

namespace apn::filter_drag
{
	inline struct KeyboardHook
	{
		HHOOK hook = nullptr; // キーボードフックのハンドルです。
		BOOL use_shift = FALSE; // ドラッグ開始にshiftキーを必須にします。
		BOOL use_ctrl = FALSE; // ドラッグ開始にctrlキーを必須にします。
		BOOL use_alt = FALSE; // ドラッグ開始にaltキーを必須にします。
		BOOL use_win = FALSE; // ドラッグ開始にwinキーを必須にします。

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hook = ::SetWindowsHookEx(WH_KEYBOARD, hook_proc, nullptr, ::GetCurrentThreadId());

			return !!hook;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			::UnhookWindowsHookEx(hook), hook = nullptr;

			return TRUE;
		}

		//
		// 指定されたキーが押されていなかった場合はTRUEを返します。
		//
		inline static BOOL is_key_up(UINT vk)
		{
			return ::GetKeyState(vk) >= 0;
		}

		//
		// ドラッグ開始可能な場合はTRUEを返します。
		//
		BOOL can_start_drag()
		{
			if (use_shift && is_key_up(VK_SHIFT)) return FALSE;
			if (use_ctrl && is_key_up(VK_CONTROL)) return FALSE;
			if (use_alt && is_key_up(VK_MENU)) return FALSE;
			if (use_win && (is_key_up(VK_LWIN) && is_key_up(VK_RWIN))) return FALSE;

			return TRUE;
		}

		//
		// カーソルを変更します。
		//
		BOOL set_cursor(WPARAM wParam)
		{
			// 設定ダイアログを取得します。
			HWND hwnd = magi.exin.get_setting_dialog();

			// ::WindowFromPoint()の戻り値が設定ダイアログではない場合は何もしません。
			POINT pos; ::GetCursorPos(&pos);
			if (hwnd != ::WindowFromPoint(pos)) return FALSE;

			// 修飾キーに指定されたキーの状態が変更された場合はマウスカーソルを更新します。
			if ((use_shift && wParam == VK_SHIFT) ||
				(use_ctrl && wParam == VK_CONTROL) ||
				(use_alt && wParam == VK_MENU) ||
				(use_win && (wParam == VK_LWIN || wParam == VK_RWIN)))
			{
				::PostMessage(hwnd, WM_SETCURSOR, (WPARAM)hwnd, HTCLIENT);

				return TRUE;
			}

			return FALSE;
		}

		//
		// WH_KEYBOARDのフック関数です。
		//
		static LRESULT CALLBACK hook_proc(int code, WPARAM wParam, LPARAM lParam)
		{
			if (code != HC_ACTION)
				return ::CallNextHookEx(keyboard_hook.hook, code, wParam, lParam);

			keyboard_hook.set_cursor(wParam);

			return ::CallNextHookEx(keyboard_hook.hook, code, wParam, lParam);
		}
	} keyboard_hook;
}
