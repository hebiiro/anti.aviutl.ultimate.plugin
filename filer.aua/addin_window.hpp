#pragma once

namespace apn::filer
{
	//
	// このクラスはホストプロセスのメインウィンドウです。
	//
	inline struct AddinWindow : my::aviutl::PluginWindow
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
				100, 100, 200, 300);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
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

					// アドインダイアログを初期化します。
					if (!addin_dialog.init(hwnd))
					{
						hive.message_box(L"アドインダイアログの初期化に失敗しました");
						return -1;
					}

					set_target(addin_dialog);
					resize();

					// アドインウィンドウの初期化が完了したので、
					// ここでハイブに格納しておきます。
					hive.addin_window = hwnd;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// アドインダイアログを終了します。
					addin_dialog.exit();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_window;
}
