#pragma once

namespace apn::workspace
{
	//
	// このクラスはコンテナに格納されるシャトルです。
	//
	struct Shuttle : my::Window, Container::Content, std::enable_shared_from_this<Shuttle>
	{
		//
		// このクラスはシャトルのリスナーです。
		//
		struct Listener {
			//
			// ドッキングサイトからシャトルを切り離すときに呼ばれます。
			//
			virtual void release_shuttle(Shuttle* shuttle) = 0;

			//
			// ターゲットのテキストが変更されたときに呼ばれます。
			//
			virtual void on_change_text(Shuttle* shuttle, LPCTSTR new_text) = 0;

			//
			// ターゲットのフォーカスが切り替わったときに呼ばれます。
			//
			virtual void on_change_focus(Shuttle* shuttle) = 0;
		};

		//
		// このクラスはシャトルのスタティックリスナーです。
		//
		struct StaticListener {
			//
			// シャトルが作成されたときに呼ばれます。
			//
			virtual void on_init_shuttle(const std::shared_ptr<Shuttle>& shuttle) = 0;

			//
			// シャトルが破壊されたときに呼ばれます。
			//
			virtual void on_exit_shuttle(const std::shared_ptr<Shuttle>& shuttle) = 0;
		};

		inline static constexpr LPCTSTR c_prop_name = _T("apn::workspace::shuttle");

		inline static StaticListener* static_listener = 0; // このクラスのスタティックリスナーです。
		std::set<Listener*> listeners; // このクラスのリスナーです。
		std::wstring name; // このシャトルの名前です。
		std::shared_ptr<Container> dock_container; // このシャトル専用のドッキングコンテナです。
		std::shared_ptr<Container> float_container; // このシャトル専用のフローティングコンテナです。

		//
		// HWNDに関連付けられているシャトルを返します。
		//
		static Shuttle* get_pointer(HWND hwnd)
		{
			return (Shuttle*)::GetProp(hwnd, c_prop_name);
		}

		//
		// HWNDにシャトルを関連付けます。内部的に使用されます。
		//
		inline static BOOL set_pointer(HWND hwnd, Shuttle* shuttle)
		{
			return ::SetProp(hwnd, c_prop_name, shuttle);
		}

		//
		// 指定されたシャトル名を装飾して返します。
		//
		inline static std::wstring decorate_name(const std::wstring& name)
		{
			if (name == L"AviUtl" ||
				name == L"拡張編集" ||
				name == L"設定ダイアログ")
			{
				return L"* " + name;
			}

			return name;
		}

