#pragma once

namespace fgo::filter_drag
{
	inline struct KeyboardHook
	{
		HHOOK hook = 0; // キーボードフックのハンドルです。
		BOOL useShiftKey = FALSE; // ドラッグ開始に Shift キーを必須にします。
		BOOL useCtrlKey = FALSE; // ドラッグ開始に Ctrl キーを必須にします。
		BOOL useAltKey = FALSE; // ドラッグ開始に Alt キーを必須にします。
		BOOL useWinKey = FALSE; // ドラッグ開始に Win キーを必須にします。

		void init()
		{
			hook = ::SetWindowsHookEx(WH_KEYBOARD, hookProc, 0, ::GetCurrentThreadId());
		}

		void exit()
		{
			::UnhookWindowsHookEx(hook), hook = 0;
		}

		//
		// vk が押されていなかった場合は TRUE を返す。
		//
		static BOOL isKeyUp(UINT vk)
		{
			return ::GetKeyState(vk) >= 0;
		}

		//
		// 修飾キーが設定されているのにそのキーが押されていなかった場合は FALSE を返す。
		//
		BOOL canBeginDrag()
		{
			if (useShiftKey && isKeyUp(VK_SHIFT)) return FALSE;
			if (useCtrlKey && isKeyUp(VK_CONTROL)) return FALSE;
			if (useAltKey && isKeyUp(VK_MENU)) return FALSE;
			if (useWinKey && (isKeyUp(VK_LWIN) && isKeyUp(VK_RWIN))) return FALSE;

			return TRUE;
		}

		BOOL checkModifier(WPARAM wParam)
		{
			// 設定ダイアログのウィンドウハンドルを取得する。
			HWND hwnd = sheba.auin.GetSettingDialog();

			// ::WindowFromPoint() の戻り値が設定ダイアログではない場合は何もしない。
			POINT pos; ::GetCursorPos(&pos);
			if (hwnd != ::WindowFromPoint(pos))
				return FALSE;

			// 修飾キーに指定されたキーの状態が変更された場合はマウスカーソルを更新する。
			if ((useShiftKey && wParam == VK_SHIFT) ||
				(useCtrlKey && wParam == VK_CONTROL) ||
				(useAltKey && wParam == VK_MENU) ||
				(useWinKey && (wParam == VK_LWIN || wParam == VK_RWIN)))
			{
				::PostMessage(hwnd, WM_SETCURSOR, (WPARAM)hwnd, HTCLIENT);

				return TRUE;
			}

			return FALSE;
		}

		static LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
		{
			if (code != HC_ACTION)
				return ::CallNextHookEx(keyboard_hook.hook, code, wParam, lParam);

			keyboard_hook.checkModifier(wParam);

			return ::CallNextHookEx(keyboard_hook.hook, code, wParam, lParam);
		}
	} keyboard_hook;
}
