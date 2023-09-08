#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスは低レベルのマウス入力イベントをフックします。
	//
	inline static struct LowLevelMouse
	{
		HHOOK hook = 0;

		//
		// 初期化処理です。
		//
		void init()
		{
			MY_TRACE(_T("LowLevelMouse::init()\n"));

			hook = ::SetWindowsHookEx(WH_MOUSE_LL, hookProc, hive.instance, 0);
			MY_TRACE_HEX(hook);
		}

		//
		// 後始末処理です。
		//
		void exit()
		{
			MY_TRACE(_T("LowLevelMouse::exit()\n"));

			::UnhookWindowsHookEx(hook), hook = 0;
		}

		static LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
		{
			if (code >= 0 && hive.scriptParamDialog)
			{
				auto llm = (MSLLHOOKSTRUCT*)lParam;

				switch (wParam)
				{
				case WM_RBUTTONDOWN:
					{
						MY_TRACE(_T("LowLevelMouse::hookProc(WM_RBUTTONDOWN)\n"));

						// マウスカーソル直下のウィンドウを取得します。
						POINT point; ::GetCursorPos(&point);
						HWND hwnd = ::WindowFromPoint(point);
						MY_TRACE_HWND(hwnd);

						// ウィンドウがドッキングサイトかチェックします。
						if (!MainWindow::getRootPane(hwnd)) break;

						// PSDToolKitのシャトルを取得します。
						auto shuttle = shuttleManager.get(Hive::PSDToolKitName);
						if (!shuttle) break;

						HWND child = ::GetWindow(*shuttle, GW_CHILD);
						MY_TRACE_HWND(hwnd);

						// マウスカーソルが子ウィンドウ内に存在するかチェックします。
						RECT rc; ::GetWindowRect(child, &rc);
						if (!::PtInRect(&rc, point)) break;

						// マウスメッセージを送信します。
						::ScreenToClient(child, &point);
						::PostMessage(child, WM_RBUTTONDOWN, MK_RBUTTON, PT2LP(point));

						MY_TRACE(_T("マウスメッセージを送信しました\n"));

						break;
					}
				}
			}

			return ::CallNextHookEx(low_level_mouse.hook, code, wParam, lParam);
		}
	} low_level_mouse;
}
