﻿#pragma once
#include "Container/Container.h"
#include "Container/SpreadContainer.h"
#include "Container/ScrollContainer.h"

namespace fgo::nest
{
	//
	// このクラスはコンテナに格納されるシャトルです。
	//
	struct Shuttle : Tools::Window, Container::Content, std::enable_shared_from_this<Shuttle>
	{
		//
		// シャトルの名前変更通知を受け取るだけのリスナー．
		//
		struct Prelistener {
			//
			// ターゲットのテキストが変更されたときに呼ばれます。
			//
			virtual void onChangeText(Shuttle* shuttle, LPCTSTR newText) = 0;
		};
		//
		// このクラスはシャトルのリスナーです。
		//
		struct Listener : Prelistener {
			//
			// ドッキングサイトのウィンドウハンドルが必要なときに呼ばれます。
			//
			virtual HWND getDockSite() = 0;

			//
			// ドッキングサイトからシャトルを切り離すときに呼ばれます。
			//
			virtual void releaseShuttle(Shuttle* shuttle) = 0;

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

		std::vector<std::weak_ptr<Prelistener>> title_sync_tokens{}; // タイトルテキストの同期先リストです．
		bool show_caption = true; // このシャトルのキャプションを表示にするかどうかのフラグです．

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
		// HWNDに対応するフィルタを返します。
		//
		static AviUtl::FilterPlugin* getFilter(HWND hwnd)
		{
			// magi.fpがまだ設定されていない場合は失敗します。
			if (!magi.fp) return 0;

			// AviUtl::SysInfoを取得します。
			AviUtl::SysInfo si = {};
			magi.fp->exfunc->get_sys_info(0, &si);

			// 全てのフィルタを列挙します。
			for (int i = 0; i < si.filter_n; i++)
			{
				// フィルタを取得します。
				auto fp = magi.fp->exfunc->get_filterp(i);
				if (!fp) continue; // フィルタが0の場合はスキップします。

				// フィルタとhwndが一致する場合はそのフィルタを返します。
				if (fp->hwnd == hwnd) return fp;
			}

			return 0;
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
		~Shuttle() override
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

			// ウィンドウの無駄な描画を抑制するため、
			// 一時的にターゲットウィンドウを非表示にします。
			::ShowWindow(*this, SW_HIDE);

			// ターゲットウィンドウのクライアント矩形を取得しておきます。
			RECT rcClient; ::GetClientRect(*this, &rcClient);
			MY_TRACE_RECT2(rcClient);

			// クライアント矩形をスクリーン座標に変換しておきます。
			::MapWindowPoints(*this, 0, (LPPOINT)&rcClient, 2);
			MY_TRACE_RECT2(rcClient);

			// 設定ダイアログの初期位置は縦に長過ぎたりするので微調整します。
			if (wcscmp(name, L"* 設定ダイアログ") == 0)
			{
				rcClient.right += ::GetSystemMetrics(SM_CXSIZEFRAME) * 2;
				rcClient.bottom /= 2;
			}

			// コンテナを作成します。
			dockContainer = createDockContainer();
			floatContainer = createFloatContainer();

			// ターゲットのウィンドウスタイルを変更します。
			DWORD style = onGetTargetNewStyle();
			setStyle(*this, style | WS_CHILD);
			modifyExStyle(*this, 0, WS_EX_NOPARENTNOTIFY);

			// ウィンドウスタイルが変更されたので
			// ターゲットのウィンドウ矩形を算出し、更新します。
			{
				DWORD style = getStyle(*this);
				DWORD exStyle = getExStyle(*this);
				HMENU menu = ::GetMenu(*this);

				RECT rcWindow = rcClient;
				::AdjustWindowRectEx(&rcWindow, style, !!menu, exStyle);
				::SetWindowPos(*this, 0, 0, 0, getWidth(rcWindow), getHeight(rcWindow),
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			}

			// ターゲットのウィンドウタイトルを取得します。
			TCHAR text[MAX_PATH] = {};
			::GetWindowText(*this, text, std::size(text));
			::SetWindowText(*floatContainer, text);

			// ターゲットのメニューをフローティングコンテナに移し替えます。
			::SetMenu(*floatContainer, ::GetMenu(*this));

			// ターゲットの親をフローティングコンテナに変更します。
			::SetParent(*this, *floatContainer);

			// ターゲットがあった位置にフローティングコンテナを移動します。
			{
				RECT rcWindow = rcClient;
				clientToWindow(*floatContainer, &rcWindow);
				floatContainer->setContainerPosition(&rcWindow);
			}

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
			DWORD style = getStyle(*this);
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
			DWORD style = getStyle(*this);
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
			// ※WS_CAPTIONを外さないとエディットボックスでマウス処理が行われなくなります。
			DWORD style = getStyle(*this);
			style &= ~(WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
			return style;
		}

		//
		// ターゲットウィンドウのハンドルを返します。
		//
		HWND getHWND() override
		{
			return *this;
		}

		//
		// ターゲットウィンドウの位置を調整します。
		//
		BOOL reviseContentPosition(LPRECT rc) override
		{
			// スクロールバーがあるときは、その分矩形を縮小させます。

			DWORD style = ::GetWindowLong(*this, GWL_STYLE);

			if (style & WS_VSCROLL) rc->right -= ::GetSystemMetrics(SM_CXVSCROLL);
			if (style & WS_HSCROLL) rc->bottom -= ::GetSystemMetrics(SM_CYHSCROLL);

			return TRUE;
		}

		//
		// ウィンドウ破壊時の最終処理を行います。
		//
		void postNcDestroy() override
		{
			MY_TRACE_FUNC("");

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

					// フィルタウィンドウかチェックします。
					auto fp = getFilter(hwnd);
					if (fp)
					{
						// wParamがTRUE、is_filter_window_disp()がFALSE、またはその逆の場合は
						if (!!wParam == !fp->exfunc->is_filter_window_disp(fp))
						{
							// WM_CLOSEを送信してAviUtlにフィルタウィンドウの表示状態を切り替えさせます。
							::SendMessage(*this, WM_CLOSE, 0, 0);
						}
					}

					auto container = getCurrentContainer();
					if (container)
					{
						// ターゲットウィンドウの表示状態が変更された場合はコンテナもそれに追従させます。
						// ここで::ShowWindowAsync()を使用すると一部ウィンドウ(拡張編集)の表示がおかしくなります。
						::ShowWindow(*container, wParam ? SW_SHOW : SW_HIDE);
					}

					break;
				}
			case WM_SETTEXT:
				{
					MY_TRACE_FUNC("%s, WM_SETTEXT, 0x%08X, 0x%08X", (LPCTSTR)name, wParam, lParam);

					LPCTSTR newText = (LPCTSTR)lParam;

					auto container = getCurrentContainer();
					if (container)
					{
						// コンテナのウィンドウテキストを変更します。
						::SetWindowText(*container, newText);
					}

					// リスナーにテキストの変更を通知します。
					if (listener)
						listener->onChangeText(this, newText);

					// タイトルテキストの同期先にも通知します．
					for (size_t i = title_sync_tokens.size(); i > 0;) {
						if (auto token = title_sync_tokens[--i].lock())
							token->onChangeText(this, newText);
						else title_sync_tokens.erase(title_sync_tokens.begin() + i);
					}

					break;
				}
			case WM_SETFOCUS:
			case WM_KILLFOCUS:
				{
					{
						// 「キーフレームプラグイン」用の処理です。
						// 子ウィンドウにするとWM_ACTIVATEが発生しなくなるので、
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
					auto mmi = (MINMAXINFO*)lParam;
					mmi->ptMaxTrackSize.y *= 3;

					break;
				}
			case WM_WINDOWPOSCHANGING:
				{
					MY_TRACE_FUNC("%s, WM_WINDOWPOSCHANGING, 0x%08X, 0x%08X", (LPCTSTR)name, wParam, lParam);

					auto container = getCurrentContainer();
					if (container)
					{
						auto wp = (WINDOWPOS*)lParam;
						if (!(wp->flags & SWP_NOMOVE) ||
							!(wp->flags & SWP_NOSIZE))
						{
							container->onContentPosChanging(wp);
						}
					}

					break;
				}
			case WM_WINDOWPOSCHANGED:
				{
					MY_TRACE_FUNC("%s, WM_WINDOWPOSCHANGED, 0x%08X, 0x%08X", (LPCTSTR)name, wParam, lParam);

					auto container = getCurrentContainer();
					if (container)
					{
						auto wp = (WINDOWPOS*)lParam;
						if (!(wp->flags & SWP_NOMOVE) ||
							!(wp->flags & SWP_NOSIZE))
						{
							container->onContentPosChanged(wp);
						}
					}

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
			dockContainer->setContainerPosition(rc);
		}

		//
		// ターゲットをフローティング状態にします。
		//
		void floatWindow()
		{
			MY_TRACE_FUNC("");

			// フローティングコンテナのテキストを更新します。
			TCHAR text[MAX_PATH] = {};
			::GetWindowText(*this, text, std::size(text));
			::SetWindowText(*floatContainer, text);

			// まず現在のターゲットの位置からフローティングコンテナの位置を算出します。
			floatContainer->onFloatContainer();

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
		// ドッキング状態ならTRUEを返します。
		//
		BOOL isDocking() const
		{
			if (!dockContainer) return FALSE;

			return ::GetParent(*this) == *dockContainer;
		}

		//
		// このシャトルが現在格納されているコンテナを返します。
		//
		std::shared_ptr<Container> getCurrentContainer() const
		{
			return isDocking() ? dockContainer : floatContainer;
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

		//
		// タイトルテキスト同期先のリストに追加します．
		//
		void addTitleSyncToken(const auto& token)
		{
			// ついでに肥大化しないように音信不通な同期先は間引きます．
			for (size_t i = title_sync_tokens.size(); i > 0;) {
				if (title_sync_tokens[--i].expired())
					title_sync_tokens.erase(title_sync_tokens.begin() + i);
			}
			title_sync_tokens.emplace_back(token);
			title_sync_tokens.shrink_to_fit();
		}
	};
}
