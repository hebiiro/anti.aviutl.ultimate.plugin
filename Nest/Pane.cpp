#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------

TabControl::TabControl(Pane* pane)
{
	m_hwnd = ::CreateWindowEx(
		0,
		WC_TABCONTROL,
		_T("Nest.Tab"),
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
		TCS_FOCUSNEVER,
		0, 0, 0, 0,
		pane->m_owner, 0, g_instance, 0);

	setPane(m_hwnd, pane);

	AviUtl::SysInfo si = {};
	g_auin.get_sys_info(0, &si);
	::SendMessage(m_hwnd, WM_SETFONT, (WPARAM)si.hfont, TRUE);
}

TabControl::~TabControl()
{
	::DestroyWindow(m_hwnd);
}

Pane* TabControl::getPane(HWND hwnd)
{
	return (Pane*)::GetProp(hwnd, _T("Nest.Pane"));
}

void TabControl::setPane(HWND hwnd, Pane* pane)
{
	::SetProp(hwnd, _T("Nest.Pane"), pane);
}

int TabControl::getTabCount()
{
	return ::SendMessage(m_hwnd, TCM_GETITEMCOUNT, 0, 0);
}

Shuttle* TabControl::getShuttle(int index)
{
	if (index == -1) return 0;

	TCITEM item = {};
	item.mask = TCIF_PARAM;
	::SendMessage(m_hwnd, TCM_GETITEM, index, (LPARAM)&item);

	return (Shuttle*)item.lParam;
}

int TabControl::getCurrentIndex()
{
	return ::SendMessage(m_hwnd, TCM_GETCURSEL, 0, 0);
}

int TabControl::setCurrentIndex(int index)
{
	if (index < 0 || index >= getTabCount())
		index = 0;

	return ::SendMessage(m_hwnd, TCM_SETCURSEL, index, 0);
}

int TabControl::hitTest(POINT point)
{
	TCHITTESTINFO hti = {};
	hti.pt = point;
	Pane* pane = getPane(m_hwnd);
	::MapWindowPoints(pane->m_owner, m_hwnd, &hti.pt, 1);
	return ::SendMessage(m_hwnd, TCM_HITTEST, 0, (LPARAM)&hti);
}

int TabControl::addTab(Shuttle* shuttle, LPCTSTR text, int index)
{
	TCITEM item = {};
	item.mask = TCIF_PARAM | TCIF_TEXT;
	item.lParam = (LPARAM)shuttle;
	item.pszText = (LPTSTR)text;
	return ::SendMessage(m_hwnd, TCM_INSERTITEM, index, (LPARAM)&item);
}

void TabControl::deleteTab(int index)
{
	::SendMessage(m_hwnd, TCM_DELETEITEM, index, 0);
}

void TabControl::deleteAllTabs()
{
	::SendMessage(m_hwnd, TCM_DELETEALLITEMS, 0, 0);
}

int TabControl::findTab(Shuttle* shuttle)
{
	int c = getTabCount();
	for (int i = 0; i < c; i++)
	{
		if (getShuttle(i) == shuttle)
			return i;
	}

	return -1;
}

int TabControl::moveTab(int from, int to)
{
	int c = getTabCount();

	if (from < 0 || from >= c) return -1;
	if (to < 0 || to >= c) return -1;

	int current = getCurrentIndex();
	TCHAR text[MAX_PATH] = {};
	TCITEM item = {};
	item.mask = TCIF_PARAM | TCIF_TEXT;
	item.pszText = text;
	item.cchTextMax = MAX_PATH;
	::SendMessage(m_hwnd, TCM_GETITEM, from, (LPARAM)&item);
	::SendMessage(m_hwnd, TCM_DELETEITEM, from, 0);
	::SendMessage(m_hwnd, TCM_INSERTITEM, to, (LPARAM)&item);
	if (from == current) setCurrentIndex(to);

	return to;
}

void TabControl::changeCurrent()
{
	int current = getCurrentIndex();
	int c = getTabCount();
	for (int i = 0; i < c; i++)
	{
		Shuttle* shuttle = getShuttle(i);

		if (i == current)
		{
			MY_TRACE(_T("%ws を表示します\n"), (BSTR)shuttle->m_name);

			::ShowWindow(shuttle->m_hwnd, SW_SHOW);
			::ShowWindow(shuttle->m_dockContainer->m_hwnd, SW_SHOW);
		}
		else
		{
			MY_TRACE(_T("%ws を非表示にします\n"), (BSTR)shuttle->m_name);

			::ShowWindow(shuttle->m_hwnd, SW_HIDE);
			::ShowWindow(shuttle->m_dockContainer->m_hwnd, SW_HIDE);
		}

		MY_TRACE(_T("%d\n"), ::IsWindowVisible(shuttle->m_hwnd));
		MY_TRACE(_T("%d\n"), ::IsWindowVisible(shuttle->m_dockContainer->m_hwnd));
	}

	Pane* pane = getPane(m_hwnd);
	::InvalidateRect(pane->m_owner, &pane->m_position, FALSE);
}

