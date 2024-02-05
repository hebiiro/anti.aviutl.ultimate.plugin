#pragma once
#include "Hive.h"

namespace fgo::nest
{
	//
	// このクラスはシャトルを格納するコンテナ(ドッキング＆フローティング兼用)です。
	//
	struct Container : Tools::Window
	{
		//
		// このクラスはコンテナのコンテンツです。
		//
		struct Content {
			//
			// コンテンツのウィンドウハンドルが必要なときに呼ばれます。
			//
			virtual HWND getHWND() = 0;

			//
			// コンテンツのウィンドウ位置を取得するために呼ばれます。
			//
			virtual BOOL reviseContentPosition(LPRECT rc) = 0;
		};

		inline static const LPCTSTR ClassName = _T("Nest.Container");

		//
		// このコンテナに格納するコンテンツです。
		//
		Content* content = 0;

		//
		// 位置変更を抑制するためのロックカウントです。
		//
		LONG lockCount = 0;

		//
		// このクラスはコンテナをロックします。
		//
		struct Locker {
			Container* container = 0;
			Locker(Container* container) : container(container) { container->lockCount++; }
			~Locker() { container->lockCount--; }
		};

		//
		// コンテナがロックされている場合はTRUEを返します。
		//
		BOOL locks() { return lockCount != 0; }

		//
		// コンストラクタです。
		// コンテンツとウィンドウスタイルを受け取り、コンテナウィンドウを作成します。
		//
		Container(Content* content, DWORD style)
			: content(content)
		{
			create(
				WS_EX_NOPARENTNOTIFY,
				ClassName,
				ClassName,
				style,
				100, 100, 400, 400,
				hive.mainWindow, 0, hive.instance, 0);
		}

		//
		// デストラクタです。
		// コンテナウィンドウを削除します。
		//
		~Container() override
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
		// 強制的にWM_SIZEを送信する::SetWindowPos()です。
		//
		static BOOL forceSetWindowPos(HWND hwnd, HWND insertAfter, int x, int y, int w, int h, UINT flags)
		{
			RECT rcOld; ::GetWindowRect(hwnd, &rcOld);
			BOOL result = ::SetWindowPos(hwnd, insertAfter, x, y, w, h, flags);

			if (!(flags & SWP_NOSIZE))
			{
				RECT rcNew; ::GetWindowRect(hwnd, &rcNew);

				// ウィンドウサイズが変更されなかった場合は
				if (getWidth(rcOld) == getWidth(rcNew) && getHeight(rcOld) == getHeight(rcNew))
				{
					// 手動でWM_SIZEを送信します。
					::SendMessage(hwnd, WM_SIZE, 0, 0);
				}
			}

			return result;
		}

		static BOOL setWindowPos(HWND hwnd, const WINDOWPOS* wp)
		{
			return ::SetWindowPos(hwnd, wp->hwndInsertAfter, wp->x, wp->y, wp->cx, wp->cy, wp->flags);
		}

		static BOOL setWindowPos(HWND hwnd, LPCRECT rc, UINT flags = SWP_NOZORDER | SWP_NOACTIVATE)
		{
			return ::SetWindowPos(hwnd, 0, rc->left, rc->top, getWidth(*rc), getHeight(*rc), flags);
		}

		static WINDOWPOS rc2wp(LPCRECT rc, UINT flags = SWP_NOZORDER | SWP_NOACTIVATE)
		{
			return { 0, 0, rc->left, rc->top, getWidth(*rc), getHeight(*rc), flags };
		}

		static RECT wp2rc(const WINDOWPOS* wp)
		{
			return { wp->x, wp->y, wp->x + wp->cx, wp->y + wp->cy };
		}

