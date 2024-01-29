#pragma once
#include "Resource.h"
#include "Hive.h"
#include "Paint.h"

namespace fgo::font_select
{
	//
	// このクラスはメインダイアログです。
	//
	struct MainDialog : Tools::Dialog
	{
		BOOL lock = FALSE;

		//
		// ウィンドウを作成します。
		//
		BOOL create(HWND parent)
		{
			if (!__super::create(hive.instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), parent))
				return FALSE;

			setComboBox(IDC_PAINT_MODE, paint.mode, _T("自動"), _T("システムカラー"), _T("テーマ"), _T("カスタムカラー"));

			return TRUE;
		}

		//
		// コントロールの値を更新します。
		//
		BOOL updateControls()
		{
			if (lock) return FALSE;

			lock = TRUE;

			setInt(IDC_ITEM_WIDTH, hive.itemWidth);
			setInt(IDC_ITEM_HEIGHT, hive.itemHeight);
			setInt(IDC_FONT_HEIGHT, hive.fontHeight);
			setText(IDC_PREVIEW_TEXT, hive.previewText);
			setComboBox(IDC_PAINT_MODE, paint.mode);
			setCheck(IDC_MARGE, hive.marge);
			setText(IDC_NORMAL_THEME, paint.state[Paint::StateID::Normal].name);
			setUInt(IDC_NORMAL_FILL, paint.state[Paint::StateID::Normal].color.fill);
			setUInt(IDC_NORMAL_TEXT, paint.state[Paint::StateID::Normal].color.text);
			setUInt(IDC_NORMAL_FONT, paint.state[Paint::StateID::Normal].color.font);
			setText(IDC_SELECT_THEME, paint.state[Paint::StateID::Select].name);
			setUInt(IDC_SELECT_FILL, paint.state[Paint::StateID::Select].color.fill);
			setUInt(IDC_SELECT_TEXT, paint.state[Paint::StateID::Select].color.text);
			setUInt(IDC_SELECT_FONT, paint.state[Paint::StateID::Select].color.font);
			setCheck(IDC_USE_CONTEXT_MENU, hive.useContextMenu);
			setCheck(IDC_USE_BACKWARD, hive.useBackward);
			setCheck(IDC_USE_FORWARD, hive.useForward);

			lock = FALSE;

			return TRUE;
		}

		//
		// コンフィグの値を更新します。
		//
		BOOL updateConfig()
		{
			if (lock) return FALSE;

			hive.itemWidth = getInt(IDC_ITEM_WIDTH);
			hive.itemHeight = getInt(IDC_ITEM_HEIGHT);
			hive.fontHeight = getInt(IDC_FONT_HEIGHT);
			hive.previewText = getText(IDC_PREVIEW_TEXT);
			hive.marge = getCheck(IDC_MARGE);
			paint.mode = getComboBox(IDC_PAINT_MODE);
			paint.state[Paint::StateID::Normal].name = getText(IDC_NORMAL_THEME);
			paint.state[Paint::StateID::Normal].color.fill = getUInt(IDC_NORMAL_FILL);
			paint.state[Paint::StateID::Normal].color.text = getUInt(IDC_NORMAL_TEXT);
			paint.state[Paint::StateID::Normal].color.font = getUInt(IDC_NORMAL_FONT);
			paint.state[Paint::StateID::Select].name = getText(IDC_SELECT_THEME);
			paint.state[Paint::StateID::Select].color.fill = getUInt(IDC_SELECT_FILL);
			paint.state[Paint::StateID::Select].color.text = getUInt(IDC_SELECT_TEXT);
			paint.state[Paint::StateID::Select].color.font = getUInt(IDC_SELECT_FONT);
			hive.useContextMenu = getCheck(IDC_USE_CONTEXT_MENU);
			hive.useBackward = getCheck(IDC_USE_BACKWARD);
			hive.useForward = getCheck(IDC_USE_FORWARD);

			HWND hwnd = magi.auin.GetSettingDialog();
			HWND combobox = ::GetDlgItem(hwnd, Hive::ControlID::FontComboBox);
			::InvalidateRect(combobox, 0, TRUE);

			return TRUE;
		}

		//
		// ダイアログプロシージャです。
		//
		INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					MY_TRACE_FUNC("WM_COMMAND, 0x%08X, 0x%08X", wParam, lParam);

					UINT code = HIWORD(wParam);
					UINT id = LOWORD(wParam);
					HWND control = (HWND)lParam;

					switch (id)
					{
					case IDOK:
					case IDCANCEL: return 0;
					case IDC_ITEM_WIDTH:
					case IDC_ITEM_HEIGHT:
					case IDC_FONT_HEIGHT:
					case IDC_PREVIEW_TEXT:
					case IDC_NORMAL_THEME:
					case IDC_SELECT_THEME:
						{
							if (code == EN_UPDATE) updateConfig();

							break;
						}
					case IDC_PAINT_MODE:
						{
							if (code == CBN_SELCHANGE) updateConfig();

							break;
						}
					case IDC_NORMAL_FILL:
					case IDC_NORMAL_TEXT:
					case IDC_NORMAL_FONT:
					case IDC_SELECT_FILL:
					case IDC_SELECT_TEXT:
					case IDC_SELECT_FONT:
						{
							COLORREF color = getUInt(id);

							static COLORREF customColors[16] = {};
							CHOOSECOLOR cc { sizeof(cc) };
							cc.hwndOwner = hwnd;
							cc.lpCustColors = customColors;
							cc.rgbResult = color;
							cc.Flags = CC_RGBINIT | CC_FULLOPEN;
							if (!::ChooseColor(&cc)) return TRUE;

							color = cc.rgbResult;

							setUInt(id, color);
							::InvalidateRect(control, 0, FALSE);

							return TRUE;
						}
					case IDC_MARGE:
					case IDC_USE_CONTEXT_MENU:
					case IDC_USE_BACKWARD:
					case IDC_USE_FORWARD:
						{
							updateConfig();

							break;
						}
					}

					break;
				}
			case WM_DRAWITEM:
				{
					UINT id = wParam;

					switch (id)
					{
					case IDC_NORMAL_FILL:
					case IDC_NORMAL_TEXT:
					case IDC_NORMAL_FONT:
					case IDC_SELECT_FILL:
					case IDC_SELECT_TEXT:
					case IDC_SELECT_FONT:
						{
							auto dis = (DRAWITEMSTRUCT*)lParam;

							COLORREF color = getUInt(id);
							GdiObj<HBRUSH> brush = ::CreateSolidBrush(color);
							::FillRect(dis->hDC, &dis->rcItem, brush);

							return TRUE;
						}
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
