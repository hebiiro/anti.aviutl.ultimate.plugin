#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------

void Shuttle::init(HWND hwnd)
{
	// ターゲットのウィンドウハンドルを格納する。
	m_hwnd = hwnd;

	// ウィンドウハンドルにポインタを結び付ける。
	setShuttle(m_hwnd, this);

	// コンテナを作成する。
	m_dockContainer.reset(onCreateDockContainer());
	m_floatContainer.reset(onCreateFloatContainer());

	// ターゲットのウィンドウスタイルを変更する。
	DWORD style = onGetTargetNewStyle();
	::SetWindowLong(m_hwnd, GWL_STYLE, style | WS_CHILD);
//	::SetWindowPos(m_hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	// ターゲットのウィンドウタイトルを取得する。
	TCHAR text[MAX_PATH] = {};
	::GetWindowText(m_hwnd, text, MAX_PATH);
	::SetWindowText(m_floatContainer->m_hwnd, text);

	// ターゲットウィンドウのウィンドウ矩形を取得しておく。
	RECT rc; ::GetWindowRect(m_hwnd, &rc);
	int x = rc.left;
	int y = rc.top;
	int w = getWidth(rc);
	int h = getHeight(rc);

	// ターゲットウィンドウの表示状態を取得しておく。
	BOOL visible = ::IsWindowVisible(m_hwnd);

	// ターゲットの親をフローティングコンテナに変更する。
	::SetParent(m_hwnd, m_floatContainer->m_hwnd);

	// ::SetWindowPos() を呼び出し、フック処理を促す。
	// コンテナの種類によってはコンテナのウィンドウサイズが調整される。
	::SetWindowPos(m_hwnd, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

	// ターゲットが表示状態ならフローティングコンテナを表示する。
	if (visible)
		::ShowWindow(m_floatContainer->m_hwnd, SW_SHOW);

	// ターゲットをサブクラス化する。
	m_targetWndProc = (WNDPROC)::SetWindowLong(m_hwnd, GWL_WNDPROC, (LONG)targetWndProc);
}

Container* Shuttle::onCreateDockContainer()
{
	// ドッキングコンテナのスタイルを決定する。
	DWORD style = ::GetWindowLong(m_hwnd, GWL_STYLE);
	DWORD dockStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	// ドッキングコンテナのタイプを決定する。
	if (style & WS_THICKFRAME)
		return new SpreadContainer(this, dockStyle);
	else
		return new ScrollContainer(this, dockStyle);
}

Container* Shuttle::onCreateFloatContainer()
{
	// フローティングコンテナのスタイルを決定する。
	DWORD style = ::GetWindowLong(m_hwnd, GWL_STYLE);
	DWORD floatStyle = WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	floatStyle |= style & (WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

	// フローティングコンテナのタイプを決定する。
	if (style & WS_THICKFRAME)
		return new SpreadContainer(this, floatStyle);
	else
		return new Container(this, floatStyle);
}

DWORD Shuttle::onGetTargetNewStyle()
{
	// ※ WS_CAPTION を外さないとエディットボックスでマウス処理が行われなくなる。
	DWORD style = ::GetWindowLong(m_hwnd, GWL_STYLE);
	style &= ~(WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	return style;
}

void Shuttle::onSetTargetWindowPos(LPRECT rc)
{
	// スクロールバーがあるときは、その分矩形を縮小する。

	DWORD style = ::GetWindowLong(m_hwnd, GWL_STYLE);

	if (style & WS_VSCROLL) rc->right -= ::GetSystemMetrics(SM_CXVSCROLL);
	if (style & WS_HSCROLL) rc->bottom -= ::GetSystemMetrics(SM_CYHSCROLL);
}

LRESULT Shuttle::onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return container->onWndProc(hwnd, message, wParam, lParam);
}

LRESULT Shuttle::onTargetWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NCPAINT:
		{
			// クライアント領域を除外してから塗り潰す。
			HDC dc = ::GetWindowDC(hwnd);
			RECT rc; ::GetWindowRect(hwnd, &rc);
			RECT rcClip; ::GetClientRect(hwnd, &rcClip);
			::MapWindowPoints(hwnd, 0, (LPPOINT)&rcClip, 2);
			::OffsetRect(&rcClip, -rc.left, -rc.top);
			::OffsetRect(&rc, -rc.left, -rc.top);
			HRGN rgn = ::CreateRectRgnIndirect(&rcClip);
			::ExtSelectClipRgn(dc, rgn, RGN_DIFF);
			::DeleteObject(rgn);
			fillBackground(dc, &rc);
			::ExtSelectClipRgn(dc, 0, RGN_COPY);
			::ReleaseDC(hwnd, dc);
			return 0;
		}
	case WM_SHOWWINDOW:
		{
//			MY_TRACE(_T("WM_SHOWWINDOW, 0x%08X, 0x%08X\n"), wParam, lParam);

			// ターゲットウィンドウの表示状態が変更されたらコンテナもそれに追従する。
			::ShowWindow(::GetParent(hwnd), wParam ? SW_SHOW : SW_HIDE);

			break;
		}
	case WM_SETTEXT:
		{
//			MY_TRACE(_T("WM_SETTEXT, 0x%08X, 0x%08X\n"), wParam, lParam);

			// コンテナのウィンドウテキストを更新する。
			::SetWindowText(::GetParent(hwnd), (LPCTSTR)lParam);

			if (m_pane)
			{
				// タブのテキストを更新する。
				m_pane->m_tab.changeText(this, (LPCTSTR)lParam);

				// ペインのタイトル部分を再描画する。
				::InvalidateRect(m_pane->m_owner, &m_pane->m_position, FALSE);
			}

			break;
		}
	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		{
			// 「キーフレームプラグイン」用。
			WPARAM active = (message == WM_SETFOCUS) ? WA_ACTIVE : WA_INACTIVE;
			::SendMessage(hwnd, WM_ACTIVATE, active, 0);

			if (m_pane)
			{
				// ペインのタイトル部分を再描画する。
				::InvalidateRect(m_pane->m_owner, &m_pane->m_position, FALSE);
			}

			break;
		}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		{
			// ターゲットウィンドウがクリックされたらフォーカスを当てるようにする。
			::SetFocus(hwnd);

			break;
		}
	case WM_GETMINMAXINFO:
		{
			// ウィンドウの限界サイズを増やしておく。
			MINMAXINFO* mmi = (MINMAXINFO*)lParam;
			mmi->ptMaxTrackSize.y *= 3;

			break;
		}
	}

	return container->onTargetWndProc(hwnd, message, wParam, lParam);
}

