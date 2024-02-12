#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはウィンドウの生成をフックします。
	//
	inline struct
	{
		//
		// 初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			auto user32 = ::GetModuleHandle(_T("user32.dll"));

			Tools::attach(CreateWindowExA, ::GetProcAddress(user32, "CreateWindowExA"));
			Tools::attach(CreateWindowExW, ::GetProcAddress(user32, "CreateWindowExW"));

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ウィンドウをフックします。
		//
		static BOOL hookWindow(HWND hwnd, LPCTSTR className, LPCTSTR windowName)
		{
			MY_TRACE_FUNC("0x%08X, %s, %s", hwnd, className, windowName);

			// ウィンドウがフック対象かチェックします。
			if (!isHookTarget(hwnd, className, windowName)) return FALSE;

			// ここで確保したメモリはMainWindowのInitShuttleで開放します。
			auto orig = new Hive::OrigWindow();

			// ウィンドウ名を取得します。
			::StringCchCopy(orig->windowName, std::size(orig->windowName), windowName);

			// MainWindowにInitShuttleを送信します。
			::SendMessage(hive.mainWindow, Hive::WindowMessage::InitShuttle, (WPARAM)hwnd, (LPARAM)orig);

			return TRUE;
		}

		static BOOL isHookTarget(HWND hwnd, LPCTSTR className, LPCTSTR windowName)
		{
			if (!windowName || !className) return FALSE;

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

		//
		// このクラスは::CreateWindowExA()をフックします。
		//
		inline static struct
		{
			inline static HWND WINAPI hook(DWORD exStyle, LPCSTR className, LPCSTR windowName,
				DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				HWND hwnd = orig(exStyle, className, windowName, style, x, y, w, h, parent, menu, instance, param);

				// クラス名がアトムの場合は何もしません。
				if ((DWORD)className <= 0x0000FFFF) return hwnd;

				hookWindow(hwnd, (_bstr_t)className, (_bstr_t)windowName);

				return hwnd;
			}
			inline static decltype(&hook) orig = 0;
		} CreateWindowExA;

		//
		// このクラスは::CreateWindowExW()をフックします。
		//
		inline static struct
		{
			inline static HWND WINAPI hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
				DWORD style, int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				HWND hwnd = orig(exStyle, className, windowName, style, x, y, w, h, parent, menu, instance, param);

				// クラス名がアトムの場合は何もしません。
				if ((DWORD)className <= 0x0000FFFF) return hwnd;

				hookWindow(hwnd, (_bstr_t)className, (_bstr_t)windowName);

				return hwnd;
			}
			inline static decltype(&hook) orig = 0;
		} CreateWindowExW;
	} create_window;
}
