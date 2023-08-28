#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------
namespace Bouyomisan {
//---------------------------------------------------------------------

HWND g_target = 0; // Bouyomisan のウィンドウ。
HWND g_holder = 0; // Bouyomisan のウィンドウを保有するウィンドウ (ホルダー)。

struct TimerID {
	static const UINT Init = 1000;
};

//---------------------------------------------------------------------

HWND findWindow()
{
	HWND result = 0;
	::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
		// ウィンドウ名をチェックする。
		TCHAR windowText[MAX_PATH] = {};
		::GetWindowText(hwnd, windowText, std::size(windowText));
		if (!_tcsstr(windowText, _T("棒読みさん"))) return TRUE;

		// クラス名をチェックする。
		TCHAR className[MAX_PATH] = {};
		::GetClassName(hwnd, className, std::size(className));
		if (!_tcsstr(className, _T("Bouyomisan"))) return TRUE;

		// 棒読みさんウィンドウが見つかったので FALSE を返して列挙を終了する。
		*(HWND*)lParam = hwnd;
		return FALSE;
	}, (LPARAM)&result);

	return result;
}

// ホルダーを作成する。
HWND createHolder()
{
	MY_TRACE(_T("Bouyomisan::createHolder()\n"));
/*
	if (!::GetModuleHandle(_T("Bouyomisan.auf")))
	{
		MY_TRACE(_T("Bouyomisan が見つかりませんでした\n"));

		return 0;
	}
*/
	const LPCTSTR className = _T("Nest.BouyomisanHolder");
	const LPCTSTR name = _T("棒読みさん");

	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.lpfnWndProc = Bouyomisan::holderProc;
	wc.hInstance = g_instance;
	wc.lpszClassName = className;
	::RegisterClass(&wc);

	g_holder = ::CreateWindowEx(
		0,
		className,
		name,
		WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		g_hub, 0, g_instance, 0);

	{
		// ホルダーををシャトルの中に入れる。
		ShuttlePtr shuttle = std::make_shared<Shuttle>();
		g_shuttleManager.addShuttle(shuttle, name, g_holder);
	}

	// このタイミングではまだ Bouyomisan ウィンドウが作成されていないので、
	// タイマーで遅らせて初期化処理を行う。
	::SetTimer(g_holder, TimerID::Init, 1000, 0);

	return g_holder;
}

// ホルダーの表示/非表示を切り替える。
void showHolder()
{
	MY_TRACE(_T("Bouyomisan::showHolder()\n"));

	::SendMessage(g_holder, WM_CLOSE, 0, 0);
}

// Bouyomisan ウィンドウをホルダーのクライアント領域まで広げる。
// 非同期で処理する。
void resize()
{
	MY_TRACE(_T("Bouyomisan::resize()\n"));

	RECT rc; ::GetClientRect(g_holder, &rc);
	int w = getWidth(rc);
	int h = getHeight(rc);

	UINT flags = SWP_NOZORDER | SWP_ASYNCWINDOWPOS;

	if (::IsWindowVisible(g_holder))
		flags |= SWP_SHOWWINDOW;

	::SetWindowPos(g_target, 0, 0, 0, w, h, flags);
}

// ホルダーのウィンドウ関数。
LRESULT CALLBACK holderProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
			MY_TRACE(_T("Bouyomisan::holderProc(WM_CREATE)\n"));

			break;
		}
	case WM_DESTROY:
		{
			MY_TRACE(_T("Bouyomisan::holderProc(WM_DESTROY)\n"));

			break;
		}
	case WM_CLOSE:
		{
			MY_TRACE(_T("Bouyomisan::holderProc(WM_CLOSE)\n"));

			if (::IsWindowVisible(hwnd))
			{
				::ShowWindow(hwnd, SW_HIDE);
			}
			else
			{
				::ShowWindow(hwnd, SW_SHOW);
			}

			return 0;
		}
	case WM_SHOWWINDOW:
		{
			MY_TRACE(_T("Bouyomisan::holderProc(WM_SHOWWINDOW)\n"));

			if (wParam && ::IsWindow(g_target))
				::ShowWindow(g_target, SW_SHOW);

			break;
		}
	case WM_SIZE:
		{
			MY_TRACE(_T("Bouyomisan::holderProc(WM_SIZE)\n"));

			if (::IsWindow(g_target))
				resize();

			break;
		}
	case WM_TIMER:
		{
			switch (wParam)
			{
			case TimerID::Init:
				{
					g_target = findWindow();

					if (g_target)
					{
						MY_TRACE(_T("Bouyomisan ウィンドウが見つかりました => 0x%08X\n"), g_target);

						DWORD remove = WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
						DWORD add = WS_CHILD;

						::KillTimer(hwnd, TimerID::Init);
						modifyStyle(g_target, remove, add);
						::SetParent(g_target, hwnd);
						resize();
					}

					break;
				}
			}

			break;
		}
	}

	return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//---------------------------------------------------------------------
} // namespace Bouyomisan
//---------------------------------------------------------------------
