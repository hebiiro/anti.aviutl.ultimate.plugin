#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスはコウィンドウです。
	// ビジュアル(外部プロセスのウィンドウ)を格納するためのウィンドウです。
	// aviutlプラグインウィンドウのように振る舞います。
	// 『音声視覚化』カテゴリに分類されます。
	//
	struct CoWindow : my::aviutl::PluginWindow
	{
		//
		// 名前です。
		// ウィンドウ名を決定するために使用されます。
		//
		std::wstring name;

		//
		// ウィンドウを作成します。
		//
		BOOL create(const std::wstring& name)
		{
			// 名前を設定します。
			this->name = name;

			// 親ウィンドウを取得します。
			auto parent = ::GetWindow(magi.fp->hwnd, GW_OWNER);

			// ウィンドウを作成する度にX座標をずらします。
			static auto w = 100;
			static auto x = 0; x += w;

			// AviUtlプラグインウィンドウと同じように振る舞うウィンドウを作成します。
			return create_as_plugin(
				hive.instance, parent, name.c_str(),
				WS_EX_NOPARENTNOTIFY | WS_EX_TOOLWINDOW,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				x, 100, w, 400);
		}

		//
		// ウィンドウ名を更新します。
		//
		void update_window_name()
		{
			::SetWindowTextW(*this, name.c_str());
			::SetWindowTextW(get_target(), name.c_str());
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

					// カテゴリ名を設定します。
					::SetProp(hwnd, L"aviutl.plugin.category_name", (HANDLE)hive.c_display_name);

					// 外部プロセスにコウィンドウが作成されたことを通知します。
					::PostMessage(hive.ui_window, share::c_message.c_init_co_window, (WPARAM)hwnd, 0);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					// 外部プロセスにコウィンドウが終了していることを通知します。
					::PostMessage(hive.ui_window, share::c_message.c_exit_co_window, (WPARAM)hwnd, (LPARAM)get_target());

					break;
				}
			case WM_GETMINMAXINFO:
				{
//					MY_TRACE_FUNC("WM_GETMINMAXINFO, {/hex}, {/hex}", wParam, lParam);

					// ウィンドウの最小サイズを0に変更します。
					auto mmi = (MINMAXINFO*)lParam;
					mmi->ptMinTrackSize.x = 0;
					mmi->ptMinTrackSize.y = 0;

					break;
				}
			case share::c_message.c_init_co_window:
				{
					MY_TRACE_FUNC("c_init_co_window, {/hex}, {/hex}", wParam, lParam);

					auto co_window = (HWND)wParam;
					auto visual = (HWND)lParam;

					set_target(visual);
					resize();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
