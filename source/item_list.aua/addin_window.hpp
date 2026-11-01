#pragma once

namespace apn::item_list
{
	//
	// このクラスはアドインウィンドウです。
	// このウィンドウはプラグインウィンドウのように振る舞います。
	//
	inline struct addin_window_t : StdAddinWindow
	{
		inline static constexpr struct timer_id_t {
			inline static constexpr auto k_refresh_list = WM_APP + 2026;
		} k_timer_id;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return create_as_plugin(
				hive.instance,
				magi.exin.get_aviutl_window(),
				hive.k_display_name,
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
		// アイテム一覧をリフレッシュします。
		//
		BOOL refresh_list()
		{
			MY_TRACE_FUNC("");

			return !!::SetTimer(*this, k_timer_id.k_refresh_list, hive.refresh_delay, nullptr);
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

					// メインダイアログを初期化します。
					if (!addin_dialog.init(hive.instance, hwnd))
					{
						MY_TRACE("メインダイアログの作成に失敗しました\n");

						return -1;
					}

					// このウィンドウで表示するターゲットを設定します。
					set_target(addin_dialog);

					// 初期化が完了したので、このウィンドウをメインウィンドウに設定します。
					hive.main_window = addin_window;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", w_param, l_param);

					// メインダイアログを終了します。
					addin_dialog.exit();

					break;
				}
			case WM_TIMER:
				{
					switch (w_param)
					{
					case k_timer_id.k_refresh_list:
						{
							// アイテム一覧をリフレッシュします。
							app->refresh_list();

							// タイマーを終了します。
							::KillTimer(hwnd, w_param);

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} addin_window;
}
