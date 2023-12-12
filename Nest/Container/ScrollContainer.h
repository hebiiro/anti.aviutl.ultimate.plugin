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
		ScrollContainer(Content* content, DWORD style)
			: Container(content, style)
		{
		}

		//
		// デストラクタです。
		//
		~ScrollContainer() override
		{
		}

		//
		// スクロール範囲を更新します。
		//
		BOOL updateScrollBar()
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			HWND hwnd = content->getHWND();

			// コンテンツのクライアント矩形を取得します。
			RECT rc; ::GetClientRect(hwnd, &rc);

			// コンテンツのクライアント矩形の幅と高さを算出します。
			int contentSize[2] = { getWidth(rc), getHeight(rc) };

			// コンテナのクライアント矩形を取得します。
			RECT rcContainer; ::GetClientRect(*this, &rcContainer);

			// スクロールバーの有無によってコンテナのクライアント矩形を調整します。
			DWORD style = getStyle(*this);
			if (style & WS_VSCROLL) rcContainer.right += ::GetSystemMetrics(SM_CXVSCROLL);
			if (style & WS_HSCROLL) rcContainer.bottom += ::GetSystemMetrics(SM_CYHSCROLL);

			// コンテナのクライアント矩形の幅と高さを算出します。
			int containerSize[2] = { getWidth(rcContainer), getHeight(rcContainer) };

			constexpr int h = 0;
			constexpr int v = 1;

			if (contentSize[h] > containerSize[h])
			{
				// コンテンツの横幅の方が大きいので水平スクロールバーが必要です。
				// なので、コンテナの縦幅を水平スクロールバーの太さの分だけ小さくします。
				containerSize[v] -= ::GetSystemMetrics(SM_CYHSCROLL);

				if (contentSize[v] > containerSize[v])
				{
					// コンテンツの縦幅の方が大きいので垂直スクロールバーが必要です。
					// なので、コンテナの横幅を垂直スクロールバーの太さの分だけ小さくします。
					containerSize[h] -= ::GetSystemMetrics(SM_CXVSCROLL);
				}
			}
			else if (contentSize[v] > containerSize[v])
			{
				// コンテンツの縦幅の方が大きいので垂直スクロールバーが必要です。
				// なので、コンテナの横幅を垂直スクロールバーの太さの分だけ小さくします。
				containerSize[h] -= ::GetSystemMetrics(SM_CXVSCROLL);

				if (contentSize[h] > containerSize[h])
				{
					// コンテンツの横幅の方が大きいので水平スクロールバーが必要です。
					// なので、コンテナの縦幅を水平スクロールバーの太さの分だけ小さくします。
					containerSize[v] -= ::GetSystemMetrics(SM_CYHSCROLL);
				}
			}

			// スクロール範囲を変更します。
			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_PAGE | SIF_RANGE;
			si.nMin = 0;
			for (size_t i = 0; i < std::size(contentSize); i++) {
				si.nMax = contentSize[i] - 1;
				si.nPage = containerSize[i];
				::SetScrollInfo(*this, i, &si, TRUE);
			}

			// NC領域の変更を通知します。
			::SetWindowPos(*this, 0, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED);

			return TRUE;
		}

		//
		// スクロール位置を変更します。
		//
		void scroll(int bar, WPARAM wParam, int delta = 1)
		{
			MY_TRACE_FUNC("");

			SCROLLINFO si = { sizeof(si) };
			si.fMask = SIF_POS | SIF_RANGE;
			::GetScrollInfo(*this, bar, &si);

			switch (LOWORD(wParam))
			{
			case SB_LEFT:      si.nPos = si.nMin; break;
			case SB_RIGHT:     si.nPos = si.nMax; break;
			case SB_LINELEFT:  si.nPos += -10 * delta; break;
			case SB_LINERIGHT: si.nPos +=  10 * delta; break;
			case SB_PAGELEFT:  si.nPos += -60 * delta; break;
			case SB_PAGERIGHT: si.nPos +=  60 * delta; break;
			case SB_THUMBTRACK:
			case SB_THUMBPOSITION: si.nPos = HIWORD(wParam); break;
			}

			::SetScrollInfo(*this, bar, &si, TRUE);
		}

		//
		// コンテンツの位置を更新します(位置のみ変更します)。
		//
		BOOL updateContentPos()
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			HWND hwnd = content->getHWND();

			// スクロール量を取得します。
			int scrollPos[2] = {};
			for (size_t i = 0; i < std::size(scrollPos); i++)
				scrollPos[i] = ::GetScrollPos(*this, i);

			// コンテナのクライアント矩形(の左上座標)を取得します。
			RECT rc = { 0, 0 };

			// クライアント矩形をウィンドウ矩形に変換します。
			clientToWindow(hwnd, &rc);

			// ウィンドウ矩形の左上の座標を取得します。
			int pos[2] = { rc.left, rc.top };

			// 座標をスクロールの分だけずらします。
			for (size_t i = 0; i < std::size(pos); i++)
				pos[i] -= scrollPos[i];

			// コンテンツの位置を変更します。
			::SetWindowPos(hwnd, 0, pos[0], pos[1], 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

			// 強制再描画でスクロールに素早く追従させます。
			::UpdateWindow(hwnd);

			return TRUE;
		}

		//
		// コンテナを正規化されたrcの位置に移動します。
		//
		void setContainerPosition(LPCRECT rc) override
		{
			MY_TRACE_FUNC("%d, %d, %d, %d", rc->left, rc->top, rc->right, rc->bottom);

			// ロックされている場合は何もしません。
			if (locks()) return;

			// ロックします。
			Locker locker(this);

			// コンテンツのHWNDを取得します。
			HWND hwnd = content->getHWND();

			// コンテンツが格納されている現在のコンテナを取得します。
			// (ドッキング完了後に呼び出された場合はparentはこのコンテナになります)
			HWND parent = ::GetParent(hwnd);

			// 現在のコンテナのスクロール位置を取得しておきます。
			int scrollPos[2] = {};
			for (size_t i = 0; i < std::size(scrollPos); i++)
				scrollPos[i] = ::GetScrollPos(parent, i);

			// このコンテナの位置を変更します。
			__super::setContainerPosition(rc);

			// このコンテナのサイズが変更されたのでスクロール範囲を更新します。
			// スクロール位置を更新する前にこの処理を行う必要があります。
			updateScrollBar();

			// 現在のコンテナのスクロール位置をこのコンテナに適用します。
			for (size_t i = 0; i < std::size(scrollPos); i++)
				::SetScrollPos(*this, i, scrollPos[i], TRUE);

			// スクロール位置が更新されたのでコンテンツの位置を更新します。
			updateContentPos();
		}

		//
		// コンテナがフローティング状態になるときに呼ばれます。
		//
		void onFloatContainer() override
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			HWND hwnd = content->getHWND();

			// コンテンツが格納されている現在のコンテナ(ドッキングコンテナ)を取得します。
			HWND parent = ::GetParent(hwnd);

			// 現在のコンテナのクライアント矩形を取得します。
			RECT rc; ::GetClientRect(parent, &rc);

			// クライント座標からスクリーン座標に変換します。
			::MapWindowPoints(parent, 0, (LPPOINT)&rc, 2);

			// 現在のコンテナのクライアント矩形をこのコンテナのウィンドウ矩形に変換します。
			clientToWindow(*this, &rc);

			// このコンテナの位置を変更します。
			setContainerPosition(&rc);
		}

		//
		// コンテンツの位置を修正するために呼ばれます。
		//
		BOOL reviseContentPosition(WINDOWPOS* wp) override
		{
			MY_TRACE_FUNC("");

			if (!__super::reviseContentPosition(wp)) return FALSE;

			// コンテンツの位置をスクロールの分だけずらします。
			wp->x -= ::GetScrollPos(*this, SB_HORZ);
			wp->y -= ::GetScrollPos(*this, SB_VERT);

			return TRUE;
		}

		//
		// コンテンツの位置を変更します。
		//
		BOOL setContentPosition() override
		{
			MY_TRACE_FUNC("");

			// スクロールバーとコンテナの位置を更新します。
			updateScrollBar();
			updateContentPos();

			return TRUE;
		}

		//
		// コンテンツの位置にフィットするようにコンテナの位置を変更します。
		//
		BOOL fitContainerPosition(const WINDOWPOS* content_wp) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンテンツの位置が変更されたときに(コンテンツから)呼ばれます。
		//
		BOOL onContentPosChanged(WINDOWPOS* wp) override
		{
			MY_TRACE_FUNC("");

			if (!__super::onContentPosChanged(wp)) return FALSE;

			updateScrollBar();

			return TRUE;
		}

		//
		// コンテナのウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			LRESULT result = __super::onWndProc(hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_VSCROLL:
				{
					// 縦スクロールに応じてスクロールしてからコンテンツの位置を更新します。
					Locker lcoker(this);
					scroll(SB_VERT, wParam);
					updateContentPos();

					break;
				}
			case WM_HSCROLL:
				{
					// 横スクロールに応じてスクロールしてからコンテンツの位置を更新します。
					Locker lcoker(this);
					scroll(SB_HORZ, wParam);
					updateContentPos();

					break;
				}
			case WM_MOUSEWHEEL:
				{
					MY_TRACE_FUNC("WM_MOUSEWHEEL, 0x%08X, 0x%08X", wParam, lParam);

					// ホイールに応じてスクロールしてからコンテンツの位置を更新します。
					Locker lcoker(this);
					scroll(SB_VERT, SB_PAGEUP, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
					updateContentPos();

					break;
				}
			case WM_MOUSEHWHEEL:
				{
					MY_TRACE_FUNC("WM_MOUSEHWHEEL, 0x%08X, 0x%08X", wParam, lParam);

					// ホイールに応じてスクロールしてからコンテンツの位置を更新します。
					Locker lcoker(this);
					scroll(SB_HORZ, SB_PAGELEFT, GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
					updateContentPos();

					break;
				}
			}

			return result;
		}
	};
}