		//
		// コンテナを正規化されたrcの位置に移動します。
		//
		virtual void setContainerPosition(LPCRECT rc)
		{
			MY_TRACE_FUNC("");

			int x = rc->left;
			int y = rc->top;
			int w = rc->right - rc->left;
			int h = rc->bottom - rc->top;

			x = std::min<int>(rc->left, rc->right);
			y = std::min<int>(rc->top, rc->bottom);
			w = std::max<int>(w, 0);
			h = std::max<int>(h, 0);

			forceSetWindowPos(*this, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
		}

		//
		// コンテナがフローティング状態になるときに呼ばれます。
		//
		virtual void onFloatContainer()
		{
			MY_TRACE_FUNC("");

			// コンテンツとコンテナのクライアント矩形が同じ位置になるように
			// コンテナのウィンドウ矩形を変更します。

			// コンテンツのHWNDを取得します。
			HWND hwnd = content->getHWND();

			// コンテンツのクライアント矩形を取得します。
			RECT rc; ::GetClientRect(hwnd, &rc);

			// クライアント矩形をスクリーン座標に変換します。
			::MapWindowPoints(::GetParent(hwnd), 0, (LPPOINT)&rc, 2);

			// クライアント矩形をウィンドウ矩形に変換します。
			clientToWindow(*this, &rc);

			// コンテナをrcの位置に移動します。
			setContainerPosition(&rc);
		}

		//
		// コンテンツの位置を修正するために呼ばれます。
		//
		virtual BOOL reviseContentPosition(WINDOWPOS* wp)
		{
			MY_TRACE_FUNC("");

			// コンテンツの位置だけ変更します。サイズは変更しません。

			HWND hwnd = content->getHWND();

			RECT rc = { 0, 0 };
			clientToWindow(hwnd, &rc);
			content->reviseContentPosition(&rc);
			wp->x = rc.left;
			wp->y = rc.top;

			return TRUE;
		}

		//
		// ロックしてからコンテンツの位置を変更します。
		// すでにロックされている場合は何もしません。
		//
		BOOL setContentPositionWithLock()
		{
			MY_TRACE_FUNC("");

			// ロックされている場合は何もしません。
			if (locks()) return FALSE;

			// ロックします。
			Locker locker(this);

			// コンテンツの位置を変更します。
			return setContentPosition();
		}

		//
		// コンテンツの位置を変更します。
		//
		virtual BOOL setContentPosition()
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			HWND hwnd = content->getHWND();

			// コンテンツのウィンドウ位置を取得します。
			RECT rc; ::GetWindowRect(hwnd, &rc);

			// スクリーン座標からクライアント座標に変換します。
			::MapWindowPoints(0, *this, (LPPOINT)&rc, 2);

			// コンテンツの位置を取得します。
			WINDOWPOS wp = rc2wp(&rc);
			reviseContentPosition(&wp);

			// コンテンツの位置を変更します。
			setWindowPos(hwnd, &wp);

			return TRUE;
		}

		//
		// ロックしてからコンテンツの位置にフィットするようにコンテナの位置を変更します。
		// すでにロックされている場合は何もしません。
		//
		BOOL fitContainerPositionWithLock(const WINDOWPOS* content_wp)
		{
			MY_TRACE_FUNC("");

			// ロックされている場合は何もしません。
			if (locks()) return FALSE;

			// ロックします。
			Locker locker(this);

			// コンテナの位置を変更します。
			return fitContainerPosition(content_wp);
		}

		//
		// コンテンツの位置にフィットするようにコンテナの位置を変更します。
		//
		virtual BOOL fitContainerPosition(const WINDOWPOS* content_wp)
		{
			MY_TRACE_FUNC("");

			// コンテンツのウィンドウ矩形を取得します。
			RECT rc = wp2rc(content_wp);

			// ウィンドウ矩形をクライアント矩形に変換します。
			windowToClient(content->getHWND(), &rc);

			// クライアント座標からスクリーン座標に変換します。
			::MapWindowPoints(*this, 0, (LPPOINT)&rc, 2);

			// クライアント矩形をウィンドウ矩形に変換します。
			clientToWindow(*this, &rc);

			// コンテナの位置を変更します。
			setWindowPos(*this, &rc, content_wp->flags);

			return TRUE;
		}

		//
		// コンテンツの位置が変更されるときに(コンテンツから)呼ばれます。
		//
		virtual BOOL onContentPosChanging(WINDOWPOS* wp)
		{
			MY_TRACE_FUNC("");

			// ロックされている場合は何もしません。
			if (locks()) return FALSE;

			// 元のコンテンツの位置を取得しておきます。
			// この位置は主にAviUtlから指定された座標です。
			WINDOWPOS content_wp = *wp;

			// コンテンツの位置を修正します。
			reviseContentPosition(wp);

			// 元のコンテンツの位置に合わせてコンテナの位置も変更します。
			fitContainerPositionWithLock(&content_wp);

			return TRUE;
		}

		//
		// コンテンツの位置が変更されたときに(コンテンツから)呼ばれます。
		//
		virtual BOOL onContentPosChanged(WINDOWPOS* wp)
		{
			MY_TRACE_FUNC("");

			// ロックされている場合は何もしません。
			if (locks()) return FALSE;

			return TRUE;
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
					MY_TRACE_FUNC("WM_SIZE");

					// コンテンツの位置を変更します。
					setContentPositionWithLock();

					break;
				}
			case WM_SETFOCUS:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				{
					// ターゲットウィンドウにフォーカスを当てます。
					::SetFocus(content->getHWND());

					break;
				}
			case WM_ACTIVATE:
			case WM_COMMAND:
			case WM_CLOSE:
				{
					// メッセージをそのままターゲットウィンドウに転送します。
					return ::SendMessage(content->getHWND(), message, wParam, lParam);
				}
			case WM_SYSCOMMAND:
				{
					// システムコマンド以外の場合は
					if (wParam < 0xF000)
					{
						// メッセージをそのままターゲットウィンドウに転送します。
						return ::SendMessage(content->getHWND(), message, wParam, lParam);
					}

					break;
				}
			}

			switch (message)
			{
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
				{
					// コンテンツのHWNDを取得します。
					HWND hwnd = content->getHWND();

					// マウス座標を取得します。
					POINT point = LP2PT(lParam);

					// コンテナの座標系からコンテンツの座標系へ変換します。
					::MapWindowPoints(*this, hwnd, &point, 1);

					// コンテンツにメッセージを転送します。
					return ::SendMessage(hwnd, message, wParam, MAKELPARAM(point.x, point.y));
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
