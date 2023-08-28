#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------

// デバッグ用コールバック関数。デバッグメッセージを出力する
void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

//---------------------------------------------------------------------

AviUtlInternal g_auin;
HINSTANCE g_instance = 0;
HWND g_hub = 0;
HTHEME g_theme = 0;
HMENU g_colonyMenu = 0;
HHOOK g_gmHook = 0;

AviUtlWindowPtr g_aviutlWindow(new AviUtlWindow());
ExEditWindowPtr g_exeditWindow(new ExEditWindow());
SettingDialogPtr g_settingDialog(new SettingDialog());

ColonyManager g_colonyManager;
ShuttleManager g_shuttleManager;
PanePtr g_hotBorderPane;

int g_borderWidth = 8;
int g_captionHeight = 24;
int g_tabHeight = 24;
int g_menuBreak = 30;
int g_tabMode = TabMode::bottom;
COLORREF g_fillColor = RGB(0x99, 0x99, 0x99);
COLORREF g_borderColor = RGB(0xcc, 0xcc, 0xcc);
COLORREF g_hotBorderColor = RGB(0x00, 0x00, 0x00);
COLORREF g_activeCaptionColor = ::GetSysColor(COLOR_HIGHLIGHT);
COLORREF g_activeCaptionTextColor = RGB(0xff, 0xff, 0xff);
COLORREF g_inactiveCaptionColor = ::GetSysColor(COLOR_HIGHLIGHTTEXT);
COLORREF g_inactiveCaptionTextColor = RGB(0x00, 0x00, 0x00);
BOOL g_useTheme = FALSE;
BOOL g_forceScroll = FALSE;
BOOL g_showPlayer = FALSE;

BOOL g_movieplaymain = FALSE;

//---------------------------------------------------------------------

// メニューのコピーを作成する。
void copyMenu(HMENU srcMenu, HMENU dstMenu)
{
	int c = ::GetMenuItemCount(srcMenu);

	for (int i = 0; i < c; i++)
	{
		TCHAR text[MAX_PATH] = {};
		MENUITEMINFO mii = { sizeof(mii) };
		mii.fMask = MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU;
		mii.dwTypeData = text;
		mii.cch = MAX_PATH;
		::GetMenuItemInfo(srcMenu, i, TRUE, &mii);

		if (mii.hSubMenu)
		{
			HMENU subMenu = ::CreatePopupMenu();
			copyMenu(mii.hSubMenu, subMenu);
			mii.hSubMenu = subMenu;
		}

		::InsertMenuItem(dstMenu, i, TRUE, &mii);
	}
}

// ターゲットのメニューを表示する。
BOOL showTargetMenu(HWND hwndColony, POINT point)
{
	PanePtr root = g_colonyManager.getRootPane(hwndColony);

	// ペインを取得する。
	PanePtr pane = root->hitTestPane(point);
	if (!pane) return FALSE;

	Shuttle* shuttle = pane->getActiveShuttle();
	if (!shuttle) return FALSE;

	// 座標をチェックする。
	RECT rcMenu = pane->getMenuRect();
	if (!::PtInRect(&rcMenu, point)) return FALSE;

	// メニューを表示する座標を算出する。
	point.x = rcMenu.left;
	point.y = rcMenu.bottom;
	::ClientToScreen(hwndColony, &point);

	// ターゲットを取得する。
	HWND hwnd = shuttle->m_hwnd;

	// ターゲットのメニューを取得する。
	HMENU srcMenu = ::GetMenu(hwnd);
	if (!srcMenu) return FALSE;

	// ポップアップメニューを作成する。
	HMENU dstMenu = ::CreatePopupMenu();
	copyMenu(srcMenu, dstMenu);

	// ポップアップメニューを表示する。
	::TrackPopupMenuEx(dstMenu, TPM_VERPOSANIMATION, point.x, point.y, hwnd, 0);

	// ポップアップメニューを削除する。
	::DestroyMenu(dstMenu);

	return TRUE;
}

