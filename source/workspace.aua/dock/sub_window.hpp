#pragma once

namespace apn::workspace
{
	//
	// このクラスはサブウィンドウです。
	// このウィンドウはドッキング機能を有しながら、
	// このウィンドウ自体を他のドックサイトにドッキングさせることもできます。
	//
	struct SubWindow : DockSite<Shuttle>
	{
		//
		// このクラスはタイマーのIDです。
		//
		inline static constexpr struct TimerID {
			inline static constexpr UINT_PTR c_update_interceptors = 1000;
			inline static constexpr UINT_PTR c_update_window_text = 1001;
		} c_timer_id;

		//
		// このクラスのインスタンスのコレクションです。
		//
		inline static std::set<SubWindow*> collection;

		//
		// コンストラクタです。
		//
		SubWindow()
		{
			MY_TRACE_FUNC("");

			// インスタンスをコレクションに追加します。
			collection.emplace(this);
		}

		//
		// デストラクタです。
		//
		virtual ~SubWindow() override
		{
			MY_TRACE_FUNC("");

			// インスタンスをコレクションから削除します。
			collection.erase(this);
		}

		//
		// すべてのサブウィンドウを削除します。
		//
		inline static void clear()
		{
			// サブウィンドウを削除する度にコレクションの個数が変わってしまうので、まずコピーを作ります。
			auto copy = collection;
			for (auto sub_window : copy)
			{
				// 無駄な描画を減らすためにウィンドウを非表示にします。
				::ShowWindow(*sub_window, SW_HIDE);

				// サブウィンドウを削除します。
				sub_window->destroy();
			}

			// 念のためコレクションを空にします。
			collection.clear();
		}

		//
		// 流動的なサブウィンドウを削除します。
		//
		inline static void clear_fluids()
		{
			// サブウィンドウを削除する度にコレクションの個数が変わってしまうので、まずコピーを作ります。
			auto copy = collection;
			for (auto sub_window : copy)
			{
				// ルートペインを取得します。
				auto root = sub_window->get_root_pane(*sub_window);
				if (!root) continue;

				// ペインのレイアウトが固体化されている場合は何もしません。
				if (root->is_solid) continue;

				// 無駄な描画を減らすためにウィンドウを非表示にします。
				::ShowWindow(*sub_window, SW_HIDE);

				// サブウィンドウを削除します。
				sub_window->destroy();
			}
		}

		//
		// 使用可能なシャトル名を返します。
		//
		inline static std::wstring get_available_name()
		{
			for (size_t i = 0; TRUE; i++)
			{
				auto name = my::format(L"サブ{/}", i + 1);
				MY_TRACE_STR(name);

				if (!shuttle_manager.get(name)) return name;
			}
		}

		//
		// 指定されたウィンドウがサブウィンドウの場合はTRUEを返します。
		//
		inline static BOOL is_sub_window(HWND hwnd)
		{
			return is_dock_site(hwnd);
		}

		//
		// ウィンドウを作成します。
		//
		BOOL create(const std::wstring& name, HWND parent)
		{
			MY_TRACE_FUNC("{/}, {/hex}", name, parent);

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = hive.c_class_name.c_sub_window;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				wc.lpszClassName,
				name.c_str(),
				WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, nullptr, wc.hInstance, nullptr);
		}

		//
		// ターゲットにアタッチします。
		//
		void init(const std::wstring& name, HWND hwnd)
		{
			MY_TRACE_FUNC("");

			__super::init(name, hwnd);

			// フローティングコンテナのアイコンを設定します。
			auto icon = (HICON)::GetClassLong(hive.aviutl_window, GCL_HICON);
			::SendMessage(*float_container, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			::SendMessage(*float_container, WM_SETICON, ICON_BIG, (LPARAM)icon);

			// フローティングコンテナのシステムメニューに項目を追加します。
			auto menu = ::GetSystemMenu(*float_container, FALSE);
			::InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING, c_command_id.c_rename_sub_window, _T("名前を変更"));
			::InsertMenu(menu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);

			// ウィンドウ名の更新を促します。
			// ただし、この時点では他のシャトルの作成が完了していないので、
			// ここではメッセージを送信するだけにして、それをあとからハンドルします。
			fire_update_interceptors();
		}

		//
		// シャトルの名前が変更されたときに呼ばれます。
		//
		virtual void on_rename() override
		{
			MY_TRACE_FUNC("");

			// シャトルの名前が変更されたので
			// インターセプターを再設置します。
			update_interceptors();

			// ウィンドウ名を更新します。
			update_window_text();
		}

		//
		// インターセプターを再設置します。
		//
		void update_interceptors()
		{
			MY_TRACE_FUNC("");

			// 一旦すべてのインターセプターを削除します。
			interceptors.clear();

			// このサブウィンドウのシャトル名を取得します。
			auto text = this->name;

			// シャトル名に入っている他のシャトルの名前を検索します。
			std::wregex regex(L"%(.+?)%");
			std::wsmatch results;
			auto start = text.cbegin();
			while (std::regex_search(start, text.cend(), results, regex))
			{
				// 検索結果である他のシャトルの名前を取得します。
				auto result = results[1].str();

				// 装飾されたシャトル名を取得します。
				auto name = Shuttle::decorate_name(result);

				// 他のシャトルが見つかった場合は
				auto shuttle = shuttle_manager.get(name.c_str());
				if (shuttle && !is_sub_window(*shuttle)) // 循環参照になる可能性があるのでサブウィンドウは除外します。
				{
					// インターセプターを作成してメッセージを傍受します。
					interceptors.emplace_back(std::make_shared<Interceptor>(this, shuttle, result));
				}

				// 次の検索位置に進めます。
				start = results[0].second;
			}
		}

