#include "pch.h"
#include "RenameColonyDialog.h"

//---------------------------------------------------------------------

int RenameColonyDialog::doModal(HWND parent)
{
	create(g_instance, MAKEINTRESOURCE(IDD_RENAME_COLONY), parent);

	::SetDlgItemText(*this, IDC_NEW_NAME, m_shuttle->m_name);

	int retValue = Dialog::doModal();

	if (IDOK != retValue)
		return retValue;

	TCHAR newName[MAX_PATH] = {};
	::GetDlgItemText(*this, IDC_NEW_NAME, newName, MAX_PATH);

	// 古い名前と新しい名前が違うなら
	if (::StrCmp(m_shuttle->m_name, newName) != 0)
	{
		// シャトルの名前を変更する。
		m_shuttleManager->renameShuttle(m_shuttle, newName);
	}

	return retValue;
}

void RenameColonyDialog::onOK()
{
	TCHAR newName[MAX_PATH] = {};
	::GetDlgItemText(*this, IDC_NEW_NAME, newName, MAX_PATH);

	// 新しい名前が現在のシャトルの名前を違い、
	// なおかつ新しい名前のシャトルがすでに存在する場合は
	if (::StrCmp(m_shuttle->m_name, newName) != 0 &&
		g_shuttleManager.getShuttle(newName))
	{
		// メッセージボックスを出す。
		::MessageBox(*this, _T("名前が重複しています"), _T("Nest"), MB_OK | MB_ICONWARNING);

		return;
	}

	Dialog::onOK();
}

void RenameColonyDialog::onCancel()
{
	Dialog::onCancel();
}

//---------------------------------------------------------------------
