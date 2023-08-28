#include "pch.h"
#include "Nest.h"

//---------------------------------------------------------------------

PanePtr ColonyManager::getRootPane(HWND colony)
{
	PanePtr* p = (PanePtr*)::GetProp(colony, _T("Nest.RootPane"));
	if (p) return *p;
	return 0;
}

BOOL ColonyManager::canDocking(HWND colony, HWND hwnd)
{
	PanePtr root = getRootPane(hwnd);
	if (root)
	{
		// hwnd はコロニーなので、入れ子構造をチェックする。

		while (colony)
		{
			if (hwnd == colony)
			{
				// hwnd が colony の祖先なのでドッキングできない。
				return FALSE;
			}

			colony = ::GetParent(colony);
		}
	}

	return TRUE;
}

// ペインのレイアウトを再帰的に再計算する。
void ColonyManager::calcLayout(HWND colony)
{
	MY_TRACE(_T("calcLayout(0x%08X)\n"), colony);

	RECT rc; ::GetClientRect(colony, &rc);
	PanePtr root = getRootPane(colony);
	root->recalcLayout(&rc);
}

void ColonyManager::calcAllLayout()
{
	calcLayout(g_hub);
	::InvalidateRect(g_hub, 0, FALSE);

	for (auto colony : m_array)
	{
		calcLayout(colony);
		::InvalidateRect(colony, 0, FALSE);
	}
}

void ColonyManager::updateColonyMenu()
{
	// すべての項目を削除する。
	while (::DeleteMenu(g_colonyMenu, 0, MF_BYPOSITION)){}

	int c = (int)m_array.size();
	for (int i = 0; i < c; i++)
	{
		HWND colony = m_array[i];

		MENUITEMINFO mii = { sizeof(mii) };
		mii.fMask = MIIM_STRING | MIIM_DATA | MIIM_ID | MIIM_STATE;

		TCHAR windowName[MAX_PATH] = {};
		::GetWindowText(colony, windowName, MAX_PATH);

		if (::IsWindowVisible(colony))
			mii.fState = MFS_CHECKED;
		else
			mii.fState = MFS_UNCHECKED;

		mii.wID = CommandID::COLONY_BEGIN + i;
		mii.dwItemData = (DWORD)colony;
		mii.dwTypeData = windowName;

		::InsertMenuItem(g_colonyMenu, i, TRUE, &mii);
	}
}

void ColonyManager::executeColonyMenu(UINT id)
{
	MENUITEMINFO mii = { sizeof(mii) };
	mii.fMask = MIIM_DATA;
	::GetMenuItemInfo(g_colonyMenu, id, FALSE, &mii);

	HWND colony = (HWND)mii.dwItemData;
	if (colony)
		::SendMessage(colony, WM_CLOSE, 0, 0);
}

//---------------------------------------------------------------------

void ColonyManager::insert(HWND colony)
{
	// ルートペインを作成する。
	::SetProp(colony, _T("Nest.RootPane"), (HANDLE)new PanePtr(new Pane(colony)));

	// 最初に作成されるコロニーはハブになるのでコレクションには追加しない。
	if (g_hub)
	{
		// コレクションに追加する。
		m_array.push_back(colony);
	}
}

void ColonyManager::erase(HWND colony)
{
	// コレクションから削除する。
	for (auto it = m_array.begin(); it != m_array.end(); it++)
	{
		if (colony == *it)
		{
			m_array.erase(it);
			break;
		}
	}

	eraseInternal(colony);
}

void ColonyManager::eraseInternal(HWND colony)
{
	// ルートペインを取得する。
	PanePtr* root = (PanePtr*)::RemoveProp(colony, _T("Nest.RootPane"));

	// コロニーがハブではないなら
	if (root && colony != g_hub)
	{
		// ドッキング中のウィンドウが削除されないようにルートペインをリセットする。
		root->get()->resetPane();

		// ルートペインを削除する。
		delete root;
	}
}

void ColonyManager::clear()
{
	// 全てのコロニーを削除する。
	for (auto colony : m_array)
	{
		eraseInternal(colony);
		::DestroyWindow(colony);
	}

	// コレクションをクリアする。
	m_array.clear();
}

//---------------------------------------------------------------------