void TabControl::changeText(Shuttle* shuttle, LPCTSTR text)
{
	int index = findTab(shuttle);
	if (index == -1) return;

	TCITEM item = {};
	item.mask = TCIF_TEXT;
	item.pszText = (LPTSTR)text;
	::SendMessage(m_hwnd, TCM_SETITEM, index, (LPARAM)&item);
}

//---------------------------------------------------------------------

Pane::Pane(HWND owner)
	: m_tab(this)
	, m_owner(owner)
{
}

Pane::~Pane()
{
}

Shuttle* Pane::getActiveShuttle()
{
	int current = m_tab.getCurrentIndex();
	return m_tab.getShuttle(current);
}

int Pane::addShuttle(Shuttle* shuttle, int index)
{
	MY_TRACE(_T("Pane::addShuttle(0x%08X, %d)\n"), shuttle, index);

	// ウィンドウはすでにドッキング済みなので何もしない。
	if (m_tab.findTab(shuttle) != -1) return -1;

	// ウィンドウが他のペインとドッキング済みなら
	if (shuttle->m_pane)
	{
		// 他のペインとのドッキングを解除させる。
		shuttle->m_pane->removeShuttle(shuttle);
	}

	// 追加位置が無効の場合は末尾に追加する。
	if (index == -1)
		index = m_tab.getTabCount();

	// ウィンドウテキストを取得する。
	TCHAR text[MAX_PATH] = {};
	::GetWindowText(shuttle->m_hwnd, text, MAX_PATH);

	// タブを追加する。
	int result = m_tab.addTab(shuttle, text, index);

	// ウィンドウをドッキング状態にする。
	shuttle->m_pane = this;
	shuttle->dockWindow();

	return result;
}

void Pane::removeShuttle(Shuttle* shuttle)
{
	MY_TRACE(_T("Pane::removeShuttle(0x%08X)\n"), shuttle);

	// ウィンドウ側からの参照を削除する。
	shuttle->m_pane = 0;

	// ペイン側からの参照を削除する。
	int index = m_tab.findTab(shuttle);
	if (index != -1)
	{
		BOOL same = index == m_tab.getCurrentIndex();

		// タブを削除する。
		m_tab.deleteTab(index);

		// 削除されたアイテムとカレントアイテムが同じなら
		if (same)
		{
			// カレントアイテムを設定し直す。
			m_tab.setCurrentIndex(index);
		}

		// カレントアイテムが無効なら
		if (m_tab.getCurrentIndex() == -1)
		{
			// 末尾のアイテムをカレントにする。
			m_tab.setCurrentIndex(m_tab.getTabCount() - 1);
		}

		m_tab.changeCurrent();
	}

	// ウィンドウをフローティング状態にする。
	shuttle->floatWindow();
}

void Pane::removeAllShuttles()
{
	// すべてのウィンドウをフローティング状態にする。
	int c = m_tab.getTabCount();
	for (int i = 0; i < c; i++)
	{
		Shuttle* shuttle = m_tab.getShuttle(i);

		shuttle->m_pane = 0;
		shuttle->floatWindow();
	}

	// すべてのタブを削除する。
	m_tab.deleteAllTabs();
}

// ペインをリセットする。
void Pane::resetPane()
{
	removeAllShuttles();

	for (auto& child : m_children)
	{
		if (!child) continue;

		child->resetPane();
		child = 0;
	}
}

// ペインの分割モードを変更する。
void Pane::setSplitMode(int splitMode)
{
	m_splitMode = splitMode;

	switch (m_splitMode)
	{
	case SplitMode::none:
		{
			resetPane();

			break;
		}
	case SplitMode::vert:
		{
			m_border = getWidth(m_position) / 2;

			if (!m_children[0]) m_children[0].reset(new Pane(m_owner));
			if (!m_children[1]) m_children[1].reset(new Pane(m_owner));

			break;
		}
	case SplitMode::horz:
		{
			m_border = getHeight(m_position) / 2;

			if (!m_children[0]) m_children[0].reset(new Pane(m_owner));
			if (!m_children[1]) m_children[1].reset(new Pane(m_owner));

			break;
		}
	}

	removeAllShuttles();
}

