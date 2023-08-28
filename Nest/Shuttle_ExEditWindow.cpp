#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------

void ExEditWindow::init(HWND hwnd)
{
	Shuttle::init(hwnd);

	// フローティングコンテナのアイコンを設定する。
	HICON icon = (HICON)::GetClassLong(m_hwnd, GCL_HICON);
	::SendMessage(m_floatContainer->m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon);
	::SendMessage(m_floatContainer->m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon);
#if 0
	// エラーになるため、一時的に無効化。
	WNDCLASS wc = {};
	wc.style = 0;
	wc.hCursor = 0;
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hInstance = ::GetModuleHandle(_T("exedit.auf"));
	wc.lpszClassName = _T("AviUtl"); // 「VoiceroidUtil」用。
	ATOM atom = ::RegisterClass(&wc);

	m_dummy = true_CreateWindowExA(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
		"AviUtl",
		"拡張編集",
		WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0, 0, 0,
		g_hub, 0, wc.hInstance, 0);
#endif
}

DWORD ExEditWindow::onGetTargetNewStyle()
{
	// ※ WS_CAPTION を外すとウィンドウの高さ調節にズレが生じる。
	DWORD style = ::GetWindowLong(m_hwnd, GWL_STYLE);
	style &= ~(WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	return style;
}

void ExEditWindow::onSetTargetWindowPos(LPRECT rc)
{
	// ターゲットウィンドウのサイズを微調整する。
	rc->bottom -= g_auin.GetLayerHeight() / 2;
	::SendMessage(m_hwnd, WM_SIZING, WMSZ_BOTTOM, (LPARAM)rc);
}

LRESULT ExEditWindow::onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Shuttle::onContainerWndProc(container, hwnd, message, wParam, lParam);
}

LRESULT ExEditWindow::onTargetWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SHOWWINDOW:
		{
			::ShowWindow(m_dummy, wParam ? SW_SHOW : SW_HIDE);

			break;
		}
	case WM_SETTEXT:
		{
			::SetWindowText(m_dummy, (LPCTSTR)lParam);

			break;
		}
	}

	return Shuttle::onTargetWndProc(container, hwnd, message, wParam, lParam);
}

//---------------------------------------------------------------------
