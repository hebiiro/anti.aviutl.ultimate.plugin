#pragma once

namespace Tools::AviUtl
{
	//
	// このクラスは任意のウィンドウ(ターゲット)を内包するウィンドウ拡張(ホルダー)です。
	//
	struct WindowHolderExtension : Window2::Extension
	{
		//
		// ターゲットウィンドウです。
		//
		HWND target = 0;

		//
		// ターゲットウィンドウを返します。
		//
		HWND getTarget() const { return target; }

		//
		// ターゲットウィンドウを設定します。
		//
		void setTarget(HWND newTarget){ target = newTarget; }

		//
		// ターゲットウィンドウをホルダーのクライアント領域まで広げます。
		// 非同期で処理します。
		//
		void resize(HWND hwnd)
		{
//			MY_TRACE_FUNC("");

			RECT rc; ::GetClientRect(hwnd, &rc);
			int x = rc.left;
			int y = rc.top;
			int w = getWidth(rc);
			int h = getHeight(rc);

			UINT flags = SWP_NOZORDER | SWP_ASYNCWINDOWPOS;

			if (::IsWindowVisible(hwnd))
				flags |= SWP_SHOWWINDOW;

			::SetWindowPos(target, 0, x, y, w, h, flags);
		}

		//
		// ウィンドウ拡張の名前です。
		//
		inline static constexpr LPCWSTR Name = L"Tools::AviUtl::WindowHolderExtension";

		//
		// ウィンドウ拡張の名前を返します。
		//
		constexpr LPCWSTR getExtensionName() const
		{
			return Name;
		}

		//
		// ウィンドウプロシージャです。
		//
		BOOL onWndProc(Window2* window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr)
		{
			switch (message)
			{
			case WM_SHOWWINDOW:
				{
//					MY_TRACE_FUNC("WM_SHOWWINDOW, 0x%08X, 0x%08X", wParam, lParam);

					if (wParam && ::IsWindow(target))
						::ShowWindowAsync(target, SW_SHOW);

					break;
				}
			case WM_SIZE:
				{
//					MY_TRACE_FUNC("WM_SIZE, 0x%08X, 0x%08X", wParam, lParam);

					if (::IsWindow(target))
						resize(hwnd);

					break;
				}
			}

			return FALSE;
		}
	};
}
