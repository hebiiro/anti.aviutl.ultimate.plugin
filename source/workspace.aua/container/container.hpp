#pragma once

namespace apn::workspace
{
	//
	// このクラスはシャトルを格納するコンテナ(ドッキング＆フローティング兼用)です。
	//
	struct Container : my::Window
	{
		//
		// このクラスはコンテナのコンテンツです。
		//
		struct Content {
			//
			// コンテンツのウィンドウハンドルが必要なときに呼ばれます。
			//
			virtual HWND get_hwnd() = 0;

			//
			// コンテンツのウィンドウ位置を取得するために呼ばれます。
			//
			virtual BOOL revise_content_position(LPRECT rc) = 0;
		};

		//
		// コンテナのプロパティ名です。
		//
		inline static constexpr struct PropName {
			inline static constexpr auto c_type = _T("apn::workspace::container::type");
		} c_prop_name;

		//
		// コンテナのタイプです。
		//
		inline static constexpr struct Type {
			inline static constexpr int32_t c_dock = 1;
			inline static constexpr int32_t c_float = 2;
		} c_type;

		//
		// ウィンドウに関連付けられているコンテナタイプを返します。
		//
		inline static int32_t get_type(HWND hwnd)
		{
			return (int32_t)::GetProp(hwnd, c_prop_name.c_type);
		}

		//
		// 指定されたコンテナタイプをウィンドウに関連付けます。
		//
		inline static int32_t set_type(HWND hwnd, int32_t type)
		{
			return (int32_t)::SetProp(hwnd, c_prop_name.c_type, (HANDLE)type);
		}

		//
		// コンテナのウィンドウクラス名です。
		//
		inline static constexpr auto c_class_name = _T("apn::workspace::container");

		//
		// このコンテナに格納するコンテンツです。
		//
		Content* content = nullptr;

		//
		// 位置変更を抑制するためのロックカウントです。
		//
		LONG lock_count = 0;

		//
		// このクラスはコンテナをロックします。
		//
		struct Locker {
			Container* container = nullptr;
			Locker(Container* container) : container(container) { container->lock_count++; }
			~Locker() { container->lock_count--; }
		};

		//
		// コンテナがロックされている場合はTRUEを返します。
		//
		BOOL is_locked() { return lock_count != 0; }

		//
		// コンストラクタです。
		// コンテンツとウィンドウスタイルを受け取り、コンテナウィンドウを作成します。
		//
		Container(Content* content, DWORD style, DWORD ex_style)
			: content(content)
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", content, style, ex_style);

			create(
				ex_style,
				c_class_name,
				c_class_name,
				style,
				100, 100, 400, 400,
				hive.main_window, nullptr, hive.instance, nullptr);
		}

		//
		// デストラクタです。
		// コンテナウィンドウを削除します。
		//
		virtual ~Container() override
		{
			MY_TRACE_FUNC("");

			destroy();
		}

		//
		// コンテナのウィンドウクラスを登録します。
		//
		inline static BOOL register_window_class()
		{
			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = c_class_name;
			return !!::RegisterClass(&wc);
		}

		//
		// コンテナを正規化されたrcの位置に移動します。
		//
		virtual void set_container_position(LPCRECT rc)
		{
			MY_TRACE_FUNC("");

			auto x = rc->left;
			auto y = rc->top;
			auto w = rc->right - rc->left;
			auto h = rc->bottom - rc->top;

			x = std::min<int>(rc->left, rc->right);
			y = std::min<int>(rc->top, rc->bottom);
			w = std::max<int>(w, 0);
			h = std::max<int>(h, 0);

			set_window_pos_force(*this, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
		}

		//
		// コンテナがドッキング状態になるときに呼ばれます。
		//
		virtual void on_dock_container()
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// ロックします。
			Locker locker(this);

			// ターゲットの親ウィンドウを切り替えます。
			::SetParent(hwnd, *this);
		}

		//
		// コンテナがフローティング状態になるときに呼ばれます。
		//
		virtual void on_float_container()
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// ウィンドウタイトルを更新します。
			::SetWindowTextW(*this, my::get_window_text(hwnd).c_str());

			// コンテンツとコンテナのクライアント矩形が同じ位置になるように
			// コンテナのウィンドウ矩形を変更します。