// ターゲットウィンドウを表示する。
void Shuttle::showTargetWindow()
{
	// ターゲットウィンドウが非表示状態なら
	if (!::IsWindowVisible(m_hwnd))
	{
		if (this == g_aviutlWindow.get())
		{
			// AviUtl ウィンドウは普通に表示する。
			::ShowWindow(m_hwnd, SW_SHOW);
		}
		else if (this == g_settingDialog.get())
		{
			// 設定ダイアログの表示は拡張編集に任せる。
			return;
		}
		else
		{
			// ターゲットウィンドウを表示状態にする。
			// (WM_CLOSE は表示/非表示状態をトグルで切り替える)
			::SendMessage(m_hwnd, WM_CLOSE, 0, 0);
		}

		// AviUtl がターゲットウィンドウを表示しなかった場合は
		if (!::IsWindowVisible(m_hwnd))
		{
			// 手動でウィンドウを表示する。
			::ShowWindow(m_hwnd, SW_SHOW);
		}
	}
}

void Shuttle::dockWindow()
{
	MY_TRACE_HWND(m_pane->m_owner);

	// ドッキングコンテナの親ウィンドウを切り替える。
	::SetParent(m_dockContainer->m_hwnd, m_pane->m_owner);

	// 親ウィンドウをドッキングコンテナに切り替える。
	::SetParent(m_hwnd, m_dockContainer->m_hwnd);
	::ShowWindow(m_floatContainer->m_hwnd, SW_HIDE);
	::ShowWindow(m_dockContainer->m_hwnd, SW_SHOW);

	showTargetWindow();
}

void Shuttle::floatWindow()
{
	TCHAR text[MAX_PATH] = {};
	::GetWindowText(m_hwnd, text, MAX_PATH);
	::SetWindowText(m_floatContainer->m_hwnd, text);

	resizeFloatContainer();

	// 親ウィンドウをフローティングコンテナに切り替える。
	::SetParent(m_hwnd, m_floatContainer->m_hwnd);
	::ShowWindow(m_dockContainer->m_hwnd, SW_HIDE);
	if (::IsWindowVisible(m_hwnd))
		::ShowWindow(m_floatContainer->m_hwnd, SW_SHOW);

	// ドッキングコンテナの親ウィンドウを切り替える。
	::SetParent(m_dockContainer->m_hwnd, g_hub);
}

void Shuttle::resizeDockContainer(LPCRECT rc)
{
	m_dockContainer->onResizeDockContainer(rc);
}

void Shuttle::resizeFloatContainer()
{
	m_floatContainer->onResizeFloatContainer();
}

LRESULT CALLBACK Shuttle::targetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hwndContainer = ::GetParent(hwnd);
	Container* container = Container::getContainer(hwndContainer);
	if (!container) return ::DefWindowProc(hwnd, message, wParam, lParam);
	return container->m_shuttle->onTargetWndProc(container, hwnd, message, wParam, lParam);
}

Shuttle* Shuttle::getShuttle(HWND hwnd)
{
	return (Shuttle*)::GetProp(hwnd, _T("Nest.Shuttle"));
}

void Shuttle::setShuttle(HWND hwnd, Shuttle* window)
{
	::SetProp(hwnd, _T("Nest.Shuttle"), window);
}

//---------------------------------------------------------------------
