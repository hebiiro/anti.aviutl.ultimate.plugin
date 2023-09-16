#pragma once
#include "Container/Container.h"
#include "Container/SpreadContainer.h"
#include "Container/ScrollContainer.h"

namespace fgo::nest
{
	//
	// このクラスはコンテナに格納されるシャトルです。
	//
	struct Shuttle : Tools::Window, Container::Listener, std::enable_shared_from_this<Shuttle>
	{
		//
		// このクラスはシャトルのリスナーです。
		//
		struct Listener {
			//
			// ドッキングサイトのウィンドウハンドルが必要なときに呼ばれます。
			//
			virtual HWND getDockSite() = 0;

			//
			// ドッキングサイトからシャトルを切り離すときに呼ばれます。
			//
			virtual void releaseShuttle(Shuttle* shuttle) = 0;

			//
			// ターゲットのテキストが変更されたときに呼ばれます。
			//
			virtual void onChangeText(Shuttle* shuttle, LPCTSTR newText) = 0;

			//
			// ターゲットのフォーカスが切り替わったときに呼ばれます。
			//
			virtual void onChangeFocus(Shuttle* shuttle) = 0;
		};

		//
		// このクラスはシャトルのスタティックリスナーです。
		//
		struct StaticListener {
			//
			// シャトルが作成されたときに呼ばれます。
			//
			virtual void onInitShuttle(const std::shared_ptr<Shuttle>& shuttle) = 0;

			//
			// シャトルが破壊されたときに呼ばれます。
			//
			virtual void onExitShuttle(const std::shared_ptr<Shuttle>& shuttle) = 0;
		};

		inline const static LPCTSTR PropName = _T("Nest.Shuttle");

		inline static StaticListener* staticListener = 0; // このクラスのスタティックリスナーです。
		Listener* listener = 0; // このクラスのリスナーです。
		_bstr_t name = L""; // このシャトルの名前です。
		std::shared_ptr<Container> dockContainer; // このシャトル専用のドッキングコンテナです。
		std::shared_ptr<Container> floatContainer; // このシャトル専用のフローティングコンテナです。

		//
		// HWNDに関連付けられているシャトルを返します。
		//
		static Shuttle* getPointer(HWND hwnd)
		{
			return (Shuttle*)::GetProp(hwnd, PropName);
		}

		//
		// HWNDにシャトルを関連付けます。内部的に使用されます。
		//
		static BOOL setPointer(HWND hwnd, Shuttle* shuttle)
		{
			return ::SetProp(hwnd, PropName, shuttle);
		}

		//
		// コンストラクタです。
		//
		Shuttle()
		{
		}

		//
		// デストラクタです。
		//
		virtual ~Shuttle()
		{
			MY_TRACE_FUNC("0x%08X, %ws", (HWND)*this, (BSTR)name);

			// サブクラス化を解除して、これ以降の後始末処理を省略します。
			unsubclass();

			// ターゲットウィンドウの親ウィンドウを0にします。
			// これにより、コンテナウィンドウが破壊されてもターゲットウィンドウが道連れに破壊されずにすみます。
			::SetParent(*this, 0);
		}