			// コンテンツのクライアント矩形を取得します。
			auto rc = my::get_client_rect(hwnd);

			// クライアント矩形をスクリーン座標に変換します。
			my::map_window_points(::GetParent(hwnd), nullptr, &rc);

			// クライアント矩形をウィンドウ矩形に変換します。
			my::client_to_window(*this, &rc);

			// コンテナをrcの位置に移動します。
			set_container_position(&rc);

			// ロックします。
			Locker locker(this);

			// ターゲットの親ウィンドウを切り替えます。
			::SetParent(hwnd, *this);
		}

		//
		// コンテンツの位置を修正するために呼ばれます。
		//
		virtual BOOL revise_content_position(WINDOWPOS* wp)
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/hex}", wp->x, wp->y, wp->cx, wp->cy, wp->flags);

			// コンテンツの位置だけ変更します。サイズは変更しません。

			auto hwnd = content->get_hwnd();

			auto rc = RECT { 0, 0 };
			my::client_to_window(hwnd, &rc);
			content->revise_content_position(&rc);
			wp->x = rc.left;
			wp->y = rc.top;

			return TRUE;
		}

		//
		// ロックしてからコンテンツの位置を変更します。
		// すでにロックされている場合は何もしません。
		//
		BOOL set_content_position_with_lock()
		{
			MY_TRACE_FUNC("");

			// ロックされている場合は何もしません。
			if (is_locked()) return FALSE;

			// ロックします。
			Locker locker(this);

			// コンテンツの位置を変更します。
			return set_content_position();
		}

		//
		// コンテンツの位置を変更します。
		//
		virtual BOOL set_content_position()
		{
			MY_TRACE_FUNC("");

			// コンテンツのHWNDを取得します。
			auto hwnd = content->get_hwnd();

			// コンテンツのウィンドウ位置を取得します。
			auto rc = my::get_window_rect(hwnd);

			// スクリーン座標からクライアント座標に変換します。
			my::map_window_points(nullptr, *this, &rc);

			// コンテンツの位置を取得します。
			auto wp = rc_to_wp(&rc);
			revise_content_position(&wp);

			// コンテンツの位置を変更します。
			set_window_pos(hwnd, &wp);

			return TRUE;
		}

		//
		// ロックしてからコンテンツの位置にフィットするようにコンテナの位置を変更します。
		// すでにロックされている場合は何もしません。
		//
		BOOL fit_container_position_with_lock(const WINDOWPOS* content_wp)
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/hex}", content_wp->x, content_wp->y, content_wp->cx, content_wp->cy, content_wp->flags);

			// ロックされている場合は何もしません。
			if (is_locked()) return FALSE;

			// ロックします。
			Locker locker(this);

			// コンテナの位置を変更します。
			return fit_container_position(content_wp);
		}

		//
		// コンテンツの位置にフィットするようにコンテナの位置を変更します。
		//
		virtual BOOL fit_container_position(const WINDOWPOS* content_wp)
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/hex}", content_wp->x, content_wp->y, content_wp->cx, content_wp->cy, content_wp->flags);

			// コンテンツのウィンドウ矩形を取得します。
			auto rc = wp_to_rc(content_wp);

			// コンテンツのウィンドウ矩形をコンテンツのクライアント矩形に変換します。
			my::window_to_client(content->get_hwnd(), &rc);

			// コンテンツのクライアント矩形をコンテナのウィンドウ矩形に変換します。
			my::client_to_window(*this, &rc);

			// スクリーン座標をコンテナの親ウィンドウ座標に変換します。
			my::map_window_points(nullptr, ::GetParent(*this), &rc);

			// コンテナの位置を変更します。
			set_window_pos(*this, &rc, content_wp->flags);

			return TRUE;
		}

		//
		// コンテンツの位置が変更されるときに(コンテンツから)呼ばれます。
		//
		virtual BOOL on_content_pos_changing(WINDOWPOS* wp)
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/hex}", wp->x, wp->y, wp->cx, wp->cy, wp->flags);

			// ロックされている場合は何もしません。
			if (is_locked()) return FALSE;

			// 元のコンテンツの位置を取得しておきます。
			// この位置は主にAviUtlから指定された座標です。
			auto content_wp = *wp;

			// コンテンツの位置を修正します。
			revise_content_position(wp);

			// 元のコンテンツの位置に合わせてコンテナの位置も変更します。
			fit_container_position_with_lock(&content_wp);

			return TRUE;
		}

		//
		// コンテンツの位置が変更されたときに(コンテンツから)呼ばれます。
		//
		virtual BOOL on_content_pos_changed(WINDOWPOS* wp)
		{
			MY_TRACE_FUNC("{/}, {/}, {/}, {/}, {/hex}", wp->x, wp->y, wp->cx, wp->cy, wp->flags);

			// ロックされている場合は何もしません。
			if (is_locked()) return FALSE;

			return TRUE;
		}

		//
		// ウィンドウの識別名を返します。
		//
		virtual LPCTSTR get_window_id() override { return c_class_name; }

		//
		// コンテナのウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_PAINT:
				{
					// 背景色で塗りつぶします。

					my::PaintDC dc(hwnd);
					auto rc = my::get_client_rect(hwnd);

					painter.fill_background(dc, &rc);

					return 0;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {/}, ({/}, {/})", wParam, (short)LOWORD(lParam), (short)HIWORD(lParam));

					// コンテンツの位置を変更します。
					set_content_position_with_lock();

					break;
				}
			case WM_SETFOCUS:
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
				{
					// ターゲットウィンドウにフォーカスを当てます。
					::SetFocus(content->get_hwnd());

					break;
				}
			case WM_GETMINMAXINFO:
			case WM_ACTIVATE:
			case WM_COMMAND:
			case WM_CLOSE:
			case WM_CLEAR:
			case WM_COPY:
			case WM_CUT:
			case WM_PASTE:
