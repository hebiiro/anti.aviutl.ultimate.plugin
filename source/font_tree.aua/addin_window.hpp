#pragma once

namespace apn::font_tree
{
	//
	// このクラスはアドインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct AddinWindow : StdAddinWindow
	{
		inline static constexpr struct Message {
			inline static constexpr uint32_t c_init_preview = WM_APP + 1;
		} c_message;

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
				100, 100, 600, 600);
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

					// コモンコントロールを初期化します。
					INITCOMMONCONTROLSEX icex = {};
					icex.dwICC = ICC_TREEVIEW_CLASSES;
					::InitCommonControlsEx(&icex);

					// メインダイアログを初期化します。
					if (!addin_dialog.init(hive.instance, hwnd))
					{
						MY_TRACE("メインダイアログの作成に失敗しました\n");

						return -1;
					}

					// このウィンドウで表示するターゲットを設定します。
					set_target(addin_dialog);

					// ターゲットをリサイズします。
					resize();

					// 初期化が完了したので、このウィンドウをメインウィンドウに設定します。
					hive.main_window = addin_window;

					// プレビューの初期化を予約します。
					::PostMessage(hwnd, c_message.c_init_preview, 0, 0);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					// メインダイアログを終了します。
					addin_dialog.exit();

					// プレビューを終了します。
					preview_manager.exit();

					break;
				}
			case c_message.c_init_preview:
				{
					MY_TRACE_FUNC("c_message.c_init_preview, {/hex}, {/hex}", wParam, lParam);

					// プレビューを初期化します。
					preview_manager.init();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_window;
}