// ペインの設定をするメニューを表示する。
void showPaneMenu(HWND hwndColony)
{
	PanePtr root = g_colonyManager.getRootPane(hwndColony);

	POINT cursorPos; ::GetCursorPos(&cursorPos);
	POINT point = cursorPos;
	::ScreenToClient(hwndColony, &point);

	PanePtr pane = root->hitTestPane(point);
	if (!pane) return;

	POINT point2 = point;
	::MapWindowPoints(hwndColony, pane->m_owner, &point2, 1);
	BOOL inCaption = pane->hitTestCaption(point2);

	int c = pane->m_tab.getTabCount();
	int ht = pane->m_tab.hitTest(point);
	Shuttle* shuttle = 0;
	if (ht != -1)
		shuttle = pane->m_tab.getShuttle(ht);
	else if (inCaption)
		shuttle = pane->m_tab.getShuttle(pane->m_tab.getCurrentIndex());
	HWND colonyInShuttle = 0;
	if (shuttle && g_colonyManager.getRootPane(shuttle->m_hwnd))
		colonyInShuttle = shuttle->m_hwnd;

	HMENU menu = ::CreatePopupMenu();

	::AppendMenu(menu, MF_STRING, CommandID::SPLIT_MODE_NONE, _T("分割なし"));
	::AppendMenu(menu, MF_STRING, CommandID::SPLIT_MODE_VERT, _T("垂直線で分割"));
	::AppendMenu(menu, MF_STRING, CommandID::SPLIT_MODE_HORZ, _T("水平線で分割"));
	::AppendMenu(menu, MF_SEPARATOR, -1, 0);
	::AppendMenu(menu, MF_STRING, CommandID::ORIGIN_TOP_LEFT, _T("左上を原点にする"));
	::AppendMenu(menu, MF_STRING, CommandID::ORIGIN_BOTTOM_RIGHT, _T("右下を原点にする"));
	::AppendMenu(menu, MF_SEPARATOR, -1, 0);
	::AppendMenu(menu, MF_STRING, CommandID::MOVE_TO_LEFT, _T("左に移動する"));
	if (ht == -1 || ht <= 0)
		::EnableMenuItem(menu, CommandID::MOVE_TO_LEFT, MF_GRAYED | MF_DISABLED);
	::AppendMenu(menu, MF_STRING, CommandID::MOVE_TO_RIGHT, _T("右に移動する"));
	if (ht == -1 || ht >= c - 1)
		::EnableMenuItem(menu, CommandID::MOVE_TO_RIGHT, MF_GRAYED | MF_DISABLED);
	::AppendMenu(menu, MF_STRING, CommandID::IS_BORDER_LOCKED, _T("ボーダーをロック"));
	if (pane->m_isBorderLocked)
		::CheckMenuItem(menu, CommandID::IS_BORDER_LOCKED, MF_CHECKED);
	::AppendMenu(menu, MF_SEPARATOR, -1, 0);
	::AppendMenu(menu, MF_STRING, CommandID::RENAME_COLONY, _T("名前を変更"));
	if (!colonyInShuttle)
		::EnableMenuItem(menu, CommandID::RENAME_COLONY, MF_GRAYED | MF_DISABLED);

	{
		::AppendMenu(menu, MF_STRING | MF_MENUBARBREAK, CommandID::WINDOW, _T("ドッキングを解除"));
		// ドッキングしているシャトルが存在しない場合はこのメニューアイテムを無効化する。
		if (c == 0)
			::EnableMenuItem(menu, CommandID::WINDOW, MF_DISABLED | MF_GRAYED);

		// 表示状態のウィンドウをメニューに追加する。
		int index = 1;
		for (auto& x : g_shuttleManager.m_map)
		{
			ShuttlePtr shuttle = x.second;

			// 非表示状態のウィンドウはスキップする。
			if (!::IsWindowVisible(shuttle->m_hwnd)) continue;

			// メニューアイテムを追加する。
			::AppendMenu(menu, MF_STRING, CommandID::WINDOW + index, x.first);

			// ウィンドウのタブが存在するなら
			if (pane->m_tab.findTab(shuttle.get()) != -1)
			{
				// メニューアイテムにチェックを入れる。
				::CheckMenuItem(menu, CommandID::WINDOW + index, MF_CHECKED);
			}

			// ドッキング不可能なら
			if (!g_colonyManager.canDocking(hwndColony, shuttle->m_hwnd))
			{
				// メニューアイテムを無効にする。
				::EnableMenuItem(menu, CommandID::WINDOW + index, MF_DISABLED | MF_GRAYED);
			}

			// カウンタをインクリメントする。
			index++;
		}

		// 非表示状態のウィンドウをメニューに追加する。
		int i = 0;
		for (auto& x : g_shuttleManager.m_map)
		{
			ShuttlePtr shuttle = x.second;

			// 表示状態のウィンドウはスキップする。
			if (::IsWindowVisible(shuttle->m_hwnd)) continue;

			UINT flags = MF_STRING;

			// 最初のメニューアイテムなら
			if (i == 0)
			{
				// 折り返しフラグを設定する。
				flags |= MF_MENUBARBREAK;
			}
			// 折り返し設定が有効なら
			else if (g_menuBreak)
			{
				// 折り返し位置かチェックする。
				if (i % g_menuBreak == 0)
				{
					// 折り返しフラグを設定する。
					flags |= MF_MENUBARBREAK;
				}
			}

			// メニューアイテムを追加する。
			::AppendMenu(menu, flags, CommandID::WINDOW + index, x.first);

			// ウィンドウのタブが存在するなら
			if (pane->m_tab.findTab(shuttle.get()) != -1)
			{
				// メニューアイテムにチェックを入れる。
				::CheckMenuItem(menu, CommandID::WINDOW + index, MF_CHECKED);
			}

			// ドッキング不可能なら
			if (!g_colonyManager.canDocking(hwndColony, shuttle->m_hwnd))
			{
				// メニューアイテムを無効にする。
				::EnableMenuItem(menu, CommandID::WINDOW + index, MF_DISABLED | MF_GRAYED);
			}

			// カウンタをインクリメントする。
			index++;
			i++;
		}
	}

	switch (pane->m_splitMode)
	{
	case SplitMode::none: ::CheckMenuItem(menu, CommandID::SPLIT_MODE_NONE, MF_CHECKED); break;
	case SplitMode::vert: ::CheckMenuItem(menu, CommandID::SPLIT_MODE_VERT, MF_CHECKED); break;
	case SplitMode::horz: ::CheckMenuItem(menu, CommandID::SPLIT_MODE_HORZ, MF_CHECKED); break;
	}

	switch (pane->m_origin)
	{
	case Origin::topLeft: ::CheckMenuItem(menu, CommandID::ORIGIN_TOP_LEFT, MF_CHECKED); break;
	case Origin::bottomRight: ::CheckMenuItem(menu, CommandID::ORIGIN_BOTTOM_RIGHT, MF_CHECKED); break;
	}

	int id = ::TrackPopupMenu(menu, TPM_NONOTIFY | TPM_RETURNCMD, cursorPos.x, cursorPos.y, 0, hwndColony, 0);

	if (id)
	{
		switch (id)
		{
		case CommandID::SPLIT_MODE_NONE: pane->setSplitMode(SplitMode::none); break;
		case CommandID::SPLIT_MODE_VERT: pane->setSplitMode(SplitMode::vert); break;
		case CommandID::SPLIT_MODE_HORZ: pane->setSplitMode(SplitMode::horz); break;

		case CommandID::ORIGIN_TOP_LEFT: pane->m_origin = Origin::topLeft; break;
		case CommandID::ORIGIN_BOTTOM_RIGHT: pane->m_origin = Origin::bottomRight; break;

		case CommandID::MOVE_TO_LEFT: pane->m_tab.moveTab(ht, ht - 1); break;
		case CommandID::MOVE_TO_RIGHT: pane->m_tab.moveTab(ht, ht + 1); break;

		case CommandID::IS_BORDER_LOCKED: pane->m_isBorderLocked = !pane->m_isBorderLocked; break;

		case CommandID::RENAME_COLONY: g_shuttleManager.renameShuttle(hwndColony, shuttle); break;
		}

		if (id == CommandID::WINDOW)
		{
			// ドッキングを解除する。

			int index = ht;

			if (index == -1)
				index = pane->m_tab.getCurrentIndex();

			if (index != -1)
			{
				Shuttle* shuttle = pane->m_tab.getShuttle(index);
				pane->removeShuttle(shuttle);
			}
		}
		else if (id > CommandID::WINDOW)
		{
			// ウィンドウをドッキングする。

			TCHAR text[MAX_PATH] = {};
			::GetMenuString(menu, id, text, MAX_PATH, MF_BYCOMMAND);
			MY_TRACE_TSTR(text);

			ShuttlePtr shuttle = g_shuttleManager.getShuttle(text);
			if (shuttle && g_colonyManager.canDocking(hwndColony, shuttle->m_hwnd))
			{
				Pane* oldPane = shuttle->m_pane;

				int index = pane->addShuttle(shuttle.get(), ht);
				if (index != -1)
				{
					if (oldPane)
						oldPane->recalcLayout();

					pane->m_tab.setCurrentIndex(index);
					pane->m_tab.changeCurrent();
				}
			}
		}

		pane->recalcLayout();

		::InvalidateRect(hwndColony, 0, FALSE);
	}

	::DestroyMenu(menu);
}

void fillBackground(HDC dc, LPCRECT rc)
{
	// テーマを使用するなら
	if (g_useTheme)
	{
		int partId = WP_DIALOG;
		int stateId = 0;

		// テーマ API を使用して背景を描画する。
		::DrawThemeBackground(g_theme, dc, partId, stateId, rc, 0);
	}
	// テーマを使用しないなら
	else
	{
		// ブラシで塗りつぶす。
		HBRUSH brush = ::CreateSolidBrush(g_fillColor);
		::FillRect(dc, rc, brush);
		::DeleteObject(brush);
	}
}

// 他のポップアップウィンドウの土台となるポップアップウィンドウを作成する。
HWND createPopupWindow(HWND parent)
{
	MY_TRACE(_T("createPopupWindow(0x%08X)\n"), parent);

	WNDCLASS wc = {};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.lpfnWndProc = ::DefWindowProc;
	wc.hInstance = g_instance;
	wc.lpszClassName = _T("PopupWindow");
	::RegisterClass(&wc);

	HWND hwnd = ::CreateWindowEx(
		0,
		_T("PopupWindow"),
		_T("PopupWindow"),
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME |
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		parent, 0, g_instance, 0);

	return hwnd;
}

//---------------------------------------------------------------------
