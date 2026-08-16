#pragma once

namespace apn::dark
{
	//
	// このクラスはアドインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct addin_window_t : StdAddinWindow, entry_t::node_t
	{
		virtual BOOL on_init() { return init(); }
		virtual BOOL on_exit() { return exit(); }

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 親ウィンドウは内部プロセスの場合はaviutlウィンドウ、
			// 外部プロセスの場合はnullptrになります。
			auto parent = HWND {};
			if (magi.fp) parent = magi.fp->hwnd_parent;

			return create_as_plugin(
				hive.instance,
				parent,
				hive.c_display_name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 100, 100);
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
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", w_param, l_param);

					if (!addin_dialog.init(hive.instance, hwnd))
					{
						MY_TRACE("メインダイアログの作成に失敗しました\n");

						return -1;
					}

					// クライアント領域がダイアログのサイズと
					// 一致するようにウィンドウサイズを調整します。
					auto rc = my::get_window_rect(addin_dialog);
					my::client_to_window(hwnd, &rc);
					my::set_window_rect(hwnd, &rc);

					// 初期化が完了したので、このウィンドウをメインウィンドウに設定します。
					hive.main_window = addin_window;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", w_param, l_param);

					addin_dialog.exit();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} addin_window;
}
