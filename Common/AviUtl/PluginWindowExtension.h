#pragma once

namespace Tools::AviUtl
{
	//
	// このクラスはプラグインウィンドウのように振る舞うためのウィンドウ拡張です。
	//
	struct PluginWindowExtension : Window2::Extension
	{
		//
		// ウィンドウを作成します。
		//
		template<class T>
		static BOOL create(T& window, HINSTANCE instance, HWND parent, LPCTSTR name,
			DWORD exStyle = 0,
			DWORD style = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			int x = CW_USEDEFAULT,
			int y = CW_USEDEFAULT,
			int w = CW_USEDEFAULT,
			int h = CW_USEDEFAULT)
		{
			MY_TRACE(_T("AviUtl::PluginWindowExtension::create()\n"));

			const LPCTSTR className = _T("AviUtl");

			WNDCLASS wc = {};
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = instance;
			wc.lpszClassName = className;
			::RegisterClass(&wc);

			return window.create(
				exStyle,
				className,
				name,
				style,
				x, y, w, h,
				parent, 0, instance, 0);
		}

		//
		// ウィンドウの表示/非表示を切り替えます。
		//
		static void show(HWND hwnd)
		{
			MY_TRACE(_T("AviUtl::PluginWindowExtension::show()\n"));

			::SendMessage(hwnd, WM_CLOSE, 0, 0);
		}

		//
		// ウィンドウ拡張の名前です。
		//
		inline static const LPCWSTR Name = L"Tools::AviUtl::PluginWindowExtension";

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
			case WM_CLOSE:
				{
					MY_TRACE(_T("AviUtl::PluginWindowExtension::onWndProc(WM_CLOSE)\n"));

					if (::IsWindowVisible(hwnd))
					{
						::ShowWindow(hwnd, SW_HIDE);
					}
					else
					{
						::ShowWindow(hwnd, SW_SHOW);
					}

					*lr = 0;
					return TRUE;
				}
			}

			return FALSE;
		}
	};
}