		//
		// 指定されたターゲットにアタッチします。
		// nameはシャトル名です。
		// hwndがターゲットです。
		//
		void init(const _bstr_t& name, HWND hwnd)
		{
			MY_TRACE_FUNC("%s, 0x%08X", (LPCTSTR)name, hwnd);

			// シャトル名をセットします。
			this->name = name;

			// ターゲットウィンドウをサブクラス化します。
			// ※すでにサブクラス化済みで失敗する場合もあります。
			subclass(hwnd);

			// HWNDにポインタを関連付けます。
			setPointer(hwnd, this);

			// ターゲットウィンドウの表示状態を取得しておきます。
			BOOL visible = ::IsWindowVisible(*this);
			MY_TRACE_INT(visible);

			// ターゲットウィンドウのウィンドウ矩形を取得しておきます。
			RECT rc; ::GetWindowRect(*this, &rc);
			MY_TRACE_RECT2(rc);
			int x = rc.left;
			int y = rc.top;
			int w = getWidth(rc);
			int h = getHeight(rc);

			// コンテナを作成します。
			dockContainer = createDockContainer();
			floatContainer = createFloatContainer();

			// ターゲットのウィンドウスタイルを変更します。
			DWORD style = onGetTargetNewStyle();
			::SetWindowLong(*this, GWL_STYLE, style | WS_CHILD);
			modifyExStyle(*this, 0, WS_EX_NOPARENTNOTIFY);

			// ターゲットのウィンドウタイトルを取得します。
			TCHAR text[MAX_PATH] = {};
			::GetWindowText(*this, text, MAX_PATH);
			::SetWindowText(*floatContainer, text);

			// ウィンドウの無駄な描画を抑制するため、
			// 一時的にターゲットウィンドウを非表示にします。
			::ShowWindow(*this, SW_HIDE);

			// ターゲットの親をフローティングコンテナに変更します。
			::SetParent(*this, *floatContainer);

			// ::SetWindowPos() を呼び出し、フック処理を促します。
			// コンテナの種類によってはコンテナのウィンドウサイズが調整されます。
			::SetWindowPos(*this, 0, x, y, w, h,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

			// ターゲットが表示状態ならフローティングコンテナを表示します。
			if (visible)
			{
				::ShowWindow(*this, SW_SHOW); // この時点でフローティングコンテナも表示されているはずですが、
				::ShowWindow(*floatContainer, SW_SHOW); // 念のため::ShowWindow()を呼んでおきます。
			}

			if (staticListener)
				staticListener->onInitShuttle(shared_from_this());
		}

		//
		// ドッキングコンテナを作成して返します。
		//
		virtual std::shared_ptr<Container> createDockContainer()
		{
			// ドッキングコンテナのスタイルを決定します。
			DWORD style = ::GetWindowLong(*this, GWL_STYLE);
			DWORD dockStyle = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			// ドッキングコンテナのタイプを決定します。
			if (style & WS_THICKFRAME)
				return std::make_shared<SpreadContainer>(this, dockStyle);
			else
				return std::make_shared<ScrollContainer>(this, dockStyle);
		}

		//
		// フローティングコンテナを作成して返します。
		//
		virtual std::shared_ptr<Container> createFloatContainer()
		{
			// フローティングコンテナのスタイルを決定します。
			DWORD style = ::GetWindowLong(*this, GWL_STYLE);
			DWORD floatStyle = WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
			floatStyle |= style & (WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

			// フローティングコンテナのタイプを決定します。
			if (style & WS_THICKFRAME)
				return std::make_shared<SpreadContainer>(this, floatStyle);
			else
				return std::make_shared<Container>(this, floatStyle);
		}

		//
		// ターゲットウィンドウを表示します。
		//
		virtual void showTargetWindow()
		{
			MY_TRACE_FUNC("%s", (LPCTSTR)name);

			// ターゲットウィンドウを表示します。
			::ShowWindow(*this, SW_SHOW);
		}

		//
		// ターゲットウィンドウに適用する新しいスタイルを返します。
		//
		virtual DWORD onGetTargetNewStyle()
		{
			// ※ WS_CAPTION を外さないとエディットボックスでマウス処理が行われなくなります。
			DWORD style = ::GetWindowLong(*this, GWL_STYLE);
			style &= ~(WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
			return style;
		}

		//
		// ターゲットウィンドウのハンドルを返します。
		//
		HWND getTarget() override
		{
			return *this;
		}

		//
		// ターゲットウィンドウの位置を調整します。
		//
		void onSetTargetWindowPos(LPRECT rc) override
		{
			// スクロールバーがあるときは、その分矩形を縮小させます。

			DWORD style = ::GetWindowLong(*this, GWL_STYLE);

			if (style & WS_VSCROLL) rc->right -= ::GetSystemMetrics(SM_CXVSCROLL);
			if (style & WS_HSCROLL) rc->bottom -= ::GetSystemMetrics(SM_CYHSCROLL);
		}

		//
		// コンテナのウィンドウプロシージャです。
		//
		LRESULT onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* skipDefault) override
		{
			return 0;
		}

		//
		// ウィンドウ破壊時の最終処理を行います。
		//
		void postNcDestroy() override
		{
			MY_TRACE(_T("Shuttle::postNcDestroy()\n"));

			__super::postNcDestroy();

			if (staticListener)
				staticListener->onExitShuttle(shared_from_this());
		}

		//
		// ターゲットのウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("%s, 0x%08X, 0x%08X, 0x%08X, 0x%08X", (LPCTSTR)name, hwnd, message, wParam, lParam);

			auto container = Tools::Window::fromHandle<Container>(::GetParent(hwnd), Container::ClassName);
			if (container)
			{
				// 先にコンテナにメッセージを処理させます。

				BOOL skipDefault = FALSE;
				LRESULT lr = container->onTargetWndProc(
					hwnd, message, wParam, lParam, &skipDefault);
				if (skipDefault) return lr;
			}

			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("%s, WM_DESTROY, 0x%08X, 0x%08X", (LPCTSTR)name, wParam, lParam);

					break;
				}
			case WM_NCDESTROY:
				{
					MY_TRACE_FUNC("%s, WM_NCDESTROY, 0x%08X, 0x%08X", (LPCTSTR)name, wParam, lParam);

					break;
				}
			case WM_NCPAINT:
				{
					// クライアント領域を除外してから塗り潰します。
					HDC dc = ::GetWindowDC(hwnd);
					RECT rc; ::GetWindowRect(hwnd, &rc);
					RECT rcClip; ::GetClientRect(hwnd, &rcClip);
					::MapWindowPoints(hwnd, 0, (LPPOINT)&rcClip, 2);
					::OffsetRect(&rcClip, -rc.left, -rc.top);
					::OffsetRect(&rc, -rc.left, -rc.top);
					HRGN rgn = ::CreateRectRgnIndirect(&rcClip);
					::ExtSelectClipRgn(dc, rgn, RGN_DIFF);
					::DeleteObject(rgn);
					hive.fillBackground(dc, &rc);
					::ExtSelectClipRgn(dc, 0, RGN_COPY);
					::ReleaseDC(hwnd, dc);
					return 0;
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE_FUNC("%s, WM_SHOWWINDOW, 0x%08X, 0x%08X", (LPCTSTR)name, wParam, lParam);

					// ターゲットウィンドウの表示状態が変更された場合はコンテナもそれに追従させます。
					::ShowWindowAsync(::GetParent(hwnd), wParam ? SW_SHOW : SW_HIDE);

					break;
				}
			case WM_SETTEXT:
				{
					MY_TRACE_FUNC("%s, WM_SETTEXT, 0x%08X, 0x%08X", (LPCTSTR)name, wParam, lParam);

					LPCTSTR newText = (LPCTSTR)lParam;

					// コンテナのウィンドウテキストを変更します。
					::SetWindowText(::GetParent(hwnd), newText);

					// リスナーにテキストの変更を通知します。
					if (listener)
						listener->onChangeText(this, newText);

					break;
				}
			case WM_SETFOCUS:
			case WM_KILLFOCUS:
				{
					{
						// 「キーフレームプラグイン」用の処理です。
						// 子ウィンドウにすると WM_ACTIVATE が発生しなくなるので、
						// ここで手動で発生させます。
						WPARAM active = (message == WM_SETFOCUS) ? WA_ACTIVE : WA_INACTIVE;
						::SendMessage(hwnd, WM_ACTIVATE, active, 0);
					}

					// リスナーにフォーカスの変更を通知します。
					if (listener)
						listener->onChangeFocus(this);

					break;
				}
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				{
					// ターゲットウィンドウがクリックされたらフォーカスが当たるようにします。
					::SetFocus(hwnd);

					break;
				}
			case WM_GETMINMAXINFO:
				{
					// 設定ダイアログは縦に長くなるので、
					// ウィンドウの限界サイズを増やしておきます。
					MINMAXINFO* mmi = (MINMAXINFO*)lParam;
					mmi->ptMaxTrackSize.y *= 3;

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// ターゲットをドッキング状態にします。
		//
		void dockWindow(HWND site)
		{
			MY_TRACE_FUNC("0x%08X", site);

			// ドッキングコンテナの親ウィンドウを指定されたウィンドウに切り替えます。
			::SetParent(*dockContainer, site);

			// ターゲットの親ウィンドウをドッキングコンテナに切り替えます。
			::SetParent(*this, *dockContainer);

			// フローティングコンテナを非表示にし、ドッキングコンテナを表示します。
			::ShowWindow(*floatContainer, SW_HIDE);
			::ShowWindow(*dockContainer, SW_SHOW);
		}

		//
		// ドッキングコンテナをリサイズします。
		//
		void resize(LPCRECT rc)
		{
			dockContainer->onResizeDockContainer(rc);
		}

		//
		// ターゲットをフローティング状態にします。
		//
		void floatWindow()
		{
			MY_TRACE_FUNC("");

			// フローティングコンテナのテキストを更新します。
			TCHAR text[MAX_PATH] = {};
			::GetWindowText(*this, text, MAX_PATH);
			::SetWindowText(*floatContainer, text);

			// まず現在のターゲットの位置からフローティングコンテナの位置を算出します。
			floatContainer->onResizeFloatContainer();

			// ターゲットの親ウィンドウをフローティングコンテナに切り替えます。
			::SetParent(*this, *floatContainer);

			// ドッキングコンテナを非表示にし、フローティングコンテナを表示します。
			::ShowWindow(*dockContainer, SW_HIDE);
			if (::IsWindowVisible(*this))
				::ShowWindow(*floatContainer, SW_SHOW);

			// ドッキングコンテナの親ウィンドウをメインウィンドウに切り替えます。
			::SetParent(*dockContainer, hive.mainWindow);
		}

		//
		// リスナーを返します。
		//
		Listener* getListener() const
		{
			return listener;
		}

		//
		// リスナーを指定されたリスナーに変更します。
		//
		void setListener(Listener* listener)
		{
			this->listener = listener;
		}
	};
}