inline int clamp(int x, int minValue, int maxValue)
{
	if (x < minValue) return minValue;
	if (x > maxValue) return maxValue;
	return x;
}

int Pane::absoluteX(int x)
{
	switch (m_origin)
	{
	case Origin::topLeft: return m_position.left + x;
	case Origin::bottomRight: return m_position.right - x - g_borderWidth;
	}

	return 0;
}

int Pane::absoluteY(int y)
{
	switch (m_origin)
	{
	case Origin::topLeft: return m_position.top + y;
	case Origin::bottomRight: return m_position.bottom - y - g_borderWidth;
	}

	return 0;
}

int Pane::relativeX(int x)
{
	switch (m_origin)
	{
	case Origin::topLeft: return x - m_position.left;
	case Origin::bottomRight: return m_position.right - x + g_borderWidth;
	}

	return 0;
}

int Pane::relativeY(int y)
{
	switch (m_origin)
	{
	case Origin::topLeft: return y - m_position.top;
	case Origin::bottomRight: return m_position.bottom - y + g_borderWidth;
	}

	return 0;
}

// ドッキング領域を返す。
RECT Pane::getDockRect()
{
	// タブの数が 2 個以上なら
	if (m_tab.getTabCount() >= 2)
	{
		// タブを考慮に入れてドッキング領域を返す。
		switch (g_tabMode)
		{
		case TabMode::title: // タブをタイトルに配置する場合は
			{
				return RECT
				{
					m_position.left,
					m_position.top + std::max<int>(g_captionHeight, g_tabHeight),
					m_position.right,
					m_position.bottom,
				};
			}
		case TabMode::top: // タブを上に配置する場合は
			{
				return RECT
				{
					m_position.left,
					m_position.top + g_captionHeight + g_tabHeight,
					m_position.right,
					m_position.bottom,
				};
			}
		case TabMode::bottom: // タブを下に配置する場合は
			{
				return RECT
				{
					m_position.left,
					m_position.top + g_captionHeight,
					m_position.right,
					m_position.bottom - g_tabHeight,
				};
			}
		}
	}

	// タブを考慮に入れずにドッキング領域を返す。
	return RECT
	{
		m_position.left,
		m_position.top + g_captionHeight,
		m_position.right,
		m_position.bottom,
	};
}

RECT Pane::getCaptionRect()
{
	return RECT
	{
		m_position.left,
		m_position.top,
		m_position.right,
		m_position.top + g_captionHeight,
	};
}

RECT Pane::getMenuRect()
{
	return RECT
	{
		m_position.left,
		m_position.top,
		m_position.left + g_captionHeight,
		m_position.top + g_captionHeight,
	};
}

void Pane::normalize()
{
//	MY_TRACE(_T("Pane::normalize()\n"));

	switch (m_splitMode)
	{
	case SplitMode::vert: m_border = clamp(m_border, 0, getWidth(m_position) - g_borderWidth); break;
	case SplitMode::horz: m_border = clamp(m_border, 0, getHeight(m_position) - g_borderWidth); break;
	}
}

void Pane::recalcLayout()
{
	recalcLayout(&m_position);
}

