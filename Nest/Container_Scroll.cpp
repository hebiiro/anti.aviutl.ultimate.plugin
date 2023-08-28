#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------

ScrollContainer::ScrollContainer(Shuttle* shuttle, DWORD style)
	: Container(shuttle, style)
{
}

ScrollContainer::~ScrollContainer()
{
}

// スクロール範囲を更新する。
void ScrollContainer::updateScrollBar(HWND hwndContainer)
{
	RECT rc; ::GetClientRect(m_shuttle->m_hwnd, &rc);
	int w = rc.right - rc.left;
	int h = rc.bottom - rc.top;

	RECT rcContainer; ::GetClientRect(hwndContainer, &rcContainer);
	int cw = rcContainer.right - rcContainer.left;
	int ch = rcContainer.bottom - rcContainer.top;

	SCROLLINFO si = { sizeof(si) };
	si.fMask = SIF_PAGE | SIF_RANGE;
	si.nMin = 0;
	si.nMax = w - 1;
	si.nPage = cw;
	::SetScrollInfo(hwndContainer, SB_HORZ, &si, TRUE);
	si.nMin = 0;
	si.nMax = h - 1;
	si.nPage = ch;
	::SetScrollInfo(hwndContainer, SB_VERT, &si, TRUE);

	::SetWindowPos(m_hwnd, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

// スクロール位置を変更する。
void ScrollContainer::scroll(HWND hwndContainer, int bar, WPARAM wParam)
{
	SCROLLINFO si = { sizeof(si) };
	si.fMask = SIF_POS | SIF_RANGE;
	::GetScrollInfo(hwndContainer, bar, &si);

	switch (LOWORD(wParam))
	{
	case SB_LEFT:      si.nPos = si.nMin; break;
	case SB_RIGHT:     si.nPos = si.nMax; break;
	case SB_LINELEFT:  si.nPos += -10; break;
	case SB_LINERIGHT: si.nPos +=  10; break;
	case SB_PAGELEFT:  si.nPos += -60; break;
	case SB_PAGERIGHT: si.nPos +=  60; break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION: si.nPos = HIWORD(wParam); break;
	}

	::SetScrollInfo(hwndContainer, bar, &si, TRUE);
}

// ターゲットウィンドウの位置を再計算する。
void ScrollContainer::recalcLayout(HWND hwndContainer)
{
	// ターゲットウィンドウの位置のみを調整する。

	RECT rc = { 0, 0 };
	clientToWindow(m_shuttle->m_hwnd, &rc);
	int x = rc.left;
	int y = rc.top;

	x -= ::GetScrollPos(hwndContainer, SB_HORZ);
	y -= ::GetScrollPos(hwndContainer, SB_VERT);

	true_SetWindowPos(m_shuttle->m_hwnd, 0,
		x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}

// ドッキングコンテナのサイズを再計算するときに呼ばれる。
void ScrollContainer::onResizeDockContainer(LPCRECT rc)
{
	// フローティングコンテナを取得する。
	// (ただし、ドッキング完了後に呼び出された場合は parent は自分自身 (m_hwnd と同じ) になる)
	HWND parent = ::GetParent(m_shuttle->m_hwnd);

	// rc からドッキングコンテナの新しい位置を取得する。
	int x = rc->left;
	int y = rc->top;
	int w = rc->right - rc->left;
	int h = rc->bottom - rc->top;

	x = std::min<int>(rc->left, rc->right);
	y = std::min<int>(rc->top, rc->bottom);
	w = std::max<int>(w, 0);
	h = std::max<int>(h, 0);

	// ドッキングコンテナの位置を更新する。
	true_SetWindowPos(m_hwnd, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	// コンテナのサイズが変更されたのでスクロール範囲を更新する。
	// スクロール位置を更新する前に行う必要がある。
	updateScrollBar(m_hwnd);

	// フローティングコンテナのスクロール位置をドッキングコンテナに適用する。
	::SetScrollPos(m_hwnd, SB_HORZ, ::GetScrollPos(parent, SB_HORZ), TRUE);
	::SetScrollPos(m_hwnd, SB_VERT, ::GetScrollPos(parent, SB_VERT), TRUE);

	// スクロール位置が更新されたのでレイアウトを再計算する。
	recalcLayout(m_hwnd);
}

// ターゲットがフローティング状態になったときに呼ばれる。
void ScrollContainer::onResizeFloatContainer()
{
	// ドッキングコンテナを取得する。
	HWND parent = ::GetParent(m_shuttle->m_hwnd);

	// ドッキングコンテナのクライアント矩形からフローティングコンテナの新しい位置を取得する。
	RECT rc; ::GetClientRect(parent, &rc);
	::MapWindowPoints(parent, 0, (LPPOINT)&rc, 2);
	clientToWindow(m_hwnd, &rc);
	int x = rc.left;
	int y = rc.top;
	int w = getWidth(rc);
	int h = getHeight(rc);

	// フローティングコンテナの位置を更新する。
	true_SetWindowPos(m_hwnd, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

	// コンテナのサイズが変更されたのでスクロール範囲を更新する。
	// スクロール位置を更新する前に行う必要がある。
	updateScrollBar(m_hwnd);

	// ドッキングコンテナのスクロール位置をフローティングコンテナに適用する。
	::SetScrollPos(m_hwnd, SB_HORZ, ::GetScrollPos(parent, SB_HORZ), TRUE);
	::SetScrollPos(m_hwnd, SB_VERT, ::GetScrollPos(parent, SB_VERT), TRUE);

	// スクロール位置が更新されたのでレイアウトを再計算する。
	recalcLayout(m_hwnd);
}

// ターゲットウィンドウのサイズが変更されたときに呼ばれる。
BOOL ScrollContainer::onSetContainerPos(WINDOWPOS* wp)
{
	// スクロールバーとレイアウトを更新する。
	updateScrollBar(m_hwnd);
	recalcLayout(m_hwnd);

	return FALSE;
}

// コンテナウィンドウのウィンドウ関数。
LRESULT ScrollContainer::onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_SIZE:
		{
			// スクロールバーとレイアウトを更新する。
			updateScrollBar(hwnd);
			recalcLayout(hwnd);

			return 0; // コンテナのデフォルト処理は実行しない。
		}
	}

	LRESULT result = Container::onWndProc(hwnd, message, wParam, lParam);

	switch (message)
	{
	case WM_VSCROLL:
		{
			// 縦スクロールに応じてスクロールしてからレイアウトを再計算する。
			scroll(hwnd, SB_VERT, wParam);
			recalcLayout(hwnd);

			break;
		}
	case WM_HSCROLL:
		{
			// 横スクロールに応じてスクロールしてからレイアウトを再計算する。
			scroll(hwnd, SB_HORZ, wParam);
			recalcLayout(hwnd);

			break;
		}
	case WM_MOUSEWHEEL:
		{
			MY_TRACE(_T("ScrollContainer::containerWndProc(WM_MOUSEWHEEL, 0x%08X, 0x%08X)\n"), wParam, lParam);

			// ホイールに応じてスクロールしてからレイアウトを再計算する。
			scroll(hwnd, SB_VERT, ((int)wParam > 0) ? SB_PAGEUP : SB_PAGEDOWN);
			recalcLayout(hwnd);

			break;
		}
	}

	return result;
}

// ターゲットウィンドウのウィンドウ関数。
LRESULT ScrollContainer::onTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEWHEEL:
		{
			// 既定の処理をブロックし、コンテナウィンドウへバイパス
			::SendMessage(m_hwnd, message, wParam, lParam);

			return 0;
		}
	}

	return Container::onTargetWndProc(hwnd, message, wParam, lParam);
}

//---------------------------------------------------------------------
