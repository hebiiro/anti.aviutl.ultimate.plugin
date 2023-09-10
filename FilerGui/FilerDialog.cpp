#include "pch.h"
#include "afxdialogex.h"
#include "FilerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(FilerDialog, CDialogEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_APPCOMMAND()
	ON_COMMAND(IDC_PREV_FOLDER, &FilerDialog::OnPrevFolder)
	ON_COMMAND(IDC_NEXT_FOLDER, &FilerDialog::OnNextFolder)
	ON_COMMAND(IDC_PARENT_FOLDER, &FilerDialog::OnParentFolder)
	ON_COMMAND(IDC_GET_FILE_NAME, &FilerDialog::OnGetFileName)
	ON_COMMAND(IDC_ADD_BOOKMARK, &FilerDialog::OnAddBookmark)
	ON_COMMAND(IDC_REMOVE_BOOKMARK, &FilerDialog::OnRemoveBookmark)
	ON_COMMAND(IDC_HAS_NAV_PANE, &FilerDialog::OnHasNavPane)
	ON_CBN_SELCHANGE(IDC_FOLDER, &FilerDialog::OnSelChangeFolder)
	ON_MESSAGE(Share::Filer::Message::PostExitFilerWindow, OnPostExitFilerWindow)
END_MESSAGE_MAP()

void FilerDialog::DoDataExchange(CDataExchange* pDX)
{
	MY_TRACE(_T("FilerDialog::DoDataExchange()\n"));

	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FOLDER, folder);
	DDX_Control(pDX, IDC_SEARCH, search);
	DDX_Control(pDX, IDC_HAS_NAV_PANE, hasNavPane);
}

BOOL FilerDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case _T('F'):
			{
				if (::GetKeyState(VK_CONTROL) < 0)
				{
					search.SetFocus();
					return TRUE;
				}

				break;
			}
		case _T('G'):
			{
				if (::GetKeyState(VK_CONTROL) < 0)
				{
					folder.SetFocus();
					return TRUE;
				}

				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void FilerDialog::PostNcDestroy()
{
	MY_TRACE(_T("FilerDialog::PostNcDestroy()\n"));

	CDialogEx::PostNcDestroy();

	// thisをコレクションから取り除きます。この処理は実質的にdelete thisと同じです。
	std::erase_if(collection, [this](const auto& x){ return x.get() == this; });
}

BOOL FilerDialog::OnInitDialog()
{
	MY_TRACE(_T("FilerDialog::OnInitDialog()\n"));

	CDialogEx::OnInitDialog();

	tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	tooltip.SetMaxTipWidth(INT_MAX);
	addTool(IDC_PREV_FOLDER, _T("前のフォルダに戻ります"));
	addTool(IDC_NEXT_FOLDER, _T("次のフォルダに進みます"));
	addTool(IDC_PARENT_FOLDER, _T("一つ上の階層へ移動します"));
	addTool(IDC_GET_FILE_NAME, _T("AviUtlのカレントアイテムからフォルダを取得します"));
	addTool(IDC_ADD_BOOKMARK, _T("現在のフォルダをブックマークに追加します"));
	addTool(IDC_REMOVE_BOOKMARK, _T("現在のフォルダをブックマークから削除します"));
	addTool(IDC_HAS_NAV_PANE, _T("ツリービューの表示/非表示を切り替えます"));
	addTool(IDC_SEARCH, _T("ファイルをフィルタリングするための文字列です"));
	addTool(IDC_FOLDER, _T("現在表示中のフォルダです")
		_T("\n手動で変更した場合はエンターキーを押して確定してください")
		_T("\nリストダウンするとブックマークから選択できます"));

	return TRUE;
}

BOOL FilerDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	auto nm = (NMHDR*)lParam;
	if (nm->hwndFrom == tooltip.GetSafeHwnd())
	{
		switch (nm->code)
		{
		case TTN_SHOW:
			{
				MY_TRACE(_T("FilerDialog::OnNotify(TTN_SHOW)\n"));

				CRect rc; tooltip.GetWindowRect(&rc);
				int x = rc.left;
				int y = rc.top;
				int w = rc.Width();
				int h = rc.Height();

				{
					// ツールチップをダイアログの左上に表示します。
					CRect rc; GetClientRect(&rc);
					ClientToScreen(&rc);
					x = rc.left;
					y = rc.top - h;
				}

				tooltip.SetWindowPos(0, x, y, w, h, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

				*pResult = TRUE;
				return TRUE;
			}
		}
	}

	return __super::OnNotify(wParam, lParam, pResult);
}

int FilerDialog::OnCreate(LPCREATESTRUCT cs)
{
	MY_TRACE(_T("FilerDialog::OnCreate()\n"));

	if (CDialogEx::OnCreate(cs) == -1)
		return -1;

	return 0;
}

void FilerDialog::OnDestroy()
{
	MY_TRACE(_T("FilerDialog::OnDestroy()\n"));

	// エクスプローラを削除します。
	exitExplorer();

	CDialogEx::OnDestroy();
}

void FilerDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// エクスプローラをリサイズします。
	resizeExplorer();
}

