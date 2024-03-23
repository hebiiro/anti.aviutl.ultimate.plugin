#pragma once

namespace apn::workspace
{
	//
	// このクラスはAviUtlウィンドウ用のシャトルです。
	//
	struct AviUtlWindow : Shuttle
	{
		//
		// ターゲットにアタッチします。
		//
		void init(const std::wstring& name, HWND hwnd)
		{
			MY_TRACE_FUNC("{}, {:#010x}", name, hwnd);

			__super::init(name, hwnd);

			// フローティングコンテナのアイコンを設定します。
			auto icon = (HICON)::GetClassLong(*this, GCL_HICON);
			::SendMessage(*float_container, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			::SendMessage(*float_container, WM_SETICON, ICON_BIG, (LPARAM)icon);

			// メインウィンドウのアイコンを設定します。
			::SetClassLong(hive.main_window, GCL_HICON, (LONG)icon);
			::SetClassLong(hive.main_window, GCL_HICONSM, (LONG)icon);
		}

		//
		// ターゲットウィンドウを表示します。
		//
		virtual void show_target_window() override
		{
			// ターゲットウィンドウが表示状態なら何もしません。
			if (::IsWindowVisible(*this)) return;

			// AviUtlウィンドウは普通に表示します。
			::ShowWindow(*this, SW_SHOW);
		}

		//
		// ターゲットのウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// このタイミングでサブクラス化を解除して、後始末処理を省略します。
					unsubclass();

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, {:#010x}, {:#010x}", wParam, lParam);

					// AviUtlが終了しようとしているのでレイアウトを保存します。
					::SendMessage(hive.main_window, Hive::WindowMessage::c_write_preference, 0, 0);

					break;
				}
			case WM_SETTEXT:
				{
					MY_TRACE_FUNC("WM_SETTEXT, {:#010x}, {:#010x}", wParam, lParam);

					auto result = __super::on_wnd_proc(hwnd, message, wParam, lParam);

					// メインウィンドウのウィンドウテキストを更新します。
					::SendMessage(hive.main_window, Hive::WindowMessage::c_refresh_title, 0, 0);

					return result;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	}; inline auto aviutl_window = std::make_shared<AviUtlWindow>();
}
