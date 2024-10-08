#pragma once

namespace apn::ease_select
{
	//
	// このクラスはホストプロセスのメインウィンドウです。
	//
	inline struct HostWindow : my::aviutl::PluginWindow
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return create_as_plugin(
				hive.instance,
				magi.exin.get_aviutl_window(),
				hive.c_display_name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 300, 300);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					// ウィンドウの初期化が完了したので、
					// このウィンドウをメインウィンドウに設定します。
					hive.main_window = hwnd;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case share::message::c_fire_notify:
				{
					MY_TRACE_FUNC("share::message::c_fire_notify, {:#010x}, {:#010x}", wParam, lParam);

					::PostThreadMessage(gui.pi.dwThreadId, share::message::c_notify, 0, 0);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} host_window;
}