		//
		// ウィンドウ名を更新します。
		// 自分または他のシャトルの名前が変更されたときに呼び出してください。
		//
		void update_window_text()
		{
			MY_TRACE_FUNC("");

			// このサブウィンドウのシャトル名を取得します。
			auto text = this->name;

			for (auto interceptor : interceptors)
			{
				// textの中にある他のシャトルの名前を含んだパターンを
				// そのシャトルのウィンドウ名に置換します。

				auto src = interceptor->pattern;
				auto dst = my::get_window_text(*interceptor->shuttle);

				text = my::replace(text, src, dst);
			}

			// ウィンドウ名を変更します。
			::SetWindowTextW(*this, text.c_str());
		}

		//
		// インターセプター更新メッセージを送信します。
		//
		void fire_update_interceptors()
		{
			::SetTimer(*this, c_timer_id.c_update_interceptors, 0, nullptr);
		}

		//
		// ウィンドウ名更新メッセージを送信します。
		//
		void fire_update_window_text()
		{
			::SetTimer(*this, c_timer_id.c_update_window_text, 0, nullptr);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("{/hex}, WM_CREATE, {/hex}, {/hex}", hwnd,  wParam, lParam);

					// このウィンドウにカテゴリ名を設定します。
					::SetProp(hwnd, _T("aviutl.plugin.category_name"), (HANDLE)L"サブウィンドウ");

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("{/hex}, WM_CLOSE, {/hex}, {/hex}", hwnd,  wParam, lParam);

					if (::GetKeyState(VK_SHIFT) < 0)
					{
						// Shiftキーが押されている場合はサブウィンドウを削除します。

						// このサブウィンドウを削除していいのかどうかをユーザーに問い合わせます。
						if (IDYES != hive.message_box(L"サブウィンドウを削除しますか？", hwnd, MB_YESNO))
							return 0;

						// ウィンドウを破壊します。
						// インスタンスもここで削除されます。
						destroy();
					}
					else
					{
						// サブウィンドウの表示状態をトグルで切り替えます。

						if (::IsWindowVisible(hwnd))
						{
							::ShowWindow(hwnd, SW_HIDE);
						}
						else
						{
							::ShowWindow(hwnd, SW_SHOW);
						}
					}

					if (!::IsWindowVisible(hwnd))
					{
						// 何もしないとメインウィンドウが他のウィンドウの後ろに隠れてしまうので、
						// 手動でフォアグラウンドにします。
						::SetActiveWindow(hive.main_window);
					}

					return 0;
				}
			case WM_SYSCOMMAND: // フローティングコンテナのシステムメニューコマンドをハンドルします。
				{
					switch (wParam)
					{
					case c_command_id.c_rename_sub_window:
						{
							// サブウィンドウの名前変更ダイアログを表示します。
							shuttle_manager.show_rename_dialog(this, hwnd);

							break;
						}
					}

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_KILLFOCUS:
				{
					MY_TRACE_FUNC("WM_KILLFOCUS, {/hex}, {/hex}", wParam, lParam);

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
					// サブウィンドウにフォーカスがあっても
					// ショートカットキーが使用できるように
					// メッセージをAviUtlウィンドウに転送します。
					return ::SendMessage(hive.aviutl_window, message, wParam, lParam);
				}
			case WM_TIMER:
				{
					switch (wParam)
					{
					case c_timer_id.c_update_interceptors:
						{
							// インターセプターを更新します。
							update_interceptors();

							// インターセプターが更新されたので
							// それに合わせてウィンドウ名を更新します。
							update_window_text();

							// タイマーを終了します。
							::KillTimer(hwnd, wParam);

							break;
						}
					case c_timer_id.c_update_window_text:
						{
							// 他のシャトルの名前が変更されたので
							// それに合わせてウィンドウ名を更新します。
							update_window_text();

							// タイマーを終了します。
							::KillTimer(hwnd, wParam);

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// このクラスは他のシャトルのメッセージを傍受します。
		//
		struct Interceptor : my::WindowInterceptor
		{
			//
			// メッセージを受信するサブウィンドウです。
			//
			SubWindow* sub_window;

			//
			// メッセージを傍受されるシャトルです。
			//
			std::shared_ptr<Shuttle> shuttle;

			//
			// 文字列からシャトルを識別するためのパターンです。
			//
			std::wstring pattern;

			//
			// コンストラクタです。
			//
			Interceptor(SubWindow* sub_window, const std::shared_ptr<Shuttle>& shuttle, const std::wstring& name)
				: sub_window(sub_window)
				, shuttle(shuttle)
				, pattern(L"%" + name + L"%")
			{
				subclass(*shuttle);
			}

			//
			// デストラクタです。
			//
			virtual ~Interceptor() override
			{
				unsubclass();
			}

			//
			// ウィンドウプロシージャです。
			//
			virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_SETTEXT:
					{
						MY_TRACE_FUNC("{/hex}, WM_SETTEXT, {/hex}, {/hex}", hwnd,  wParam, lParam);

						// サブウィンドウにタイマーメッセージを送信します。
						sub_window->fire_update_window_text();

						break;
					}
				}

				return __super::on_wnd_proc(hwnd, message, wParam, lParam);
			}
		};

		//
		// インターセプターのコレクションです。
		//
		std::vector<std::shared_ptr<Interceptor>> interceptors;
	};

	inline struct SubWindowManager
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return clear();
		}

		//
		// すべてのサブウィンドウを削除します。
		//
		BOOL clear()
		{
			MY_TRACE_FUNC("");

			SubWindow::clear();

			return TRUE;
		}

		//
		// 流動的なサブウィンドウを削除します。
		//
		BOOL clear_fluids()
		{
			MY_TRACE_FUNC("");

			SubWindow::clear_fluids();

			return TRUE;
		}
	} sub_window_manager;
}
