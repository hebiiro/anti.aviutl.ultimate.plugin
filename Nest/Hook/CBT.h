#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはWH_CBTをフックします。
	//
	inline static struct CBT
	{
		HHOOK hook = 0;

		//
		// 初期化処理です。
		//
		void init()
		{
			MY_TRACE_FUNC("");

			hook = ::SetWindowsHookEx(WH_CBT, hookProc, 0, ::GetCurrentThreadId());
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
		// WH_CBTのフックプロシージャです。
		//
		static LRESULT CALLBACK hookProc(int code, WPARAM wParam, LPARAM lParam)
		{
			switch (code)
			{
			case HCBT_CREATEWND:
				{
					// ここでAviUtlウィンドウをサブクラス化すると謎のエラーが発生してしまうので、
					// ポスト先でサブクラス化します。
					// その他のウィンドウもAviUtlウィンドウのあとに処理したいので、
					// やはりポスト先でサブクラス化します。

					HWND hwnd = (HWND)wParam;
					auto cw = (CBT_CREATEWND*)lParam;

					// ウィンドウがフック対象かチェックします。
					if (isHookTarget(hwnd, cw->lpcs->lpszName))
					{
						// ウィンドウ名を取得します。
						// ここで確保されたメモリはMainWindowのWM_INIT_SHUTTLEで開放されます。
						LPTSTR windowName = new TCHAR[MAX_PATH] {};
						::StringCchCopy(windowName, MAX_PATH, cw->lpcs->lpszName);

						::PostMessage(hive.mainWindow, Hive::WindowMessage::WM_INIT_SHUTTLE, (WPARAM)hwnd, (LPARAM)windowName);
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
	} cbt;
}
