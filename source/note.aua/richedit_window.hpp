#pragma once

namespace apn::note
{
	//
	// このクラスはリッチエディットウィンドウです。
	// このウィンドウはリッチエディットコントロールを持ちます。
	//
	struct RichEditWindow : StdAddinWindow
	{
		//
		// リッチエディットコントロールです。
		//
		RichEdit richedit;

		//
		// 初期化処理を実行します。
		//
		BOOL init(LPCWSTR name)
		{
			MY_TRACE_FUNC("{/}", name);

			return create_as_plugin(
				hive.instance,
				magi.exin.get_aviutl_window(),
				name,
				WS_EX_NOPARENTNOTIFY,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 800, 600);
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

					if (!richedit.init(hwnd))
					{
						MY_TRACE("リッチエディットコントロールの作成に失敗しました\n");

						return -1;
					}

					set_target(richedit);
					resize();

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					richedit.exit();

					break;
				}
			case WM_COMMAND:
				{
					MY_TRACE_FUNC("WM_COMMAND, {/hex}, {/hex}", wParam, lParam);

					richedit.on_command_reflection(hwnd, message, wParam, lParam);

					break;
				}
			case WM_NOTIFY:
				{
					MY_TRACE_FUNC("WM_NOTIFY, {/hex}, {/hex}", wParam, lParam);

					richedit.on_notify_reflection(hwnd, message, wParam, lParam);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