void FilerDialog::OnAppCommand(CWnd* pWnd, UINT nCmd, UINT nDevice, UINT nKey)
{
	switch (nCmd)
	{
	case APPCOMMAND_BROWSER_BACKWARD:
		{
			MY_TRACE(_T("APPCOMMAND_BROWSER_BACKWARD\n"));

			OnPrevFolder();
			break;
		}
	case APPCOMMAND_BROWSER_FORWARD:
		{
			MY_TRACE(_T("APPCOMMAND_BROWSER_FORWARD\n"));

			OnNextFolder();
			break;
		}
	case APPCOMMAND_BROWSER_REFRESH:
		{
			MY_TRACE(_T("APPCOMMAND_BROWSER_REFRESH\n"));

			break;
		}
	}

	__super::OnAppCommand(pWnd, nCmd, nDevice, nKey);
}

void FilerDialog::OnOK()
{
	MY_TRACE(_T("FilerDialog::OnOK()\n"));

	CString path; folder.GetWindowText(path);
	MY_TRACE_TSTR(path);

	browseToPath(path);

//	__super::OnOK();
}

void FilerDialog::OnCancel()
{
	MY_TRACE(_T("FilerDialog::OnCancel()\n"));

//	__super::OnCancel();
}

void FilerDialog::OnPrevFolder()
{
	MY_TRACE(_T("FilerDialog::OnPrevFolder()\n"));

	if (!explorer) return;

	explorer->BrowseToIDList(NULL, SBSP_NAVIGATEBACK);
}

void FilerDialog::OnNextFolder()
{
	MY_TRACE(_T("FilerDialog::OnNextFolder()\n"));

	if (!explorer) return;

	explorer->BrowseToIDList(NULL, SBSP_NAVIGATEFORWARD);
}

void FilerDialog::OnParentFolder()
{
	MY_TRACE(_T("FilerDialog::OnParentFolder()\n"));

	if (!explorer) return;

	explorer->BrowseToIDList(NULL, SBSP_PARENT);
}

void FilerDialog::OnGetFileName()
{
	MY_TRACE(_T("FilerDialog::OnGetFileName()\n"));

	if (!explorer) return;

	fire_GetFileName();
}

void FilerDialog::OnAddBookmark()
{
	MY_TRACE(_T("FilerDialog::OnAddBookmark()\n"));

	CString path; folder.GetWindowText(path);
	MY_TRACE_TSTR((LPCTSTR)path);
	int index = folder.FindString(0, path);
	if (index == CB_ERR) folder.AddString(path);
}

void FilerDialog::OnRemoveBookmark()
{
	MY_TRACE(_T("FilerDialog::OnRemoveBookmark()\n"));

	CString path; folder.GetWindowText(path);
	MY_TRACE_TSTR((LPCTSTR)path);
	int index = folder.FindString(0, path);
	if (index != CB_ERR) folder.DeleteString(index);
}

void FilerDialog::OnHasNavPane()
{
	MY_TRACE(_T("FilerDialog::OnHasNavPane()\n"));

	exitExplorer();
	initExplorer();
}

void FilerDialog::OnSelChangeFolder()
{
	MY_TRACE(_T("FilerDialog::OnSelChangeFolder()\n"));

	int index = folder.GetCurSel();
	MY_TRACE_INT(index);
	if (index != CB_ERR)
	{
		CString path; folder.GetLBText(index, path);
		MY_TRACE_TSTR((LPCTSTR)path);

		browseToPath(path);
	}
}

//
// ファイラウィンドウが削除されるときにホストプロセスから通知されます。
// この時点でファイラウィンドウとの親子関係は解除されているので、手動でウィンドウを破壊します。
//
LRESULT FilerDialog::OnPostExitFilerWindow(WPARAM wParam, LPARAM lParam)
{
	MY_TRACE(_T("FilerDialog::OnPostExitFilerWindow(0x%08X, 0x%08X)\n"), wParam, lParam);

	return DestroyWindow();
}
