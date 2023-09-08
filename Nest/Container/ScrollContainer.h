#pragma once
#include "Container.h"

namespace fgo::nest
{
	//
	// このクラスはターゲットウィンドウをクライアント領域全体まで広げるコンテナです。
	//
	struct ScrollContainer : Container
	{
		//
		// コンストラクタです。
		//
		ScrollContainer(Listener* listener, DWORD style)
			: Container(listener, style)
		{
		}

		//
		// デストラクタです。
		//
		~ScrollContainer()
		{
		}

		//
		// スクロール範囲を更新します。
		//
		void updateScrollBar()
		{
			// ターゲットウィンドウを取得します。
			HWND target = listener->getTarget();

			RECT rc; ::GetClientRect(target, &rc);
			int w = rc.right - rc.left;
			int h = rc.bottom - rc.top;

			RECT rcContainer; ::GetClientRect(*this, &rcContainer);
			int cw = rcContainer.right - rcContainer.left;
			int ch = rcContainer.bottom - rcContainer.top;

			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nMin = 0;
			si.nMax = w - 1;
			si.nPage = cw;
			::SetScrollInfo(*this, SB_HORZ, &si, TRUE);
			si.nMin = 0;
			si.nMax = h - 1;
			si.nPage = ch;
			::SetScrollInfo(*this, SB_VERT, &si, TRUE);

			::SetWindowPos(*this, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}

		//
		// スクロール位置を変更します。
		//
		void scroll(int bar, WPARAM wParam)
		{
			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_POS | SIF_RANGE;
			::GetScrollInfo(*this, bar, &si);

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

			::SetScrollInfo(*this, bar, &si, TRUE);
		}

		//
		// ターゲットウィンドウの位置を再計算します。
		// ターゲットウィンドウの位置のみを調整します。
		//
		void recalcLayout()
		{
			// ターゲットウィンドウを取得します。
			HWND target = listener->getTarget();

			RECT rc = { 0, 0 };
			clientToWindow(target, &rc);
			int x = rc.left;
			int y = rc.top;

			x -= ::GetScrollPos(*this, SB_HORZ);
			y -= ::GetScrollPos(*this, SB_VERT);

			hive.true_SetWindowPos(target, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
		}

		//
		// ドッキングコンテナのサイズを再計算するときに呼ばれます。
		//
		void onResizeDockContainer(LPCRECT rc) override
		{
			// ターゲットウィンドウを取得します。
			HWND target = listener->getTarget();

			// フローティングコンテナを取得します。
			// (ただし、ドッキング完了後に呼び出された場合は parent はこのコンテナになります)
			HWND parent = ::GetParent(target);

			// rc からドッキングコンテナの新しい位置を取得します。
			int x = rc->left;
			int y = rc->top;
			int w = rc->right - rc->left;
			int h = rc->bottom - rc->top;

			x = std::min<int>(rc->left, rc->right);
			y = std::min<int>(rc->top, rc->bottom);
			w = std::max<int>(w, 0);
			h = std::max<int>(h, 0);

			// ドッキングコンテナの位置を更新します。
			hive.true_SetWindowPos(*this, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

			// コンテナのサイズが変更されたのでスクロール範囲を更新します。
			// スクロール位置を更新する前にこの処理を行う必要があります。
			updateScrollBar();

			// フローティングコンテナのスクロール位置をドッキングコンテナに適用します。
			::SetScrollPos(*this, SB_HORZ, ::GetScrollPos(parent, SB_HORZ), TRUE);
			::SetScrollPos(*this, SB_VERT, ::GetScrollPos(parent, SB_VERT), TRUE);

			// スクロール位置が更新されたのでレイアウトを再計算します。
			recalcLayout();
		}

		//
		// ターゲットがフローティング状態になるときに呼ばれます。
		//
		void onResizeFloatContainer() override
		{
			// ターゲットウィンドウを取得します。
			HWND target = listener->getTarget();

			// ドッキングコンテナを取得します。
			HWND parent = ::GetParent(target);

			// ドッキングコンテナのクライアント矩形からフローティングコンテナの新しい位置を取得します。
			RECT rc; ::GetClientRect(parent, &rc);
			::MapWindowPoints(parent, 0, (LPPOINT)&rc, 2);
			clientToWindow(*this, &rc);
			int x = rc.left;
			int y = rc.top;
			int w = getWidth(rc);
			int h = getHeight(rc);

			// フローティングコンテナの位置を更新します。
			hive.true_SetWindowPos(*this, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

			// コンテナのサイズが変更されたのでスクロール範囲を更新します。
			// スクロール位置を更新する前にこの処理を行う必要があります。
			updateScrollBar();

			// ドッキングコンテナのスクロール位置をフローティングコンテナに適用します。
			::SetScrollPos(*this, SB_HORZ, ::GetScrollPos(parent, SB_HORZ), TRUE);
			::SetScrollPos(*this, SB_VERT, ::GetScrollPos(parent, SB_VERT), TRUE);

			// スクロール位置が更新されたのでレイアウトを再計算します。
			recalcLayout();
		}

		//
		// コンテナの位置を再計算するときに呼ばれます。
		//
		BOOL onSetContainerPos(WINDOWPOS* wp) override
		{
			// スクロールバーとレイアウトを更新します。
			updateScrollBar();
			recalcLayout();

			return FALSE;
		}

		//
		// ターゲットのウィンドウプロシージャです。
		//
		LRESULT onTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* skipDefault) override
		{
			switch (message)
			{
			case WM_MOUSEWHEEL:
				{
					// マウスホイールでスクロールするために
					// 既定の処理をブロックし、コンテナウィンドウへバイパスします。
					*skipDefault = TRUE;
					return ::SendMessage(*this, message, wParam, lParam);
				}
			}

			return __super::onTargetWndProc(hwnd, message, wParam, lParam, skipDefault);
		}

		//
		// コンテナのウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SIZE:
				{
					// スクロールバーとレイアウトを更新します。
					updateScrollBar();
					recalcLayout();

					return 0; // コンテナのデフォルト処理は実行しません。
				}
			}

			LRESULT result = __super::onWndProc(hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_VSCROLL:
				{
					// 縦スクロールに応じてスクロールしてからレイアウトを再計算します。
					scroll(SB_VERT, wParam);
					recalcLayout();

					break;
				}
			case WM_HSCROLL:
				{
					// 横スクロールに応じてスクロールしてからレイアウトを再計算します。
					scroll(SB_HORZ, wParam);
					recalcLayout();

					break;
				}
			case WM_MOUSEWHEEL:
				{
					MY_TRACE(_T("ScrollContainer::onWndProc(WM_MOUSEWHEEL, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// ホイールに応じてスクロールしてからレイアウトを再計算します。
					scroll(SB_VERT, ((int)wParam > 0) ? SB_PAGEUP : SB_PAGEDOWN);
					recalcLayout();

					break;
				}
			}

			return result;
		}
	};
}
