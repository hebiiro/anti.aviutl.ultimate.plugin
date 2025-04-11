#pragma once

namespace apn::workspace
{
	//
	// このクラスはサブプロセスウィンドウを保持するシャトルです。
	//
	struct SubProcess : Shuttle
	{
		//
		// このクラスはタイマーのIDです。
		//
		inline static constexpr struct TimerID {
			inline static constexpr UINT_PTR c_init = 2024;
		} c_timer_id;

		//
		// このクラスのインスタンスのコレクションです。
		//
		inline static std::set<SubProcess*> collection;

		//
		// サブプロセスウィンドウのクラス名です。
		//
		std::wstring class_name;

		//
		// サブプロセスウィンドウのウィンドウ名です。
		//
		std::wstring window_name;

		//
		// プロセス終了時のモードです。
		//
		std::unordered_set<std::wstring> exit_mode;

		//
		// サブプロセスのプロセスIDです。
		//
		DWORD pid = 0;

		//
		// サブプロセスのプロセスハンドルです。
		//
		my::handle::unique_ptr<> process_handle;

		//
		// サブプロセスのウィンドウです。
		//
		HWND window = nullptr;

		//
		// コンストラクタです。
		//
		SubProcess()
		{
			MY_TRACE_FUNC("");

			// インスタンスをコレクションに追加します。
			collection.emplace(this);
		}

		//
		// デストラクタです。
		//
		virtual ~SubProcess() override
		{
			MY_TRACE_FUNC("");

			// インスタンスをコレクションから削除します。
			collection.erase(this);

			// サブプロセスウィンドウを終了します。
			if (::IsWindow(window))
			{
				if (exit_mode.contains(L"detach"))
				{
					// サブプロセスウィンドウの親ウィンドウをnullptrにします。
					// これにより、シャトルが破壊されても
					// サブプロセスウィンドウが道連れに破壊されずにすみます。
					::SetParent(window, nullptr);
				}

				if (exit_mode.contains(L"close")) ::PostMessage(window, WM_CLOSE, 0, 0);
				if (exit_mode.contains(L"quit")) ::PostMessage(window, WM_QUIT, 0, 0);
				if (exit_mode.contains(L"destroy")) ::DestroyWindow(window);
			}

			if (exit_mode.contains(L"terminate")) ::TerminateProcess(process_handle.get(), 0);
		}

		//
		// すべてのサブプロセスを削除します。
		//
		inline static void clear()
		{
			// サブプロセスを削除する度にコレクションの個数が変わってしまうので、まずコピーを作ります。
			auto copy = collection;
			for (auto sub_process : copy)
			{
				// 無駄な描画を減らすためにウィンドウを非表示にします。
				::ShowWindow(*sub_process, SW_HIDE);

				// サブプロセスを削除します。
				sub_process->destroy();
			}

			// 念のためコレクションを空にします。
			collection.clear();
		}

		//
		// ウィンドウ(サブプロセスシャトル)を作成します。
		// このウィンドウがサブプロセスウィンドウを内包します。
		//
		BOOL create(const std::shared_ptr<Hive::SubProcess::Node>& node)
		{
			MY_TRACE_FUNC("{/}", node->name);

			class_name = node->class_name;
			window_name = node->window_name;
			exit_mode = node->exit_mode;

			// ウィンドウクラスを登録します。
			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = hive.c_class_name.c_sub_process;
			::RegisterClass(&wc);

			// ウィンドウを作成します。
			if (!__super::create(
				WS_EX_NOPARENTNOTIFY,
				wc.lpszClassName,
				node->name.c_str(),
				WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				hive.main_window, nullptr, hive.instance, nullptr))
			{
				MY_TRACE("サブプロセスシャトルの作成に失敗しました\n");

				return FALSE;
			}

			// シャトルを初期化します。
			__super::init(node->name, *this);

			// サブプロセスを起動します。
			if (!node->path.empty())
			{
				std::filesystem::path path(node->path);
				auto dir = path.parent_path();

				SHELLEXECUTEINFO sei = { sizeof(sei) };
				sei.lpFile = path.c_str();
				sei.lpDirectory = dir.c_str();
				sei.nShow = SW_HIDE;
				sei.fMask = SEE_MASK_NOCLOSEPROCESS;
				if (::ShellExecuteEx(&sei))
				{
					pid = ::GetProcessId(sei.hProcess);
					process_handle.reset(sei.hProcess);
				}
			}

			return TRUE;
		}

		//
		// 表示/非表示状態をトグルで切り替えます。
		//
		void show()
		{
			MY_TRACE_FUNC("");

			::SendMessage(*this, WM_CLOSE, 0, 0);
		}

