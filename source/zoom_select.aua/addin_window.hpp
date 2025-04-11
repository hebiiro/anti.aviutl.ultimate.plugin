﻿#pragma once

namespace apn::zoom_select
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
				100, 100, 150, 200);
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
		LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					if (!addin_dialog.init(hwnd))
					{
						MY_TRACE("メインダイアログの作成に失敗しました\n");

						return -1;
					}

					set_target(addin_dialog);
					resize();

					// 初期化が完了したので、このウィンドウをメインウィンドウに設定します。
					hive.main_window = addin_window;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					addin_dialog.destroy();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_window;
}