void Pane::recalcLayout(LPCRECT rc)
{
//	MY_TRACE(_T("Pane::recalcLayout()\n"));

	if (::IsIconic(m_owner))
		return;

	// このペインの位置を更新する。
	m_position = *rc;

	int c = m_tab.getTabCount();

	// タブが 2 個以上あるなら
	if (c >= 2)
	{
		switch (g_tabMode)
		{
		case TabMode::title: // タブをタイトルに表示するなら
			{
				int x = m_position.left + g_captionHeight;
				int y = m_position.top;
				int w = getWidth(m_position) - g_captionHeight;
				int h = std::max<int>(g_captionHeight, g_tabHeight);

				modifyStyle(m_tab.m_hwnd, TCS_BOTTOM, 0);

				// タブコントロールを表示する。
				true_SetWindowPos(m_tab.m_hwnd, 0,
					x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

				break;
			}
		case TabMode::top: // タブを上に表示するなら
			{
				int x = m_position.left;
				int y = m_position.top + g_captionHeight;
				int w = getWidth(m_position);
				int h = g_tabHeight;

				modifyStyle(m_tab.m_hwnd, TCS_BOTTOM, 0);

				// タブコントロールを表示する。
				true_SetWindowPos(m_tab.m_hwnd, 0,
					x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

				break;
			}
		case TabMode::bottom: // タブを下に表示するなら
			{
				int x = m_position.left;
				int y = m_position.bottom - g_tabHeight;
				int w = getWidth(m_position);
				int h = g_tabHeight;

				modifyStyle(m_tab.m_hwnd, 0, TCS_BOTTOM);

				// タブコントロールを表示する。
				true_SetWindowPos(m_tab.m_hwnd, 0,
					x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

				break;
			}
		}
	}
	// タブが 1 個以下なら
	else
	{
		// タブコントロールを非表示にする。
		::ShowWindow(m_tab.m_hwnd, SW_HIDE);
	}

	if (c) // ウィンドウを持っている場合は
	{
		RECT rcDock = getDockRect();

		for (int i = 0; i < c; i++)
		{
			Shuttle* shuttle = m_tab.getShuttle(i);

			shuttle->resizeDockContainer(&rcDock); // ウィンドウをリサイズする。
		}

		return; // ウィンドウを持つペインは子ペインを持たないのでここで終了する。
	}

	// ボーダーが飛び出ないように調整する。
	normalize();

	switch (m_splitMode)
	{
	case SplitMode::vert:
		{
			int border = absoluteX(m_border);

			if (m_children[0])
			{
				RECT rc = { m_position.left, m_position.top, border, m_position.bottom };

				m_children[0]->recalcLayout(&rc);
			}

			if (m_children[1])
			{
				RECT rc = { border + g_borderWidth, m_position.top, m_position.right, m_position.bottom };

				m_children[1]->recalcLayout(&rc);
			}

			break;
		}
	case SplitMode::horz:
		{
			int border = absoluteY(m_border);

			if (m_children[0])
			{
				RECT rc = { m_position.left, m_position.top, m_position.right, border };

				m_children[0]->recalcLayout(&rc);
			}

			if (m_children[1])
			{
				RECT rc = { m_position.left, border + g_borderWidth, m_position.right, m_position.bottom };

				m_children[1]->recalcLayout(&rc);
			}

			break;
		}
	}
}

BOOL Pane::hitTestCaption(POINT point)
{
	RECT rc = getCaptionRect();

	return ::PtInRect(&rc, point);
}

PanePtr Pane::hitTestPane(POINT point)
{
	// point がこのペインの範囲外なら
	if (!::PtInRect(&m_position, point))
		return 0; // ヒットしない。

	// このペインがウィンドウを持っているなら
	if (m_tab.getTabCount())
		return shared_from_this(); // ヒットする。

	switch (m_splitMode)
	{
	case SplitMode::vert:
	case SplitMode::horz:
		{
			// 子ペインの関数を再帰呼び出しする。
			for (auto& child : m_children)
			{
				if (!child) continue;

				PanePtr retValue = child->hitTestPane(point);
				if (retValue) return retValue;
			}

			break;
		}
	}

	return shared_from_this();
}

PanePtr Pane::hitTestBorder(POINT point)
{
	// point がこのペインの範囲外なら
	if (!::PtInRect(&m_position, point))
		return 0; // ヒットしない。

	// このペインがウィンドウを持っているなら
	if (m_tab.getTabCount())
		return 0; // ヒットしない。

	// このペインのボーダーがロックされていないなら
	if (!m_isBorderLocked || ::GetKeyState(VK_CONTROL) < 0)
	{
		switch (m_splitMode)
		{
		case SplitMode::vert:
			{
				int border = absoluteX(m_border);

				// point がボーダーの範囲内なら
				if (point.x >= border && point.x < border + g_borderWidth)
					return shared_from_this(); // ヒットする。

				break;
			}
		case SplitMode::horz:
			{
				int border = absoluteY(m_border);

				// point がボーダーの範囲内なら
				if (point.y >= border && point.y < border + g_borderWidth)
					return shared_from_this(); // ヒットする。

				break;
			}
		default:
			{
				return 0;
			}
		}
	}

	// 子ペインの関数を再帰呼び出しする。
	for (auto& child : m_children)
	{
		if (!child) continue;

		PanePtr retValue = child->hitTestBorder(point);
		if (retValue) return retValue;
	}

	return 0;
}

int Pane::getDragOffset(POINT point)
{
	switch (m_splitMode)
	{
	case SplitMode::vert: return m_border - relativeX(point.x);
	case SplitMode::horz: return m_border - relativeY(point.y);
	}

	return 0;
}

void Pane::dragBorder(POINT point)
{
	switch (m_splitMode)
	{
	case SplitMode::vert: m_border = relativeX(point.x) + m_dragOffset; break;
	case SplitMode::horz: m_border = relativeY(point.y) + m_dragOffset; break;
	}
}

BOOL Pane::getBorderRect(LPRECT rc)
{
	// ウィンドウを持っている場合は
	if (m_tab.getTabCount())
		return FALSE; // ボーダーを持たない。

	switch (m_splitMode)
	{
	case SplitMode::vert:
		{
			int border = absoluteX(m_border);

			rc->left = border;
			rc->top = m_position.top;
			rc->right = border + g_borderWidth;
			rc->bottom = m_position.bottom;

			return TRUE;
		}
	case SplitMode::horz:
		{
			int border = absoluteY(m_border);

			rc->left = m_position.left;
			rc->top = border;
			rc->right = m_position.right;
			rc->bottom = border + g_borderWidth;

			return TRUE;
		}
	}

	return FALSE;
}

void Pane::drawBorder(HDC dc, HBRUSH brush)
{
	// ウィンドウを持っている場合は
	if (m_tab.getTabCount())
		return; // ボーダーを持たない。

	switch (m_splitMode)
	{
	case SplitMode::vert:
	case SplitMode::horz:
		{
			RECT rc;
			if (getBorderRect(&rc))
			{
				// テーマを使用するなら
				if (g_useTheme)
				{
					int partId = WP_BORDER;
					int stateId = CS_INACTIVE;

					// テーマ API を使用してボーダーを描画する。
					::DrawThemeBackground(g_theme, dc, partId, stateId, &rc, 0);
				}
				// テーマを使用しないなら
				else
				{
					// ブラシで塗り潰す。
					::FillRect(dc, &rc, brush);
				}
			}

			for (auto& child : m_children)
			{
				if (!child) continue;

				child->drawBorder(dc, brush);
			}

			break;
		}
	}
}

void Pane::drawCaption(HDC dc)
{
	Shuttle* shuttle = getActiveShuttle();

	if (shuttle)
	{
		RECT rc = getCaptionRect();

		if (rc.bottom > m_position.bottom)
			return; // キャプションがはみ出てしまう場合は何もしない。

		RECT rcMenu = getMenuRect();
		BOOL hasMenu = !!::GetMenu(shuttle->m_hwnd);

		// ウィンドウテキストを取得する。
		WCHAR text[MAX_PATH] = {};
		::GetWindowTextW(shuttle->m_hwnd, text, MAX_PATH);

		// テーマを使用するなら
		if (g_useTheme)
		{
			// ウィンドウの状態から stateId を取得する。
			int stateId = CS_ACTIVE;
			if (::GetFocus() != shuttle->m_hwnd) stateId = CS_INACTIVE;
			if (!::IsWindowEnabled(shuttle->m_hwnd)) stateId = CS_DISABLED;

			// テーマ API を使用してタイトルを描画する。
			::DrawThemeBackground(g_theme, dc, WP_CAPTION, stateId, &rc, 0);

			{
				RECT rcText = rc;

				if (hasMenu)
				{
					rcText.left = rcMenu.right;

					::DrawThemeText(g_theme, dc, WP_CAPTION, stateId,
						L"M", -1, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rcMenu);
				}

				if (rcText.left < rcText.right)
					::DrawThemeText(g_theme, dc, WP_CAPTION, stateId,
						text, ::lstrlenW(text), DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rcText);
			}
		}
		// テーマを使用しないなら
		else
		{
			// 直接 GDI を使用して描画する。

			COLORREF captionColor = g_activeCaptionColor;
			COLORREF captionTextColor = g_activeCaptionTextColor;

			if (::GetFocus() != shuttle->m_hwnd)
			{
				captionColor = g_inactiveCaptionColor;
				captionTextColor = g_inactiveCaptionTextColor;
			}

			HBRUSH brush = ::CreateSolidBrush(captionColor);
			::FillRect(dc, &rc, brush);
			::DeleteObject(brush);

			int bkMode = ::SetBkMode(dc, TRANSPARENT);
			COLORREF textColor = ::SetTextColor(dc, captionTextColor);
			{
				RECT rcText = rc;

				if (hasMenu)
				{
					rcText.left = rcMenu.right;

					::DrawTextW(dc, L"M", -1, &rcMenu, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}

				if (rcText.left < rcText.right)
					::DrawTextW(dc, text, ::lstrlenW(text), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			}
			::SetTextColor(dc, textColor);
			::SetBkMode(dc, bkMode);
		}

		return;
	}

	switch (m_splitMode)
	{
	case SplitMode::vert:
	case SplitMode::horz:
		{
			for (auto& child : m_children)
			{
				if (!child) continue;

				child->drawCaption(dc);
			}

			break;
		}
	}
}

void Pane::changeCurrent()
{
	m_tab.changeCurrent();

	for (auto& child : m_children)
	{
		if (!child) continue;

		child->changeCurrent();
	}
}

//---------------------------------------------------------------------
