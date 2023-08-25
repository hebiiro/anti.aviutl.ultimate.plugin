#pragma once

namespace Tools::AviUtl
{
	//
	// このクラスは任意のウィンドウ(ターゲット)を内包するウィンドウ拡張(ホルダー)です。
	//
	struct WindowHolderExtension : Window2::Extension
	{
		//
		// ターゲットウィンドウを設定します。
		//
		template<class T>
		static BOOL setTarget(T& window, HWND hwnd)
		{
			auto windowHolderExtension =
				window.getExtension<WindowHolderExtension>();
			if (!windowHolderExtension) return FALSE;

			windowHolderExtension->setTarget(hwnd);
			windowHolderExtension->resize(window);

			return TRUE;
		}

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
//			MY_TRACE(_T("AviUtl::WindowHolderExtension::resize()\n"));

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
		inline static const LPCWSTR Name = L"Tools::AviUtl::WindowHolderExtension";

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
//					MY_TRACE(_T("WindowHolderExtension::onWndProc(WM_SHOWWINDOW)\n"));

					if (wParam && ::IsWindow(target))
						::ShowWindow(target, SW_SHOW);

					break;
				}
			case WM_SIZE:
				{
//					MY_TRACE(_T("WindowHolderExtension::onWndProc(WM_SIZE)\n"));

					if (::IsWindow(target))
						resize(hwnd);

					break;
				}
			}

			return FALSE;
		}

	public:

		HWND target = 0; // ターゲットウィンドウです。
	};
}
