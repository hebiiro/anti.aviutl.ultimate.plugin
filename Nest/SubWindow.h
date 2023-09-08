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
		// コンストラクタです。
		//
		SubWindow(LPCTSTR name, HWND parent)
		{
			MY_TRACE(_T("SubWindow::SubWindow(%s, 0x%08X)\n"), name, parent);

			if (create(name, parent))
				init(name, *this);
		}

		//
		// デストラクタです。
		//
		virtual ~SubWindow()
		{
			MY_TRACE(_T("SubWindow::~SubWindow(), 0x%08X\n"), (HWND)*this);
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
					MY_TRACE(_T("SubWindow::onWndProc(0x%08X, WM_CREATE, 0x%08X, 0x%08X)\n"), hwnd,  wParam, lParam);

					// このサブウィンドウをマネージャーに追加します。
					// これにより、現存するすべてのサブウィンドウにアクセスできるようになります。
					subWindowManager.add(hwnd);

					break;
				}
			case WM_NCDESTROY:
				{
					MY_TRACE(_T("SubWindow::onWndProc(0x%08X, WM_NCDESTROY, 0x%08X, 0x%08X)\n"), hwnd,  wParam, lParam);

					// このあとの処理でthisが無効になるので、先に基本クラスのウィンドウプロシージャを呼んでおきます。
					LRESULT lr = __super::onWndProc(hwnd, message, wParam, lParam);

					// シャトルを削除します。
					shuttleManager.erase(this);

					return lr;
				}
			case WM_CLOSE:
				{
					MY_TRACE(_T("SubWindow::onWndProc(0x%08X, WM_CLOSE, 0x%08X, 0x%08X)\n"), hwnd,  wParam, lParam);

					if (::GetKeyState(VK_SHIFT) < 0)
					{
						// Shiftキーが押されている場合はサブウィンドウを削除します。

						// このサブウィンドウを削除していいのかどうかをユーザーに問い合わせます。
						if (IDYES != ::MessageBox(hwnd, _T("サブウィンドウを削除しますか？"), hive.AppName, MB_YESNO))
							return 0;

						// 何もしないとメインウィンドウが他のウィンドウの後ろに隠れてしまうので、
						// 手動でフォアグラウンドにします。
						::SetActiveWindow(hive.mainWindow);

						// このサブウィンドウをマネージャーから削除します。
						// これにより、このウィンドウは破壊されます。
						subWindowManager.erase(hwnd);
					}
					else
					{
						// サブウィンドウの表示状態を切り替えます。
						// ::ShowWindow(hwnd, SW_SHOW)ではなぜかWM_SHOWWINDOWが発生しないので、
						// 親ウィンドウ(コンテナ)の表示状態を直接切り替えます。

						if (::IsWindowVisible(hwnd))
						{
							::ShowWindow(::GetParent(hwnd), SW_HIDE);
						}
						else
						{
							::ShowWindow(::GetParent(hwnd), SW_SHOW);
						}
					}

					return 0;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
