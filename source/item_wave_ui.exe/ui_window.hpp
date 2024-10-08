#pragma once

namespace apn::item_wave::ui
{
	//
	// このクラスはUIウィンドウです。
	//
	inline struct UIWindow : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			constexpr auto class_name = _T("apn::item_wave::ui");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS | CS_NOCLOSE;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = class_name;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				class_name,
				class_name,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 300, 300,
				hive.host_window, nullptr, hive.instance, nullptr);
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

					// 初期化が完了したので、このウィンドウをUIウィンドウに設定します。
					hive.ui_window = hwnd;

					// このウィンドウをホストウィンドウに関連付けます。
					share::host_window::set_ui_window(hive.host_window, hwnd);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case share::c_message.c_init_process:
				{
					MY_TRACE_FUNC("c_init_process, {:#010x}, {:#010x}", wParam, lParam);

					// コンフィグを読み込みます。
					app->read_config();

					break;
				}
			case share::c_message.c_exit_process:
				{
					MY_TRACE_FUNC("c_exit_process, {:#010x}, {:#010x}", wParam, lParam);

					// 無駄なチェックを省くために
					// コンフィグチェッカーを終了しておきます。
					config_update_checker.exit();

					// コンフィグを書き込みます。
					app->write_config();

					break;
				}
			case share::c_message.c_create_cache:
				{
					MY_TRACE_FUNC("c_create_cache, {:#010x}, {:#010x}", wParam, lParam);

					app->create_cache();

					break;
				}
			case share::c_message.c_clear_caches:
				{
					MY_TRACE_FUNC("c_clear_caches, {:#010x}, {:#010x}", wParam, lParam);

					app->clear_caches();

					break;
				}
			case share::c_message.c_receive_cache:
				{
					MY_TRACE_FUNC("c_receive_cache, {:#010x}, {:#010x}", wParam, lParam);

					app->receive_cache((DWORD)wParam);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} ui_window;
}
