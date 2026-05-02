#pragma once

namespace apn::filter_relocate::view
{
	//
	// このクラスはビュー層のメインウィンドウです。
	// このウィンドウはaviutlプラグインウィンドウのように振る舞います。
	//
	inline struct main_window_t : StdAddinWindow
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return create_as_plugin(
				model::property.instance,
				magi.exin.get_aviutl_window(),
				model::property.c_display_name,
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
		// メッセージボックスを表示します。
		//
		inline int32_t message_box(const std::wstring& text, int32_t type = MB_OK | MB_ICONWARNING)
		{
			return magi.message_box(text, model::property.c_display_name, *this, type);
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

					if (!main_dialog.init(model::property.instance, hwnd))
					{
						MY_TRACE("メインダイアログの作成に失敗しました\n");

						return -1;
					}

					auto rc = my::get_window_rect(main_dialog);
					my::client_to_window(hwnd, &rc);
					my::set_window_rect(hwnd, &rc);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", w_param, l_param);

					main_dialog.exit();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} main_window;
}
