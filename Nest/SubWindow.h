#pragma once
#include "Shuttle/Shuttle.h"
#include "DockSite.h"
#include "SubWindowManager.h"

namespace fgo::nest
{
	//
	// このクラスはサブウィンドウです。
	// このウィンドウはドッキング機能を有しながら、
	// このウィンドウ自体を他のドックサイトにドッキングさせることもできます。
	//
	struct SubWindow : DockSite<Shuttle>
	{
		//
		// 使用されていないシャトル名を返します。
		//
		static void getEmptyName(LPTSTR name, int cch)
		{
			for (int i = 0; TRUE; i++)
			{
				::StringCchPrintf(name, cch, _T("サブ%d"), i + 1);
				MY_TRACE_TSTR(name);

				if (!shuttleManager.get(name))
					break;
			}
		}

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
			wc.lpszClassName = hive.SubWindowClassName;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				hive.SubWindowClassName,
				name,
				WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, 0, hive.instance, 0);
		}

		//
		// ターゲットにアタッチします。
		//
		void init(const _bstr_t& name, HWND hwnd)
		{
			__super::init(name, hwnd);

			// フローティングコンテナのアイコンを設定します。
			HICON icon = (HICON)::GetClassLong(hive.aviutlWindow, GCL_HICON);
			::SendMessage(*floatContainer, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			::SendMessage(*floatContainer, WM_SETICON, ICON_BIG, (LPARAM)icon);

			// フローティングコンテナのシステムメニューに項目を追加します。
			HMENU menu = ::GetSystemMenu(*floatContainer, FALSE);
			::InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING, CommandID::RENAME_SUB_WINDOW, _T("名前を変更"));
			::InsertMenu(menu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
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
					MY_TRACE_FUNC("0x%08X, WM_CREATE, 0x%08X, 0x%08X", hwnd,  wParam, lParam);

					// このサブウィンドウをマネージャーに追加します。
					// これにより、現存するすべてのサブウィンドウにアクセスできるようになります。
					subWindowManager.add(hwnd);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("0x%08X, WM_CLOSE, 0x%08X, 0x%08X", hwnd,  wParam, lParam);

					if (::GetKeyState(VK_SHIFT) < 0)
					{
						// Shiftキーが押されている場合はサブウィンドウを削除します。

						// このサブウィンドウを削除していいのかどうかをユーザーに問い合わせます。
						if (IDYES != ::MessageBox(hwnd, _T("サブウィンドウを削除しますか？"), hive.AppName, MB_YESNO))
							return 0;

						// このサブウィンドウをマネージャーから削除します。
						// これにより、このウィンドウは破壊されます。
						subWindowManager.erase(hwnd);
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
						::SetActiveWindow(hive.mainWindow);
					}

					return 0;
				}
			case WM_SYSCOMMAND: // フローティングコンテナのシステムメニューコマンドをハンドルします。
				{
					switch (wParam)
					{
					case CommandID::RENAME_SUB_WINDOW:
						{
							// サブウィンドウの名前変更ダイアログを表示します。
							shuttleManager.showRenameDialog(this, hwnd);

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
