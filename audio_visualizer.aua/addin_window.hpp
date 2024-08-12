#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスはアドインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct AddinWindow : StdAddinWindow
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを作成します。
			return create_as_plugin(
				hive.instance,
				magi.exin.get_aviutl_window(),
				hive.c_display_name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 500, 500);
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

					if (!addin_dialog.init(hive.instance, hwnd))
					{
						MY_TRACE("メインダイアログの作成に失敗しました\n");

						return -1;
					}

					set_target(addin_dialog);
					resize();

					// このウィンドウをメインウィンドウに設定します。
					hive.main_window = hwnd;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					addin_dialog.exit();

					break;
				}
			case share::c_message.c_init_process:
				{
					MY_TRACE_FUNC("c_init_process, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case share::c_message.c_exit_process:
				{
					MY_TRACE_FUNC("c_exit_process, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case share::c_message.c_request_option:
				{
					MY_TRACE_FUNC("c_request_option, {:#010x}, {:#010x}", wParam, lParam);

					worker.process_option((HWND)wParam, hive.option);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_window;
}
