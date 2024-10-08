#pragma once

namespace apn::font_select
{
	//
	// このクラスは設定ダイアログのサブクラスです。
	//
	inline struct SettingDialog : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログをサブクラス化します。
			subclass(magi.exin.get_setting_dialog());

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログのサブクラス化を解除します。
			unsubclass();

			return TRUE;
		}

		//
		// フォント選択用のカスタムメニューを表示します。
		//
		BOOL on_context_menu(HWND combobox)
		{
			MY_TRACE_FUNC("{:#010x}", combobox);

			if (::GetDlgCtrlID(combobox) != hive.c_control_id.c_font_combobox) return FALSE;

			// フォント選択用のカスタムメニューを作成します。
			FontMenu menu(combobox);

			// メニューを表示します。
			auto point = my::get_cursor_pos();
			auto id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, 0, hive.main_window, 0);
			if (id == 0) return FALSE;

			// 選択されたフォントをフォントのコンボボックスに適用します。
			if (CB_ERR == ::SendMessageW(combobox, CB_SETCURSEL, id - 1, 0)) return FALSE;

			// 選択フォントが変更されたことを設定ダイアログに通知します。
			::SendMessageW(::GetParent(combobox), WM_COMMAND,
				MAKEWPARAM(::GetDlgCtrlID(combobox), CBN_SELCHANGE), (LPARAM)combobox);

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_APPCOMMAND:
				{
					auto cmd  = GET_APPCOMMAND_LPARAM(lParam);

					switch (cmd)
					{
					case APPCOMMAND_BROWSER_BACKWARD:
						{
							// 「戻る」ボタンをハンドルします。
							if (hive.use_backward) on_context_menu((HWND)wParam);

							break;
						}
					case APPCOMMAND_BROWSER_FORWARD:
						{
							// 「進む」ボタンをハンドルします。
							if (hive.use_forward) on_context_menu((HWND)wParam);

							break;
						}
					}

					break;
				}
			case WM_CONTEXTMENU:
				{
					// コンテキストメニューをハンドルします。
					if (hive.use_context_menu) on_context_menu((HWND)wParam);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} setting_dialog;
}