		//
		// 指定されたシャトル名の装飾を解除して返します。
		//
		inline static std::wstring strip_name(const std::wstring& name)
		{
			const std::wstring t(L"* ");

			if (name.starts_with(t))
				return name.substr(t.length());

			return name;
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
		virtual ~Shuttle() override
		{
			MY_TRACE_FUNC("{:#010x}, {}", (HWND)*this, name);
#if 0
			// ターゲットウィンドウの親ウィンドウを0にします。
			// これにより、コンテナウィンドウが破壊されてもターゲットウィンドウが道連れに破壊されずにすみます。
			::SetParent(*this, 0);
#endif
			// サブクラス化を解除して、これ以降の後始末処理を省略します。
			unsubclass();
		}

		//
		// 指定されたターゲットにアタッチします。
		// nameはシャトル名です。
		// hwndがターゲットです。
		//
		void init(const std::wstring& name, HWND hwnd)
		{
			MY_TRACE_FUNC("{}, {:#010x}", name, hwnd);

			// シャトル名をセットします。
			this->name = name;

			// ターゲットウィンドウをサブクラス化します。
			// ※すでにサブクラス化済みで失敗する場合もあります。
			subclass(hwnd);

			// HWNDにポインタを関連付けます。
			set_pointer(hwnd, this);

			// ターゲットウィンドウの表示状態を取得しておきます。
			auto visible = ::IsWindowVisible(*this);
			MY_TRACE_INT(visible);

			// ウィンドウの無駄な描画を抑制するため、
			// 一時的にターゲットウィンドウを非表示にします。
			::ShowWindow(*this, SW_HIDE);

			// ターゲットウィンドウのクライアント矩形を取得しておきます。
			auto rc_client = my::get_client_rect(*this);
			MY_TRACE_RECT2(rc_client);

			// クライアント矩形をスクリーン座標に変換しておきます。
			my::map_window_points(*this, 0, &rc_client);
			MY_TRACE_RECT2(rc_client);

			// 設定ダイアログの初期位置は縦に長過ぎたりするので微調整します。
			if (name == L"* 設定ダイアログ")
			{
				rc_client.right += ::GetSystemMetrics(SM_CXSIZEFRAME) * 2;
				rc_client.bottom /= 2;
			}

			// コンテナを作成します。
			dock_container = create_dock_container();
			float_container = create_float_container();

			// ターゲットのウィンドウスタイルを変更します。
			auto style = on_get_target_new_style();
			my::set_style(*this, style | WS_CHILD);
			my::modify_ex_style(*this, 0, WS_EX_NOPARENTNOTIFY);

			// ウィンドウスタイルが変更されたので
			// ターゲットのウィンドウ矩形を算出し、更新します。
			{
				auto style = my::get_style(*this);
				auto ex_style = my::get_ex_style(*this);
				auto menu = ::GetMenu(*this);

				auto rc_window = rc_client;
				::AdjustWindowRectEx(&rc_window, style, !!menu, ex_style);
				::SetWindowPos(*this, 0, 0, 0, my::get_width(rc_window), my::get_height(rc_window),
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_FRAMECHANGED);
			}

			// ターゲットのウィンドウ名を取得します。
			auto text = my::get_window_text(*this);

			// フローティングコンテナのウィンドウ名を変更します。
			::SetWindowTextW(*float_container, text.c_str());

			// ターゲットのメニューをフローティングコンテナに移し替えます。
			::SetMenu(*float_container, ::GetMenu(*this));

			// ターゲットの親をフローティングコンテナに変更します。
			::SetParent(*this, *float_container);

			// ターゲットがあった位置にフローティングコンテナを移動します。
			{
				auto rc_window = rc_client;
				my::client_to_window(*float_container, &rc_window);
				float_container->set_container_position(&rc_window);
			}

			// ターゲットが表示状態ならフローティングコンテナを表示します。
			if (visible)
			{
				::ShowWindow(*this, SW_SHOW); // この時点でフローティングコンテナも表示されているはずですが、
				::ShowWindow(*float_container, SW_SHOW); // 念のため::ShowWindow()を呼んでおきます。
			}

			if (static_listener)
				static_listener->on_init_shuttle(shared_from_this());
		}

		//
		// ドッキングコンテナを作成して返します。
		//
		virtual std::shared_ptr<Container> create_dock_container()
		{
			// ドッキングコンテナのスタイルを決定します。
			auto style = my::get_style(*this);
			decltype(style) dock_style = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			// ドッキングコンテナのタイプを決定します。
			if (style & WS_THICKFRAME)
				return std::make_shared<SpreadContainer>(this, dock_style);
			else
				return std::make_shared<ScrollContainer>(this, dock_style);
		}

		//
		// フローティングコンテナを作成して返します。
		//
		virtual std::shared_ptr<Container> create_float_container()
		{
			// フローティングコンテナのスタイルを決定します。
			auto style = my::get_style(*this);
			decltype(style) float_style = WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
			float_style |= style & (WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

			// フローティングコンテナのタイプを決定します。
			if (style & WS_THICKFRAME)
				return std::make_shared<SpreadContainer>(this, float_style);
			else
				return std::make_shared<Container>(this, float_style);
		}

		//
		// ターゲットウィンドウを表示します。
		//
		virtual void show_target_window()
		{
			MY_TRACE_FUNC("{}", name);

			// ターゲットウィンドウを表示します。
			::ShowWindow(*this, SW_SHOW);
		}

		//
		// ターゲットウィンドウに適用する新しいスタイルを返します。
		//
		virtual DWORD on_get_target_new_style()
		{
			// ※WS_CAPTIONを外さないとエディットボックスでマウス処理が行われなくなります。
			auto style = my::get_style(*this);
			style &= ~(WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
			return style;
		}

		//
		// シャトルの名前が変更されたときに呼ばれます。
		//
		virtual void on_rename()
		{
			::SetWindowTextW(*this, name.c_str());
		}

		//
		// ターゲットウィンドウのハンドルを返します。
		//
		HWND get_hwnd() override
		{
			return *this;
		}

		//
		// ターゲットウィンドウの位置を調整します。
		//
		BOOL revise_content_position(LPRECT rc) override
		{
			// スクロールバーがあるときは、その分矩形を縮小させます。

			auto style = my::get_style(*this);

			if (style & WS_VSCROLL) rc->right -= ::GetSystemMetrics(SM_CXVSCROLL);
			if (style & WS_HSCROLL) rc->bottom -= ::GetSystemMetrics(SM_CYHSCROLL);

			return TRUE;
		}

		//
		// ウィンドウ破壊時の最終処理を行います。
		//
		virtual void post_nc_destroy() override
		{
			MY_TRACE_FUNC("");

			__super::post_nc_destroy();

			if (static_listener)
				static_listener->on_exit_shuttle(shared_from_this());
		}

		//
		// ターゲットのウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("{}, {:#010x}, {:#010x}, {:#010x}, {:#010x}", name, hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("{}, WM_DESTROY, {:#010x}, {:#010x}", name, wParam, lParam);

					break;
				}
			case WM_NCDESTROY:
				{
					MY_TRACE_FUNC("{}, WM_NCDESTROY, {:#010x}, {:#010x}", name, wParam, lParam);

					break;
				}
			case WM_NCPAINT:
				{
					my::WindowDC dc(hwnd);

					auto rc = my::get_window_rect(hwnd);
					auto rc_clip = my::get_client_rect(hwnd);
					my::map_window_points(hwnd, 0, &rc_clip);
					::OffsetRect(&rc_clip, -rc.left, -rc.top);
					::OffsetRect(&rc, -rc.left, -rc.top);

					// クライアント領域を除外してから塗り潰します。
					my::gdi::unique_ptr<HRGN> rgn(::CreateRectRgnIndirect(&rc_clip));
					::ExtSelectClipRgn(dc, rgn.get(), RGN_DIFF);
					painter.fill_background(dc, &rc);
					::ExtSelectClipRgn(dc, 0, RGN_COPY);

					return 0;
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE_FUNC("{}, WM_SHOWWINDOW, {:#010x}, {:#010x}", name, wParam, lParam);

					auto container = get_current_container();
					if (container)
					{
						// ターゲットウィンドウの表示状態が変更された場合はコンテナもそれに追従させます。
						// ここで::ShowWindowAsync()を使用すると一部ウィンドウ(拡張編集)の表示がおかしくなります。
						// コンテナにWM_SHOWWINDOWを送信したくないので、::SetWindowPos()を使用しています。
						UINT flags = SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOCOPYBITS;
						if (wParam)
							flags |= SWP_SHOWWINDOW;
						else
							flags |= SWP_HIDEWINDOW;
						::SetWindowPos(*container, 0, 0, 0, 0, 0, flags);
					}

					break;
				}
			case WM_SETTEXT:
				{
					MY_TRACE_FUNC("{}, WM_SETTEXT, {:#010x}, {:#010x}", name, wParam, lParam);

					auto text = (LPCTSTR)lParam;

					auto container = get_current_container();
					if (container)
					{
						// コンテナのウィンドウテキストを変更します。
						::SetWindowText(*container, text);
					}

					// リスナーにテキストの変更を通知します。
					for (auto listener : listeners)
						listener->on_change_text(this, text);

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
					for (auto listener : listeners)
						listener->on_change_focus(this);

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
					MY_TRACE_FUNC("{}, WM_WINDOWPOSCHANGING, {:#010x}, {:#010x}", name, wParam, lParam);

					auto container = get_current_container();
					if (container)
					{
						auto wp = (WINDOWPOS*)lParam;
						if (!(wp->flags & SWP_NOMOVE) ||
							!(wp->flags & SWP_NOSIZE))
						{
							container->on_content_pos_changing(wp);
						}
					}

					break;
				}
			case WM_WINDOWPOSCHANGED:
				{
					MY_TRACE_FUNC("{}, WM_WINDOWPOSCHANGED, {:#010x}, {:#010x}", name, wParam, lParam);

					auto container = get_current_container();
					if (container)
					{
						auto wp = (WINDOWPOS*)lParam;
						if (!(wp->flags & SWP_NOMOVE) ||
							!(wp->flags & SWP_NOSIZE))
						{
							container->on_content_pos_changed(wp);
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// ターゲットをドッキング状態にします。
		//
		void dock_window(HWND site)
		{
			MY_TRACE_FUNC("{:#010x}", site);

			// フローティングコンテナを非表示にします。
			::ShowWindow(*float_container, SW_HIDE);

			// ドッキングコンテナの親ウィンドウを指定されたウィンドウに切り替えます。
			::SetParent(*dock_container, site);

			// ターゲットの親ウィンドウをドッキングコンテナに切り替えます。
			::SetParent(*this, *dock_container);
		}

		//
		// ドッキングコンテナをリサイズします。
		//
		void resize(LPCRECT rc)
		{
			dock_container->set_container_position(rc);
		}

		//
		// ターゲットをフローティング状態にします。
		//
		void float_window()
		{
			MY_TRACE_FUNC("");

			// ドッキングコンテナを非表示にします。
			::ShowWindow(*dock_container, SW_HIDE);

			// フローティングコンテナのテキストを更新します。
			::SetWindowTextW(*float_container,
				my::get_window_text(*this).c_str());

			// まず現在のターゲットの位置からフローティングコンテナの位置を算出します。
			float_container->on_float_container();

			// ターゲットの親ウィンドウをフローティングコンテナに切り替えます。
			::SetParent(*this, *float_container);

			// ドッキングコンテナの親ウィンドウをメインウィンドウに切り替えます。
			::SetParent(*dock_container, hive.main_window);
		}

		//
		// ドッキング状態ならTRUEを返します。
		//
		BOOL is_docking() const
		{
			if (!dock_container) return FALSE;

			return ::GetParent(*this) == *dock_container;
		}

		//
		// このシャトルが現在格納されているコンテナを返します。
		//
		std::shared_ptr<Container> get_current_container() const
		{
			return is_docking() ? dock_container : float_container;
		}

		//
		// 指定されたリスナーを追加します。
		//
		void add_listener(Listener* listener)
		{
			listeners.emplace(listener);
		}

		//
		// 指定されたリスナーを削除します。
		//
		void remove_listener(Listener* listener)
		{
			listeners.erase(listener);
		}

		//
		// シャトルのリリースをリスナーに通知します。
		//
		void fire_release_shuttle()
		{
			auto listeners = this->listeners;
			for (auto listener : listeners)
				listener->release_shuttle(this);
		}
	};
}
