#include "pch.h"
#include "ConfigDialog.h"

//---------------------------------------------------------------------

int showConfigDialog(HWND hwnd)
{
	ConfigDialog dialog;
	dialog.create(g_instance, MAKEINTRESOURCE(IDD_CONFIG), hwnd);

	::SetDlgItemInt(dialog, IDC_FILL_COLOR, g_fillColor, FALSE);
	::SetDlgItemInt(dialog, IDC_BORDER_COLOR, g_borderColor, FALSE);
	::SetDlgItemInt(dialog, IDC_HOT_BORDER_COLOR, g_hotBorderColor, FALSE);
	::SetDlgItemInt(dialog, IDC_ACTIVE_CAPTION_COLOR, g_activeCaptionColor, FALSE);
	::SetDlgItemInt(dialog, IDC_ACTIVE_CAPTION_TEXT_COLOR, g_activeCaptionTextColor, FALSE);
	::SetDlgItemInt(dialog, IDC_INACTIVE_CAPTION_COLOR, g_inactiveCaptionColor, FALSE);
	::SetDlgItemInt(dialog, IDC_INACTIVE_CAPTION_TEXT_COLOR, g_inactiveCaptionTextColor, FALSE);
	::SetDlgItemInt(dialog, IDC_BORDER_WIDTH, g_borderWidth, FALSE);
	::SetDlgItemInt(dialog, IDC_CAPTION_HEIGHT, g_captionHeight, FALSE);
	::SetDlgItemInt(dialog, IDC_TAB_HEIGHT, g_tabHeight, FALSE);
	::SetDlgItemInt(dialog, IDC_MENU_BREAK, g_menuBreak, FALSE);
	HWND hwndTabMode = ::GetDlgItem(dialog, IDC_TAB_MODE);
	ComboBox_AddString(hwndTabMode, _T("タイトル"));
	ComboBox_AddString(hwndTabMode, _T("上"));
	ComboBox_AddString(hwndTabMode, _T("下"));
	ComboBox_SetCurSel(hwndTabMode, g_tabMode);
	HWND hwndUseTheme = ::GetDlgItem(dialog, IDC_USE_THEME);
	Button_SetCheck(hwndUseTheme, g_useTheme);
	HWND hwndForceScroll = ::GetDlgItem(dialog, IDC_FORCE_SCROLL);
	Button_SetCheck(hwndForceScroll, g_forceScroll);
	HWND hwndShowPlayer = ::GetDlgItem(dialog, IDC_SHOW_PLAYER);
	Button_SetCheck(hwndShowPlayer, g_showPlayer);

	::EnableWindow(hwnd, FALSE);
	int retValue = dialog.doModal();
	::EnableWindow(hwnd, TRUE);
	::SetActiveWindow(hwnd);

	if (IDOK != retValue)
		return retValue;

	g_fillColor = ::GetDlgItemInt(dialog, IDC_FILL_COLOR, 0, FALSE);
	g_borderColor = ::GetDlgItemInt(dialog, IDC_BORDER_COLOR, 0, FALSE);
	g_hotBorderColor = ::GetDlgItemInt(dialog, IDC_HOT_BORDER_COLOR, 0, FALSE);
	g_activeCaptionColor = ::GetDlgItemInt(dialog, IDC_ACTIVE_CAPTION_COLOR, 0, FALSE);
	g_activeCaptionTextColor = ::GetDlgItemInt(dialog, IDC_ACTIVE_CAPTION_TEXT_COLOR, 0, FALSE);
	g_inactiveCaptionColor = ::GetDlgItemInt(dialog, IDC_INACTIVE_CAPTION_COLOR, 0, FALSE);
	g_inactiveCaptionTextColor = ::GetDlgItemInt(dialog, IDC_INACTIVE_CAPTION_TEXT_COLOR, 0, FALSE);
	g_borderWidth = ::GetDlgItemInt(dialog, IDC_BORDER_WIDTH, 0, FALSE);
	g_captionHeight = ::GetDlgItemInt(dialog, IDC_CAPTION_HEIGHT, 0, FALSE);
	g_tabHeight = ::GetDlgItemInt(dialog, IDC_TAB_HEIGHT, 0, FALSE);
	g_menuBreak = ::GetDlgItemInt(dialog, IDC_MENU_BREAK, 0, FALSE);
	g_tabMode = ComboBox_GetCurSel(hwndTabMode);
	g_useTheme = Button_GetCheck(hwndUseTheme);
	g_forceScroll = Button_GetCheck(hwndForceScroll);
	g_showPlayer = Button_GetCheck(hwndShowPlayer);

	// レイアウトを再計算する。
	g_colonyManager.calcAllLayout();

	return retValue;
}

//---------------------------------------------------------------------

void ConfigDialog::onOK()
{
	Dialog::onOK();
}

void ConfigDialog::onCancel()
{
	Dialog::onCancel();
}

INT_PTR ConfigDialog::onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
		{
			UINT id = LOWORD(wParam);

			switch (id)
			{
			case IDC_FILL_COLOR:
			case IDC_BORDER_COLOR:
			case IDC_HOT_BORDER_COLOR:
			case IDC_ACTIVE_CAPTION_COLOR:
			case IDC_ACTIVE_CAPTION_TEXT_COLOR:
			case IDC_INACTIVE_CAPTION_COLOR:
			case IDC_INACTIVE_CAPTION_TEXT_COLOR:
				{
					HWND control = (HWND)lParam;

					COLORREF color = ::GetDlgItemInt(hwnd, id, 0, FALSE);

					static COLORREF customColors[16] = {};
					CHOOSECOLOR cc { sizeof(cc) };
					cc.hwndOwner = hwnd;
					cc.lpCustColors = customColors;
					cc.rgbResult = color;
					cc.Flags = CC_RGBINIT | CC_FULLOPEN;
					if (!::ChooseColor(&cc)) return TRUE;

					color = cc.rgbResult;

					::SetDlgItemInt(hwnd, id, color, FALSE);
					::InvalidateRect(control, 0, FALSE);

					return TRUE;
				}
			}

			break;
		}
	case WM_DRAWITEM:
		{
			UINT id = wParam;

			switch (id)
			{
			case IDC_FILL_COLOR:
			case IDC_BORDER_COLOR:
			case IDC_HOT_BORDER_COLOR:
			case IDC_ACTIVE_CAPTION_COLOR:
			case IDC_ACTIVE_CAPTION_TEXT_COLOR:
			case IDC_INACTIVE_CAPTION_COLOR:
			case IDC_INACTIVE_CAPTION_TEXT_COLOR:
				{
					DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;

					COLORREF color = ::GetDlgItemInt(hwnd, id, 0, FALSE);

					HBRUSH brush = ::CreateSolidBrush(color);
					FillRect(dis->hDC, &dis->rcItem, brush);
					::DeleteObject(brush);

					return TRUE;
				}
			}

			break;
		}
	}

	return Dialog::onDlgProc(hwnd, message, wParam, lParam);
}

//---------------------------------------------------------------------