		// サブプロセスウィンドウをクライアント領域全体まで広げます。
		// ウィンドウ位置の変更は非同期で処理されるので、デッドロックは起きないはずです。
		void resize()
		{
			MY_TRACE_FUNC("");

			auto rc = my::get_client_rect(*this);
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);
			::SetWindowPos(window, nullptr, 0, 0, w, h, SWP_NOZORDER | SWP_ASYNCWINDOWPOS);
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
					MY_TRACE_FUNC("{/hex}, WM_CREATE, {/hex}, {/hex}", hwnd, wParam, lParam);

					// このウィンドウにカテゴリ名を設定します。
					::SetProp(hwnd, _T("aviutl.plugin.category_name"), (HANDLE)L"サブプロセス");

					// このタイミングではまだサブプロセスウィンドウが
					// 作成されていないかもしれないので、
					// タイマーで遅らせて初期化処理を行います。
					::SetTimer(hwnd, c_timer_id.c_init, 1000, nullptr);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("{/hex}, WM_DESTROY, {/hex}, {/hex}", hwnd, wParam, lParam);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("{/hex}, WM_CLOSE, {/hex}, {/hex}", hwnd, wParam, lParam);

					// ウィンドウの表示状態をトグルで切り替えます。
					if (::IsWindowVisible(hwnd))
					{
						::ShowWindowAsync(hwnd, SW_HIDE);
					}
					else
					{
						::ShowWindowAsync(hwnd, SW_SHOW);
					}

					return 0;
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE_FUNC("{/hex}, WM_SHOWWINDOW, {/hex}, {/hex}", hwnd, wParam, lParam);

					// このウィンドウが表示されるとき、サブプロセスウィンドウも表示状態にします。
					if (wParam && ::IsWindow(window))
						::ShowWindowAsync(window, SW_RESTORE);

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("{/hex}, WM_SIZE, {/hex}, {/hex}", hwnd, wParam, lParam);

					// サブプロセスウィンドウが有効ならリサイズします。
					if (::IsWindow(window))
						resize();

					break;
				}
			case WM_TIMER:
				{
					switch (wParam)
					{
					case c_timer_id.c_init:
						{
							// 初期化を実行します。
							// 成功するまでタイマーで繰り返し実行されます。
							if (on_init())
							{
								// 初期化が完了したのでタイマーを終了します。
								::KillTimer(hwnd, wParam);
							}

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// この仮想関数はサブプロセスウィンドウを初期化するために呼ばれます。
		//
		virtual BOOL on_init()
		{
			MY_TRACE_FUNC("");

			// サブプロセスウィンドウを検索します。
			if (!on_find_window()) return FALSE;

			MY_TRACE("サブプロセスウィンドウが見つかりました => {/hex}\n", window);
#if 0
			auto menu = ::GetMenu(window);
			auto result = ::SetMenu(hwnd, menu);
#endif
			// サブプロセスウィンドウのスタイルを変更します。
			DWORD remove = WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
			DWORD add = WS_CHILD;
			my::modify_style(window, remove, add);
			my::modify_ex_style(window, 0, WS_EX_NOPARENTNOTIFY);

			// サブプロセスウィンドウの親ウィンドウを変更します。
			::SetParent(window, hwnd);

			// サブプロセスウィンドウを表示します。
			// 最小化されている場合は元に戻します。
			::ShowWindowAsync(window, SW_RESTORE);

			// WM_SIZEをポストしてresize()を実行します。
			::PostMessage(hwnd, WM_SIZE, 0, 0);

			return TRUE;
		}

		//
		// この仮想関数はサブプロセスウィンドウを検索するために呼ばれます。
		//
		virtual BOOL on_find_window()
		{
			MY_TRACE_FUNC("");

			::EnumWindows(
				[](HWND hwnd, LPARAM lParam) -> BOOL
				{
					MY_TRACE_HWND(hwnd);

					auto p = (SubProcess*)lParam;

					// hwndが目的のウィンドウかどうかチェックします。
					// hwndが目的のウィンドウではない場合はTRUEを返して次のウィンドウに進めます。

					if (p->pid)
					{
						// プロセスIDをチェックします。
						auto pid = DWORD {};
						auto tid = ::GetWindowThreadProcessId(hwnd, &pid);
						if (p->pid != pid) return TRUE;
					}

					if (!p->window_name.empty())
					{
						// ウィンドウ名をチェックします。
						auto window_name = my::get_window_text(hwnd);
						if (window_name.find(p->window_name) == window_name.npos) return TRUE;
					}

					if (!p->class_name.empty())
					{
						// クラス名をチェックします。
						auto class_name = my::get_class_name(hwnd);
						if (class_name.find(p->class_name) == class_name.npos) return TRUE;
					}

					p->window = hwnd;

					// 目的のウィンドウが見つかったので、FALSEを返して列挙を終了します。
					return FALSE;
				},
				(LPARAM)this);

			return window != nullptr;
		}
	};
}
