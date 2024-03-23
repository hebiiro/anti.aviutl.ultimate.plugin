#pragma once

namespace apn::workspace
{
	//
	// このクラスはサブプロセスのウィンドウを保持するシャトルです。
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
		// サブプロセスウィンドウです。
		//
		HWND window = 0;

		//
		// サブプロセスウィンドウを見つけて返します。
		//
		virtual HWND find_window() = 0;

		//
		// ウィンドウを作成します。
		//
		BOOL create(LPCTSTR name, HWND parent)
		{
			MY_TRACE_FUNC("{}, {:#010x}", name, parent);

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = Hive::SubProcess::c_class_name;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				Hive::SubProcess::c_class_name,
				name,
				WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, 0, hive.instance, 0);
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
			::SetWindowPos(window, 0, 0, 0, w, h, SWP_NOZORDER | SWP_ASYNCWINDOWPOS);
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
					MY_TRACE_FUNC("{:#010x}, WM_CREATE, {:#010x}, {:#010x}", hwnd, wParam, lParam);

					// このタイミングではまだサブプロセスウィンドウが
					// 作成されていないかもしれないので、
					// タイマーで遅らせて初期化処理を行います。
					::SetTimer(hwnd, c_timer_id.c_init, 1000, 0);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("{:#010x}, WM_DESTROY, {:#010x}, {:#010x}", hwnd, wParam, lParam);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("{:#010x}, WM_CLOSE, {:#010x}, {:#010x}", hwnd, wParam, lParam);

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
					MY_TRACE_FUNC("{:#010x}, WM_SHOWWINDOW, {:#010x}, {:#010x}", hwnd, wParam, lParam);

					// このウィンドウが表示されるとき、サブプロセスウィンドウも表示状態にします。
					if (wParam && ::IsWindow(window))
						::ShowWindowAsync(window, SW_RESTORE);

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("{:#010x}, WM_SIZE, {:#010x}, {:#010x}", hwnd, wParam, lParam);

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
							// サブプロセスウィンドウを見つけます。
							window = find_window();
							if (window)
							{
								MY_TRACE("サブプロセスウィンドウが見つかりました => {:#010x}\n", window);

								DWORD remove = WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
								DWORD add = WS_CHILD;

								::KillTimer(hwnd, c_timer_id.c_init);
								my::modify_style(window, remove, add);
								my::modify_ex_style(window, 0, WS_EX_NOPARENTNOTIFY);
								::SetParent(window, hwnd);

								// コンソールウィンドウ用の処理です。
								// 最小化されているので元に戻します。
								::ShowWindowAsync(window, SW_RESTORE);

								// コンソールウィンドウ用の処理です。
								// WM_SIZEをポストしてresize()を実行します。
								::PostMessage(hwnd, WM_SIZE, 0, 0);
							}

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
