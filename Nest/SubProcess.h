#pragma once
#include "Shuttle/Shuttle.h"
#include "SubProcessManager.h"

namespace fgo::nest
{
	//
	// このクラスはサブプロセスのウィンドウを保持するシャトルです。
	//
	struct SubProcess : Shuttle
	{
		struct TimerID {
			static const UINT Init = 1000;
		};

		//
		// サブプロセスウィンドウです。
		//
		HWND window = 0;

		//
		// サブプロセスウィンドウを見つけて返します。
		//
		virtual HWND findWindow() = 0;

		//
		// ウィンドウを作成します。
		//
		BOOL create(LPCTSTR name, HWND parent)
		{
			MY_TRACE_FUNC("%s, 0x%08X", name, parent);

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = hive.SubProcessClassName;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				hive.SubProcessClassName,
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
		// ウィンドウ位置の変更は非同期で処理されるので、デッドロックの心配はありません。
		void resize()
		{
			MY_TRACE_FUNC("");

			RECT rc; ::GetClientRect(*this, &rc);
			int w = getWidth(rc);
			int h = getHeight(rc);
			::SetWindowPos(window, 0, 0, 0, w, h, SWP_NOZORDER | SWP_ASYNCWINDOWPOS);
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("0x%08X, WM_CREATE, 0x%08X, 0x%08X", hwnd, wParam, lParam);

					// このサブプロセスをマネージャーに追加します。
					// これにより、現存するすべてのサブプロセスにアクセスできるようになります。
					subProcessManager.add(hwnd);

					// このタイミングではまだサブプロセスウィンドウが作成されていないかもしれないので、
					// タイマーで遅らせて初期化処理を行います。
					::SetTimer(hwnd, TimerID::Init, 1000, 0);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("0x%08X, WM_DESTROY, 0x%08X, 0x%08X", hwnd, wParam, lParam);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("0x%08X, WM_CLOSE, 0x%08X, 0x%08X", hwnd, wParam, lParam);

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
					MY_TRACE_FUNC("0x%08X, WM_SHOWWINDOW, 0x%08X, 0x%08X", hwnd, wParam, lParam);

					// このウィンドウが表示されるとき、サブプロセスウィンドウも表示状態にします。
					if (wParam && ::IsWindow(window))
						::ShowWindowAsync(window, SW_RESTORE);

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("0x%08X, WM_SIZE, 0x%08X, 0x%08X", hwnd, wParam, lParam);

					// サブプロセスウィンドウが有効ならリサイズします。
					if (::IsWindow(window))
						resize();

					break;
				}
			case WM_TIMER:
				{
					switch (wParam)
					{
					case TimerID::Init:
						{
							// サブプロセスウィンドウを見つけます。
							window = findWindow();
							if (window)
							{
								MY_TRACE(_T("サブプロセスウィンドウが見つかりました => 0x%08X\n"), window);

								DWORD remove = WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
								DWORD add = WS_CHILD;

								::KillTimer(hwnd, TimerID::Init);
								modifyStyle(window, remove, add);
								modifyExStyle(window, 0, WS_EX_NOPARENTNOTIFY);
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

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