//			case WM_APPCOMMAND:
				{
					// メッセージをそのままターゲットウィンドウに転送します。
					return ::SendMessage(content->get_hwnd(), message, wParam, lParam);
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE_FUNC("WM_SHOWWINDOW, {/hex}, {/hex}", wParam, lParam);

					// ::ShowWindow()の呼び出し以外がトリガーの場合は何もしません。
					if (lParam) break;

					// ターゲットウィンドウを取得します。
					auto target = content->get_hwnd();

					// ターゲットウィンドウがフィルタウィンドウの場合は
					auto fp = get_filter(content->get_hwnd());
					if (fp)
					{
						UINT check = 0;

						// ターゲットウィンドウの表示状態とAviUtl上での表示状態を同じにします。
						if (wParam)
						{
							fp->flag |= AviUtl::FilterPlugin::Flag::WindowActive;
							check |= MF_CHECKED;
						}
						else
						{
							fp->flag &= ~AviUtl::FilterPlugin::Flag::WindowActive;
							check |= MF_UNCHECKED;
						}

						::CheckMenuItem(fp->hmenu, fp->menu_index + 0x2AF9, check | MF_BYCOMMAND);
					}

					// コンテナの表示状態に合わせて、ターゲットウィンドウの表示状態も変更します。
					::ShowWindow(target, wParam ? SW_SHOW : SW_HIDE);

					if (fp)
					{
						// フィルタウィンドウに通知メッセージを送信します。
						::SendMessage(target, AviUtl::FilterPlugin::WindowMessage::ChangeWindow, 0, 0);
					}

					break;
				}
			case WM_SYSCOMMAND:
				{
					// システムコマンド以外の場合は
					if (wParam < 0xF000)
					{
						// メッセージをそのままターゲットウィンドウに転送します。
						return ::SendMessage(content->get_hwnd(), message, wParam, lParam);
					}

					break;
				}
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			case WM_DEADCHAR:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_SYSCHAR:
			case WM_SYSDEADCHAR:
				{
					// コンテナにフォーカスがあっても
					// ショートカットキーが使用できるように
					// メッセージをAviUtlウィンドウに転送します。
					return ::SendMessage(hive.aviutl_window, message, wParam, lParam);
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
					auto hwnd = content->get_hwnd();

					// マウス座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// コンテナの座標系からコンテンツの座標系へ変換します。
					my::map_window_points(*this, hwnd, &point);

					// コンテンツにメッセージを転送します。
					return ::SendMessage(hwnd, message, wParam, MAKELPARAM(point.x, point.y));
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};

	//
	// このクラスはコンテナを管理します。
	//
	inline struct ContainerManager
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// コンテナのウィンドウクラスを登録します。
			return Container::register_window_class();
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} container_manager;
}
