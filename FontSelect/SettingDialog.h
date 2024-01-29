#pragma once
#include "Paint.h"
#include "FontMenu.h"

namespace fgo::font_select
{
	//
	// このクラスは設定ダイアログのサブクラスです。
	//
	inline struct SettingDialog : Tools::Window
	{
		//
		// 初期化処理を行います。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 設定ダイアログをサブクラス化します。
			subclass(magi.auin.GetSettingDialog());

			return TRUE;
		}

		//
		// 後始末処理を行います。
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
		BOOL onContextMenu(HWND combobox)
		{
			MY_TRACE_FUNC("0x%08X", combobox);

			if (::GetDlgCtrlID(combobox) != Hive::ControlID::FontComboBox) return FALSE;

			// フォント選択用のカスタムメニューを作成します。
			FontMenu menu(combobox);

			// メニューを表示します。
			POINT point; ::GetCursorPos(&point);
			UINT id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, 0, hive.mainWindow, 0);
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
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
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
							if (hive.useBackward) onContextMenu((HWND)wParam);

							break;
						}
					case APPCOMMAND_BROWSER_FORWARD:
						{
							// 「進む」ボタンをハンドルします。
							if (hive.useForward) onContextMenu((HWND)wParam);

							break;
						}
					}

					break;
				}
			case WM_CONTEXTMENU:
				{
					// コンテキストメニューをハンドルします。
					if (hive.useContextMenu) onContextMenu((HWND)wParam);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	} settingDialog;
}
