#pragma once

namespace my::aviutl
{
	//
	// このクラスは任意のウィンドウ(ターゲット)を内包するウィンドウ拡張です。
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
		HWND get_target() const { return target; }

		//
		// ターゲットウィンドウを設定します。
		//
		void set_target(HWND target){ this->target = target; }

		//
		// ターゲットウィンドウをホルダーのクライアント領域まで広げます。
		// 非同期で処理します。
		//
		void resize(HWND hwnd)
		{
//			MY_TRACE_FUNC("");

			auto rc = get_client_rect(hwnd);
			auto x = rc.left;
			auto y = rc.top;
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);

			auto flags = SWP_NOZORDER | SWP_ASYNCWINDOWPOS;
			if (::IsWindowVisible(hwnd)) flags |= SWP_SHOWWINDOW;

			::SetWindowPos(target, 0, x, y, w, h, flags);
		}

		//
		// ウィンドウ拡張の名前です。
		//
		inline static constexpr auto c_name = L"my::aviutl::window_holder_extension";

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
			case WM_SHOWWINDOW:
				{
//					MY_TRACE_FUNC("WM_SHOWWINDOW, {:#010x}, {:#010x}", wParam, lParam);

					if (wParam && ::IsWindow(target))
						::ShowWindowAsync(target, SW_SHOW);

					break;
				}
			case WM_SIZE:
				{
//					MY_TRACE_FUNC("WM_SIZE, {:#010x}, {:#010x}", wParam, lParam);

					if (::IsWindow(target))
						resize(hwnd);

					break;
				}
			}

			return FALSE;
		}
	};
}
