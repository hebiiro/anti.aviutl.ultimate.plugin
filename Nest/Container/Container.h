#pragma once
#include "Hive.h"

namespace fgo::nest
{
	//
	// このクラスはシャトルを格納するドッキングコンテナとフローティングコンテナ(兼用)です。
	//
	struct Container : Tools::Window
	{
		//
		// このクラスはコンテナのリスナーです。
		//
		struct Listener {
			//
			// ターゲットのウィンドウハンドルが必要なときに呼ばれます。
			//
			virtual HWND getTarget() = 0;

			//
			// ターゲットのウィンドウ位置を調整するときに呼ばれます。
			//
			virtual void onSetTargetWindowPos(LPRECT rc) = 0;

			//
			// コンテナのウィンドウプロシージャから呼ばれます。
			//
			virtual LRESULT onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* skipDefault) = 0;
		};

		inline static const LPCTSTR ClassName = _T("Nest.Container");

		Listener* listener = 0; // このクラスのリスナーです。

		//
		// コンストラクタです。
		// リスナーとウィンドウスタイルを受け取り、コンテナウィンドウを作成します。
		//
		Container(Listener* listener, DWORD style)
			: listener(listener)
		{
			create(
				WS_EX_NOPARENTNOTIFY,
				ClassName,
				ClassName,
				style,
				100, 100, 200, 200,
				hive.mainWindow, 0, hive.instance, 0);
		}

		//
		// デストラクタです。
		// コンテナウィンドウを削除します。
		//
		virtual ~Container()
		{
			destroy();
		}

		static BOOL registerWndClass()
		{
			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = ClassName;
			return !!::RegisterClass(&wc);
		}

		//
		// ドッキングコンテナを正規化されたrcの位置に移動します。
		//
		virtual void onResizeDockContainer(LPCRECT rc)
		{
			int x = rc->left;
			int y = rc->top;
			int w = rc->right - rc->left;
			int h = rc->bottom - rc->top;

			x = std::min<int>(rc->left, rc->right);
			y = std::min<int>(rc->top, rc->bottom);
			w = std::max<int>(w, 0);
			h = std::max<int>(h, 0);

			hive.forceSetWindowPos(*this, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
		}

		//
		// ターゲットとコンテナのクライアント矩形が同じ位置になるように
		// フローティングコンテナのウィンドウ矩形を調整します。
		//
		virtual void onResizeFloatContainer()
		{
			// リスナーからターゲットウィンドウを取得します。
			HWND target = listener->getTarget();

			// ターゲットウィンドウのクライアント矩形を取得します。
			RECT rc; ::GetClientRect(target, &rc);

			// クライアント矩形をスクリーン座標に変換します。
			::MapWindowPoints(::GetParent(target), 0, (LPPOINT)&rc, 2);

			// クライアント矩形をウィンドウ矩形に変換します。
			clientToWindow(*this, &rc);

			int x = rc.left;
			int y = rc.top;
			int w = getWidth(rc);
			int h = getHeight(rc);

			hive.forceSetWindowPos(*this, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
		}

		//
		// ターゲットのウィンドウ矩形からコンテナのウィンドウ矩形を算出します。
		//
		virtual BOOL onSetContainerPos(WINDOWPOS* wp)
		{
			// リスナーからターゲットウィンドウを取得します。
			HWND target = listener->getTarget();

			// ターゲットのウィンドウ矩形(スクリーン座標)です。
			RECT rc = { wp->x, wp->y, wp->x + wp->cx, wp->y + wp->cy };

			// ターゲットのウィンドウ矩形をクライント矩形に変換します。
			windowToClient(target, &rc);

			// ターゲットのクライアント矩形をコンテナのウィンドウ矩形に変換します。
			clientToWindow(*this, &rc);

			// 算出されたコンテナのウィンドウ矩形でwpを更新します。
			wp->x = rc.left;
			wp->y = rc.top;
			wp->cx = getWidth(rc);
			wp->cy = getHeight(rc);

			return TRUE;
		}

		//
		// ターゲットのウィンドウプロシージャです。
		//
		virtual LRESULT onTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* skipDefault)
		{
			return 0;
		}

		//
		// ウィンドウの識別名を返します。
		//
		LPCTSTR getWindowId() override { return ClassName; }

		//
		// コンテナのウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			{
				// 先にリスナーにメッセージを処理させます。

				BOOL skipDefault = FALSE;
				LRESULT lr = listener->onContainerWndProc(
					this, hwnd, message, wParam, lParam, &skipDefault);
				if (skipDefault) return lr;
			}

			switch (message)
			{
			case WM_PAINT:
				{
					// 背景色で塗りつぶします。

					PAINTSTRUCT ps = {};
					HDC dc = ::BeginPaint(hwnd, &ps);
					hive.fillBackground(dc, &ps.rcPaint);
					::EndPaint(hwnd, &ps);
					return 0;
				}
			case WM_SIZE:
				{
					// ターゲットウィンドウの位置だけ変更します。サイズは変更しません。

					HWND target = listener->getTarget();

					RECT rc = { 0, 0 };
					clientToWindow(target, &rc);
					listener->onSetTargetWindowPos(&rc);
					int x = rc.left;
					int y = rc.top;

					hive.true_SetWindowPos(target, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

					break;
				}
			case WM_SETFOCUS:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				{
					// ターゲットウィンドウにフォーカスを当てます。
					::SetFocus(listener->getTarget());

					break;
				}
			case WM_ACTIVATE:
			case WM_COMMAND:
			case WM_CLOSE:
				{
					// メッセージをそのままターゲットウィンドウに転送します。
					return ::SendMessage(listener->getTarget(), message, wParam, lParam);
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
