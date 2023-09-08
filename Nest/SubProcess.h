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

		HWND target = 0; // サブプロセスののウィンドウ。

		//
		// ターゲットウィンドウを見つけて返します。
		//
		virtual HWND findWindow() = 0;

		//
		// ウィンドウを作成します。
		//
		BOOL create(LPCTSTR name, HWND parent)
		{
			MY_TRACE(_T("SubProcess::create(%s, 0x%08X)\n"), name, parent);

			const LPCTSTR className = _T("Nest.SubProcess");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = className;
			::RegisterClass(&wc);

			return __super::create(
				0,
				className,
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
			MY_TRACE(_T("SubProcess::show()\n"));

			::SendMessage(*this, WM_CLOSE, 0, 0);
		}

		// サブプロセスのウィンドウをクライアント領域全体まで広げます。
		// ウィンドウ位置の変更は非同期で処理されるので、デッドロックの心配はありません。
		void resize()
		{
			MY_TRACE(_T("SubProcess::resize()\n"));

			RECT rc; ::GetClientRect(*this, &rc);
			int w = getWidth(rc);
			int h = getHeight(rc);

			UINT flags = SWP_NOZORDER | SWP_ASYNCWINDOWPOS;

			if (::IsWindowVisible(*this))
				flags |= SWP_SHOWWINDOW;

			::SetWindowPos(target, 0, 0, 0, w, h, flags);
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
					MY_TRACE(_T("SubProcess::onWndProc(WM_CREATE)\n"));

					// このサブプロセスをマネージャーに追加します。
					// これにより、現存するすべてのサブプロセスにアクセスできるようになります。
					subProcessManager.add(hwnd);

					// このタイミングではまだサブプロセスのウィンドウが作成されていないかもしれないので、
					// タイマーで遅らせて初期化処理を行います。
					::SetTimer(hwnd, TimerID::Init, 1000, 0);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE(_T("SubProcess::onWndProc(WM_DESTROY)\n"));

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE(_T("SubProcess::onWndProc(WM_CLOSE)\n"));

					// ウィンドウの表示状態をトグルで切り替えます。
					if (::IsWindowVisible(hwnd))
					{
						::ShowWindow(hwnd, SW_HIDE);
					}
					else
					{
						::ShowWindow(hwnd, SW_SHOW);
					}

					return 0;
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE(_T("SubProcess::onWndProc(WM_SHOWWINDOW)\n"));
#if 0
					if (wParam && ::IsWindow(target))
						::ShowWindow(target, SW_SHOW);
#endif
					break;
				}
			case WM_SIZE:
				{
					MY_TRACE(_T("SubProcess::onWndProc(WM_SIZE)\n"));

					// サブプロセスのウィンドウが有効ならリサイズします。
					if (::IsWindow(target))
						resize();

					break;
				}
			case WM_TIMER:
				{
					switch (wParam)
					{
					case TimerID::Init:
						{
							// サブプロセスのウィンドウを見つけます。
							target = findWindow();
							if (target)
							{
								MY_TRACE(_T("サブプロセスのウィンドウが見つかりました => 0x%08X\n"), target);

								DWORD remove = WS_POPUP | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;
								DWORD add = WS_CHILD;

								::KillTimer(hwnd, TimerID::Init);
								modifyStyle(target, remove, add);
								::SetParent(target, hwnd);
								resize();
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
