#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスは::FindWindowA()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(LPCSTR className, LPCSTR windowName)
		{
			MY_TRACE(_T("::FindWindowA(%hs, %hs)\n"), className, windowName);

			// 「ショートカット再生」用の処理です。
			if (className && windowName && _stricmp(className, "AviUtl") == 0)
			{
				auto shuttle = shuttleManager.get(windowName);
				if (shuttle)
					return *shuttle;
			}

			return orig(className, windowName);
		}
		inline static decltype(&hook) orig = ::FindWindowA;
	} FindWindowA;

	//
	// このクラスは::FindWindowW()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(LPCWSTR className, LPCWSTR windowName)
		{
			MY_TRACE(_T("::FindWindowW(%ws, %ws)\n"), className, windowName);

			// 「PSDToolKit」の「送る」用の処理です。
			if (className && _wcsicmp(className, L"ExtendedFilterClass") == 0)
				return hive.settingDialog;

			return orig(className, windowName);
		}
		inline static decltype(&hook) orig = ::FindWindowW;
	} FindWindowW;

	//
	// このクラスは::FindWindowExA()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(HWND parent, HWND childAfter, LPCSTR className, LPCSTR windowName)
		{
			MY_TRACE(_T("::FindWindowExA(0x%08X, 0x%08X, %hs, %hs)\n"), parent, childAfter, className, windowName);

			if (!parent && className)
			{
				// 「テキスト編集補助プラグイン」用の処理です。
				if (_stricmp(className, "ExtendedFilterClass") == 0)
				{
					return hive.settingDialog;
				}
				// 「キーフレームプラグイン」用の処理です。
				else if (_stricmp(className, "AviUtl") == 0 && windowName)
				{
					auto shuttle = shuttleManager.get(windowName);
					if (shuttle)
					{
						MY_TRACE(_T("%hs を返します\n"), windowName);

						return *shuttle;
					}
				}
			}

			return orig(parent, childAfter, className, windowName);
		}
		inline static decltype(&hook) orig = ::FindWindowExA;
	} FindWindowExA;

	//
	// このクラスは::GetWindow()をフックします。
	//
	inline struct
	{
		inline static HWND WINAPI hook(HWND hwnd, UINT cmd)
		{
//			MY_TRACE(_T("::GetWindow(0x%08X, %d)\n"), hwnd, cmd);
//			MY_TRACE_HWND(hwnd);

			if (cmd == GW_OWNER)
			{
				if (hwnd == hive.exeditWindow)
				{
					// 拡張編集ウィンドウのオーナーウィンドウとしてAviUtlウィンドウを返すようにします。
					return hive.aviutlWindow;
				}
				else if (hwnd == hive.settingDialog)
				{
					// 設定ダイアログのオーナーウィンドウとして拡張編集ウィンドウを返すようにします。
					return hive.exeditWindow;
				}

				if (Shuttle::getPointer(hwnd))
				{
					// 「スクリプト並べ替え管理」「シークバー＋」などの一般的なプラグイン用の処理です。
					// シャトルのオーナーが要求されたときはAviUtlウィンドウを返すようにします。
					return hive.aviutlWindow;
				}
			}

			return orig(hwnd, cmd);
		}
		inline static decltype(&hook) orig = ::GetWindow;
	} GetWindow;

	//
	// このクラスは::EnumThreadWindows()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(DWORD threadId, WNDENUMPROC enumProc, LPARAM lParam)
		{
//			MY_TRACE(_T("::EnumThreadWindows(%d, 0x%08X, 0x%08X)\n"), threadId, enumProc, lParam);

			// 「イージング設定時短プラグイン」用の処理です。
			if (threadId == ::GetCurrentThreadId() && enumProc && lParam)
			{
				// enumProc()の中で::GetWindow()が呼ばれます。
				if (!enumProc(hive.settingDialog, lParam))
					return FALSE;
			}

			return orig(threadId, enumProc, lParam);
		}
		inline static decltype(&hook) orig = ::EnumThreadWindows;
	} EnumThreadWindows;

	//
	// このクラスは::EnumWindows()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(WNDENUMPROC enumProc, LPARAM lParam)
		{
//			MY_TRACE(_T("::EnumWindows(0x%08X, 0x%08X)\n"), enumProc, lParam);

			// 「拡張編集RAMプレビュー」用の処理です。
			if (enumProc && lParam)
			{
				if (!enumProc(hive.aviutlWindow, lParam))
					return FALSE;
			}

			return orig(enumProc, lParam);
		}
		inline static decltype(&hook) orig = ::EnumWindows;
	} EnumWindows;

	//
	// このクラスは::SetWindowLongA()をフックします。
	//
	inline struct
	{
		inline static LONG WINAPI hook(HWND hwnd, int index, LONG newLong)
		{
//			MY_TRACE(_T("::SetWindowLongA(0x%08X, %d, 0x%08X)\n"), hwnd, index, newLong);

			// 「拡張ツールバー」用の処理です。
			if (index == GWL_HWNDPARENT)
			{
				Shuttle* shuttle = Shuttle::getPointer(hwnd);
				if (shuttle)
				{
					MY_TRACE(_T("シャトルが取得できるウィンドウはHWNDPARENTを変更できないようにします\n"));
					return 0;
				}
			}

			return orig(hwnd, index, newLong);
		}
		inline static decltype(&hook) orig = ::SetWindowLongA;
	} SetWindowLongA;
}
