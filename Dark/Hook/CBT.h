#pragma once
#include "Servant.h"

namespace fgo::dark::hook
{
	//
	// このクラスはWH_CBTをフックします。
	//
	inline struct CBT
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
//					MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X", code, wParam, lParam);

					auto hwnd = (HWND)wParam;
//					MY_TRACE_HWND(hwnd);
					auto cw = (CBT_CREATEWND*)lParam;

					TCHAR className[MAX_PATH] = {};
					::GetClassName(hwnd, className, std::size(className));

					if (	cw->lpcs->lpszName &&
						!::GetParent(hwnd) &&
						::lstrcmpi(className, _T("AviUtl")) == 0 &&
						::lstrcmpi(cw->lpcs->lpszName, _T("AviUtl")) == 0)
					{
						MY_TRACE(_T("AviUtlウィンドウが見つかりました\n"));

						if (servant.init(hwnd))
							break; // AviUtlウィンドウはここではサブクラス化しません。
					}

					gdi::client.subclass(hwnd);

					break;
				}
			}

			return ::CallNextHookEx(0, code, wParam, lParam);
		}
	} cbt;
}
