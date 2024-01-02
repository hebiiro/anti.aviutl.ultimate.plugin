#pragma once
#include "Paint.h"

namespace fgo::font_preview
{
	//
	// このクラスは設定ダイアログのサブクラスです。
	//
	struct SettingDialog : Tools::Window
	{
		//
		// コンボボックス内の指定されたアイテムのテキストを返します。
		//
		static std::wstring getTextFromComboBox(HWND combobox, int index)
		{
			if (index == -1)
				index = (int)::SendMessageW(combobox, CB_GETCURSEL, 0, 0);

			// テキストの長さを取得します。
			int textLength = ::SendMessageW(combobox, CB_GETLBTEXTLEN, index, 0);
			if (textLength <= 0) return _T("");

			// テキストを取得します。
			std::wstring text(textLength, L'\0');
			::SendMessageW(combobox, CB_GETLBTEXT, index, (LPARAM)&text[0]);

			return text;
		}

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
		// ドロップダウン時の処理です。
		// ドロップダウンリストの位置を調整します。
		//
		BOOL onDropDown(HWND combobox)
		{
			MY_TRACE_FUNC("0x%08X", combobox);

			COMBOBOXINFO cbi = { sizeof(cbi) };
			::GetComboBoxInfo(combobox, &cbi);

			// 一旦リストボックスを非表示にします。
			::ShowWindow(cbi.hwndList, SW_HIDE);

			RECT rcComboBox; ::GetWindowRect(combobox, &rcComboBox);

			RECT rcListBox = { 0, 0, hive.listboxWidth, hive.listboxHeight };
			clientToWindow(cbi.hwndList, &rcListBox);
			int x = 0;
			int y = 0;
			int w = getWidth(rcListBox);
			int h = getHeight(rcListBox);

			{
				// リストボックスに一旦サイズを適用してから
				// 改めてサイズを取得します。

				::SetWindowPos(cbi.hwndList, 0, 0, 0, w, h,
					SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
				::GetWindowRect(cbi.hwndList, &rcListBox);
				w = getWidth(rcListBox);
				h = getHeight(rcListBox);
			}

			BOOL validX = FALSE;
			RECT rcMonitor; getMonitorRect(combobox, &rcMonitor);

			// リストボックスのボトムがモニタのボトムからはみ出す場合は
			if (rcComboBox.bottom + h > rcMonitor.bottom)
			{
				// リストボックスのトップがモニタのトップからはみ出す場合は
				if (rcComboBox.top - h < rcMonitor.top)
				{
					y = rcMonitor.top;
					h = std::min<int>(h, getHeight(rcMonitor));

					// リストボックスのレフトがモニタのレフトからはみ出す場合は
					if (rcComboBox.left - w < rcMonitor.left)
					{
						// リストボックスをコンボボックスの右側に配置します。
						x = rcComboBox.right;
						w = std::min<int>(w, rcMonitor.right - x);
					}
					else
					{
						// リストボックスをコンボボックスの左側に配置します。
						x = rcComboBox.left - w;
					}

					// xとwをセットしたのでフラグを立てます。
					validX = TRUE;
				}
				else
				{
					// リストボックスをコンボボックスの上側に配置します。
					y = rcComboBox.top - h;
				}
			}
			else
			{
				// リストボックスをコンボボックスの下側に配置します。
				y = rcComboBox.bottom;
			}

			// xとwがまだセットされていない場合は
			if (!validX)
			{
				// リストボックスの幅がモニタの幅を超えないように調整します。
				w = std::min<int>(w, getWidth(rcMonitor));

				// リストボックスのレフトがモニタのレフトからはみ出す場合は
				if (rcComboBox.left - w < rcMonitor.left)
				{
					// リストボックスのレフトがモニタのレフトと一致するように配置します。
					x = rcMonitor.left;
				}
				// リストボックスのライトがモニタのライトからはみ出す場合は
				else if (rcComboBox.left + w > rcMonitor.right)
				{
					// リストボックスのライトがモニタのライトと一致するように配置します。
					x = rcMonitor.right - w;
				}
				else
				{
					// リストボックスのレフトがコンボボックスのレフトと一致するように配置します。
					x = rcComboBox.left;
				}
			}

			// リストボックス内のアイテムの高さを変更します。
			::SendMessage(combobox, CB_SETITEMHEIGHT, 0, hive.itemHeight);

			// リストボックス本体の位置を変更します。
			::SetWindowPos(cbi.hwndList, 0, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);

			// リストボックスを表示します。
			::ShowWindow(cbi.hwndList, SW_SHOW);

			return TRUE;
		}

		//
		// オーナードロー用の処理です。
		//
		BOOL onDrawItem(DRAWITEMSTRUCT* dis)
		{
//			MY_TRACE_FUNC("%d", dis->itemID);

			auto dc = dis->hDC;
			auto rc = dis->rcItem;
			auto text = getTextFromComboBox(dis->hwndItem, dis->itemID);

			MY_TRACE(_T("%ws, %d, 0x%08X, 0x%08X\n"), text.c_str(), dis->itemID, dis->itemAction, dis->itemState);

			if (dis->itemState & ODS_COMBOBOXEDIT)
			{
				// コンボボックスの本体部分を描画します。

				int stateId = Paint::StateID::Normal;

				if (dis->itemState & ODS_SELECTED) stateId = Paint::StateID::Select;

				{
					// 背景を描画します。
					GdiObj<HBRUSH> brush = ::CreateSolidBrush(paint.getFillColor(stateId));
					::FillRect(dc, &rc, brush);
				}

				{
					// テキストを描画します。

					int bkMode = ::SetBkMode(dc, TRANSPARENT);
					COLORREF textColor = ::SetTextColor(dc, paint.getTextColor(stateId));

					::DrawTextW(dc, text.c_str(), -1, &rc,
						DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);

					::SetTextColor(dc, textColor);
					::SetBkMode(dc, bkMode);
				}
			}
			else
			{
				// ドロップダウンリストのアイテムを描画します。

				int stateId = Paint::StateID::Normal;

				if (dis->itemState & ODS_FOCUS) return FALSE;
				else if (dis->itemState & ODS_SELECTED) stateId = Paint::StateID::Select;

				{
					// 背景を描画します。
					GdiObj<HBRUSH> brush = ::CreateSolidBrush(paint.getFillColor(stateId));
					::FillRect(dc, &rc, brush);
				}

				{
					// テキストを描画します。

					int bkMode = ::SetBkMode(dc, TRANSPARENT);
					COLORREF textColor = ::SetTextColor(dc, paint.getTextColor(stateId));

					{
						// プレビュー用のテキストを描画します。

						GdiObj<HFONT> font = ::CreateFont(
							getHeight(rc), 0, 0, 0, 0, FALSE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
							DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, text.c_str());
						GdiObjSelector fontSelector(dc, font);

						::DrawTextW(dc, hive.previewText, -1, &rc, DT_RIGHT | DT_BOTTOM | DT_NOPREFIX);
					}

					{
						// フォント名を描画します。

						::DrawTextW(dc, text.c_str(), -1, &rc, DT_LEFT | DT_TOP | DT_NOPREFIX);
					}

					::SetTextColor(dc, textColor);
					::SetBkMode(dc, bkMode);
				}
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					UINT code = HIWORD(wParam);
					UINT id = LOWORD(wParam);
					HWND sender = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, 0x%08X, 0x%08X, 0x%08X", code, id, sender);

					if (id == Hive::ControlID::FontComboBox && code == CBN_DROPDOWN)
					{
						// このタイミングでは早すぎるので
						// 一旦メッセージをポストしてから処理します。
						::PostMessage(hwnd, Hive::WindowMessage::DropDown, 0, (LPARAM)sender);
					}

					break;
				}
			case WM_DRAWITEM:
				{
					auto dis = (DRAWITEMSTRUCT*)lParam;
					if (dis->CtlType == ODT_COMBOBOX && dis->CtlID == Hive::ControlID::FontComboBox)
						onDrawItem(dis);

					break;
				}
			}

			if (message == Hive::WindowMessage::DropDown)
			{
				// CBN_DROPDOWNからの続きを処理します。
				onDropDown((HWND)lParam);
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
