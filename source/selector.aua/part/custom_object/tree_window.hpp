#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはカスタムオブジェクトのツリーウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct TreeWindow : SelectorWindow
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return create_as_plugin(
				hive.instance,
				hive.aviutl_window,
				_T("カスタムオブジェクトツリー"),
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 500, 400);
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
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					// ツリーダイアログを初期化します。
					if (!tree_dialog.init(hive.instance, hwnd))
					{
						MY_TRACE("ツリーダイアログの作成に失敗しました\n");

						return -1;
					}

					// このウィンドウで表示するターゲットを設定します。
					set_target(tree_dialog);

					// ターゲットをリサイズします。
					resize();

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					// ツリーダイアログを終了します。
					tree_dialog.exit();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} tree_window;
}
