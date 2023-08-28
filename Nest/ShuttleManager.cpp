#include "pch.h"
#include "Nest.h"
#include "RenameColonyDialog.h"

//---------------------------------------------------------------------

ShuttlePtr ShuttleManager::getShuttle(const _bstr_t& name)
{
	auto it = m_map.find(name);
	if (it == m_map.end()) return 0;
	return it->second;
}

void ShuttleManager::addShuttle(ShuttlePtr shuttle, const _bstr_t& name, HWND hwnd)
{
	// 指定された名前のシャトルがすでに存在している場合は
	if (getShuttle(name))
	{
		// メッセージボックスを出す。
		TCHAR text[MAX_PATH] = {};
		::StringCbPrintf(text, sizeof(text), _T("ウィンドウ名が重複しています\nウィンドウ名 : %ws"), (BSTR)name);
		::MessageBox(g_hub, text, _T("Nest"), MB_OK | MB_ICONWARNING);

		return;
	}

	m_map[name] = shuttle;
	shuttle->m_name = name;
	shuttle->init(hwnd);
}

void ShuttleManager::removeShuttle(Shuttle* shuttle)
{
	// ドッキングしているなら
	if (shuttle->m_pane)
	{
		// ペインから切り離す。
		shuttle->m_pane->removeShuttle(shuttle);
	}

	// マップからも削除する。
	for (auto it = m_map.begin(); it != m_map.end(); it++)
	{
		if (it->second.get() == shuttle)
		{
			m_map.erase(it);
			break;
		}
	}

	// 基本的にこの時点で shuttle は無効なポインタになる。
}

void ShuttleManager::renameShuttle(Shuttle* shuttle, const _bstr_t& newName)
{
	assert(m_map.find(newName) == m_map.end());

	// (1) シャトルの名前を変更する。
	// (2) 新しい名前でマップに登録する。
	// (3) 古い名前をマップから削除する。
	// (4) ウィンドウ名を新しい名前に更新する。

	_bstr_t oldName = shuttle->m_name;

	shuttle->m_name = newName; // (1)
	m_map[newName] = m_map[oldName]; // (2)
	m_map.erase(oldName); // (3)
	::SetWindowText(shuttle->m_hwnd, newName); // (4)
}

void ShuttleManager::renameShuttle(HWND owner, Shuttle* shuttle)
{
	RenameColonyDialog dialog;
	dialog.m_shuttleManager = this;
	dialog.m_shuttle = shuttle;
	dialog.doModal(owner);
}

//---------------------------------------------------------------------
