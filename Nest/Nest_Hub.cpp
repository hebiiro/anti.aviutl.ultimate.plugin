#include "pch.h"
#include "Nest.h"
#include "ConfigDialog.h"

//---------------------------------------------------------------------
namespace Hub {
//---------------------------------------------------------------------

// 他のウィンドウの土台となるシングルウィンドウを作成する。
HWND create()
{
	MY_TRACE(_T("Hub::create()\n"));

	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.lpfnWndProc = Hub::wndProc;
	wc.hInstance = g_instance;
	wc.lpszClassName = _T("AviUtl"); // クラス名を AviUtl に偽装する。「AoiSupport」用。
	::RegisterClass(&wc);

	HWND hwnd = ::CreateWindowEx(
		0,
		_T("Nest"), // フックして AviUtl に置き換える。
		_T("Nest"),
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME |
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, g_instance, 0);

	return hwnd;
}

void initSystemMenu()
{
	HMENU menu = ::GetSystemMenu(g_hub, FALSE);
	int index = 0;
	::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::CREATE_COLONY, _T("コロニーを新規作成"));
	::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::IMPORT_LAYOUT, _T("レイアウトのインポート"));
	::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::EXPORT_LAYOUT, _T("レイアウトのエクスポート"));
	::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::SHOW_CONFIG_DIALOG, _T("Nestの設定"));
	::InsertMenu(menu, index++, MF_BYPOSITION | MF_POPUP, (UINT)g_colonyMenu, _T("コロニー"));
	if (::GetModuleHandle(_T("PSDToolKit.auf")))
		::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::SHOW_PSD_TOOL_KIT, _T("PSDToolKit(外部)を表示"));
	::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, CommandID::SHOW_BOUYOMISAN, _T("棒読みさんを表示"));
	::InsertMenu(menu, index++, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
}

//---------------------------------------------------------------------

// ファイルからレイアウトをインポートする。
BOOL importLayout(HWND hwnd)
{
	// ファイル選択ダイアログを表示してファイル名を取得する。

	WCHAR fileName[MAX_PATH] = {};

	WCHAR folderName[MAX_PATH] = {};
	::GetModuleFileNameW(g_instance, folderName, MAX_PATH);
	::PathRemoveExtensionW(folderName);

	OPENFILENAMEW ofn = { sizeof(ofn) };
	ofn.hwndOwner = hwnd;
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = L"レイアウトのインポート";
	ofn.lpstrInitialDir = folderName;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"レイアウトファイル (*.xml)\0*.xml\0" "すべてのファイル (*.*)\0*.*\0";
	ofn.lpstrDefExt = L"xml";

	if (!::GetOpenFileNameW(&ofn))
		return FALSE;

	// レイアウトファイルをインポートする。
	loadConfig(fileName, TRUE);

	// レイアウトを再計算する。
	g_colonyManager.calcAllLayout();

	// シングルウィンドウが非表示なら表示する。
	if (!::IsWindowVisible(hwnd))
		::ShowWindow(hwnd, SW_SHOW);

	// シングルウィンドウをフォアグラウンドにする。
	::SetForegroundWindow(hwnd);
	::SetActiveWindow(hwnd);

	return TRUE;
}

// ファイルにレイアウトをエクスポートする。
BOOL exportLayout(HWND hwnd)
{
	// ファイル選択ダイアログを表示してファイル名を取得する。

	WCHAR fileName[MAX_PATH] = {};

	WCHAR folderName[MAX_PATH] = {};
	::GetModuleFileNameW(g_instance, folderName, MAX_PATH);
	::PathRemoveExtensionW(folderName);

	OPENFILENAMEW ofn = { sizeof(ofn) };
	ofn.hwndOwner = hwnd;
	ofn.Flags = OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle = L"レイアウトのエクスポート";
	ofn.lpstrInitialDir = folderName;
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"レイアウトファイル (*.xml)\0*.xml\0" "すべてのファイル (*.*)\0*.*\0";
	ofn.lpstrDefExt = L"xml";

	if (!::GetSaveFileNameW(&ofn))
		return FALSE;

	// レイアウトファイルをエクスポートする。
	saveConfig(fileName, TRUE);

	return TRUE;
}

//---------------------------------------------------------------------

void updateMenu()
{
	HMENU menu = ::GetMenu(g_hub);

	LPCTSTR text = _T("再生時最大化 OFF");
	if (g_showPlayer) text = _T("再生時最大化 ON");

	MENUITEMINFO mii = { sizeof(mii) };
	mii.fMask = MIIM_STRING;
	mii.dwTypeData = (LPTSTR)text;

	::SetMenuItemInfo(menu, CommandID::MAXIMIZE_PLAY, FALSE, &mii);

	::DrawMenuBar(g_hub);
}

//---------------------------------------------------------------------

