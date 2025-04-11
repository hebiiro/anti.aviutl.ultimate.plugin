#pragma once

namespace my::aviutl
{
	//
	// このクラスはプラグインウィンドウのように振る舞うためのウィンドウ拡張です。
	//
	struct PluginWindowExtension : Window2::Extension
	{
		//
		// ウィンドウを作成します。
		//
		template<typename T>
		static BOOL create(T& window, HINSTANCE instance, HWND parent, LPCTSTR name,
			DWORD ex_style = WS_EX_NOPARENTNOTIFY,
			DWORD style = WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			int x = CW_USEDEFAULT,
			int y = CW_USEDEFAULT,
			int w = CW_USEDEFAULT,
			int h = CW_USEDEFAULT)
		{
			MY_TRACE_FUNC("");

			constexpr auto class_name = _T("AviUtl");

			WNDCLASS wc = {};
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = instance;
			wc.lpszClassName = class_name;
			wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			::RegisterClass(&wc);

			return window.create(
				ex_style,
				class_name,
				name,
				style,
				x, y, w, h,
				parent, nullptr, instance, nullptr);
		}

		//
		// ウィンドウの表示/非表示を切り替えます。
		//
		static void show(HWND hwnd)
		{
			MY_TRACE_FUNC("");

			::SendMessage(hwnd, WM_CLOSE, 0, 0);
		}

		//
		// ウィンドウ拡張の名前です。
		//
		inline static constexpr auto c_name = L"my::aviutl::plugin_window_extension";

		//
		// ウィンドウ拡張の名前を返します。
		//
		virtual constexpr LPCWSTR get_extension_name() const override
		{
			return c_name;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual BOOL on_wnd_proc(Window2* window, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr) override
		{
			switch (message)
			{
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, {/hex}, {/hex}", wParam, lParam);

					if (::IsWindowVisible(hwnd))
					{
						::ShowWindowAsync(hwnd, SW_HIDE);
					}
					else
					{
						::ShowWindowAsync(hwnd, SW_SHOW);
					}

					*lr = 0;
					return TRUE;
				}
			}

			return FALSE;
		}
	};
}
