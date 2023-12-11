#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはWH_CALLWNDPROCRETをフックします。
	//
	inline static struct CallWndProcRet
	{
		HHOOK hook = 0;

		//
		// 初期化処理です。
		//
		void init()
		{
			MY_TRACE_FUNC("");

			hook = ::SetWindowsHookEx(WH_CALLWNDPROCRET, hookProc, 0, ::GetCurrentThreadId());
			MY_TRACE_HEX(hook);
		}

		//
		// 後始末処理です。
		//
		void exit()
		{
			MY_TRACE_FUNC("");

			::UnhookWindowsHookEx(hook), hook = 0;
		}

		//
		// WH_CALLWNDPROCRETのフックプロシージャです。
		//
		static LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
		{
			if (code != HC_ACTION)
				return ::CallNextHookEx(0, code, wParam, lParam);

			auto cwpr = (CWPRETSTRUCT*)lParam;

			switch (cwpr->message)
			{
			case WM_NCCREATE:
				{
					MY_TRACE_INT(cwpr->lResult);

					if (cwpr->lResult == -1)
						break;

					auto hwnd = cwpr->hwnd;

					// ウィンドウ名を取得します。
					TCHAR windowName[MAX_PATH] = {};
					::GetWindowText(hwnd, windowName, std::size(windowName));
					MY_TRACE_TSTR(windowName);

					// ウィンドウがフック対象かチェックします。
					if (isHookTarget(hwnd, windowName))
					{
						// ここで確保したメモリはMainWindowのWM_INIT_SHUTTLEで開放します。
						auto orig = new Hive::OrigWindow();

						// ウィンドウ名を取得します。
						::StringCchCopy(orig->windowName, std::size(orig->windowName), windowName);

						// MainWindowにWM_INIT_SHUTTLEを送信します。
						::PostMessage(hive.mainWindow, Hive::WindowMessage::WM_INIT_SHUTTLE, (WPARAM)hwnd, (LPARAM)orig);
					}

					break;
				}
			}

			return ::CallNextHookEx(0, code, wParam, lParam);
		}

		static BOOL isHookTarget(HWND hwnd, LPCTSTR windowName)
		{
			if (!windowName) return FALSE;

			// クラス名を取得します。
			TCHAR className[MAX_PATH] = {};
			::GetClassName(hwnd, className, std::size(className));

			// 主要ウィンドウのハンドルはここで取得しておきます。
			if (_tcsicmp(className, _T("AviUtl")) == 0)
			{
				if (_tcsicmp(windowName, _T("AviUtl")) == 0)
				{
					if (!hive.aviutlWindow) hive.aviutlWindow = hwnd;
				}
				else if (_tcsicmp(windowName, _T("拡張編集")) == 0)
				{
					if (!hive.exeditWindow) hive.exeditWindow = hwnd;
				}

				return TRUE;
			}
			else if (_tcsicmp(className, _T("ExtendedFilterClass")) == 0)
			{
				if (!hive.settingDialog) hive.settingDialog = hwnd;

				return TRUE;
			}

			return FALSE;
		}
	} call_wnd_proc_ret;
}