// シングルウィンドウのウィンドウ関数。
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
		{
			MY_TRACE(_T("Hub::wndProc(WM_***KEY***, 0x%08X, 0x%08X, 0x%08X)\n"), message, wParam, lParam);

			return ::SendMessage(g_aviutlWindow->m_hwnd, message, wParam, lParam);
		}
	case WM_ACTIVATE: // 「patch.aul」用。
		{
			MY_TRACE(_T("Hub::wndProc(WM_ACTIVATE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (LOWORD(wParam) == WA_INACTIVE)
				::SendMessage(g_aviutlWindow->m_hwnd, message, wParam, lParam);

			break;
		}
	case WM_MENUSELECT: // 「patch.aul」用。
		{
			MY_TRACE(_T("Hub::wndProc(WM_MENUSELECT, 0x%08X, 0x%08X)\n"), wParam, lParam);

			::SendMessage(g_aviutlWindow->m_hwnd, message, wParam, lParam);

			break;
		}
	case WM_CLOSE:
		{
			MY_TRACE(_T("Hub::wndProc(WM_CLOSE, 0x%08X, 0x%08X)\n"), wParam, lParam);

			return ::SendMessage(g_aviutlWindow->m_hwnd, message, wParam, lParam);
		}
	case WM_COMMAND:
		{
			MY_TRACE(_T("Hub::wndProc(WM_COMMAND, 0x%08X, 0x%08X)\n"), wParam, lParam);

			UINT id = LOWORD(wParam);

			switch (id)
			{
			case CommandID::MAXIMIZE_PLAY:
				{
					MY_TRACE(_T("CommandID::MAXIMIZE_PLAY\n"));

					g_showPlayer = !g_showPlayer;

					updateMenu();

					break;
				}
			}

			return ::SendMessage(g_aviutlWindow->m_hwnd, message, wParam, lParam);
		}
	case WM_SYSCOMMAND:
		{
			MY_TRACE(_T("Hub::wndProc(WM_SYSCOMMAND, 0x%08X, 0x%08X)\n"), wParam, lParam);

			if (wParam >= CommandID::COLONY_BEGIN &&
				wParam < CommandID::COLONY_END)
			{
				g_colonyManager.executeColonyMenu(wParam);
				break;
			}

			switch (wParam)
			{
			case CommandID::SHOW_CONFIG_DIALOG:
				{
					// Nest の設定ダイアログを開く。
					if (IDOK == showConfigDialog(hwnd))
					{
						// 設定が変更された可能性があるので、ハブのメニューを更新する。
						updateMenu();
					}

					break;
				}
			case CommandID::IMPORT_LAYOUT:
				{
					// レイアウトファイルをインポートする。
					importLayout(hwnd);

					break;
				}
			case CommandID::EXPORT_LAYOUT:
				{
					// レイアウトファイルをエクスポートする。
					exportLayout(hwnd);

					break;
				}
			case CommandID::CREATE_COLONY:
				{
					// コロニーを新規作成する。
					createColony(0);

					break;
				}
			case CommandID::SHOW_PSD_TOOL_KIT:
				{
					PSDToolKit::showHolder();

					break;
				}
			case CommandID::SHOW_BOUYOMISAN:
				{
					Bouyomisan::showHolder();

					break;
				}
			case SC_RESTORE:
			case SC_MINIMIZE: // 「patch.aul」用。
				{
					::SendMessage(g_aviutlWindow->m_hwnd, message, wParam, lParam);

					break;
				}
			}

			break;
		}
	case WM_INITMENUPOPUP:
		{
			if ((HMENU)wParam == g_colonyMenu)
				g_colonyManager.updateColonyMenu();

			break;
		}
	case WM_CREATE:
		{
			MY_TRACE(_T("Hub::wndProc(WM_CREATE)\n"));

			g_theme = ::OpenThemeData(hwnd, VSCLASS_WINDOW);
			MY_TRACE_HEX(g_theme);

			g_colonyMenu = ::CreatePopupMenu();
			MY_TRACE_HEX(g_colonyMenu);

			break;
		}
	case WM_DESTROY:
		{
			MY_TRACE(_T("Hub::wndProc(WM_DESTROY)\n"));

			::DestroyMenu(g_colonyMenu), g_colonyMenu = 0;
			::CloseThemeData(g_theme), g_theme = 0;

			break;
		}
	case WindowMessage::WM_POST_INIT: // 最後の初期化処理。
		{
			// システムメニューに独自の項目を追加する。
			initSystemMenu();

			// 設定をファイルから読み込む。
			if (loadConfig() == S_OK)
				updateMenu();

			// シングルウィンドウが非表示なら表示する。
			if (!::IsWindowVisible(hwnd))
				::ShowWindow(hwnd, SW_SHOW);

			// 最初のレイアウト計算。
			g_colonyManager.calcAllLayout();

			// シングルウィンドウをフォアグラウンドにする。
			::SetForegroundWindow(hwnd);
			::SetActiveWindow(hwnd);

			break;
		}
	}

	return colonyProc(hwnd, message, wParam, lParam);
}

//---------------------------------------------------------------------
} // namespace Hub
//---------------------------------------------------------------------
