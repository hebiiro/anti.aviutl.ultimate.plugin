#include "pch.h"
#include "afxdialogex.h"
#include "FilerDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(FilerDialog, CDialogEx)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_APPCOMMAND()
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_PREV, &FilerDialog::OnClickedPrev)
	ON_BN_CLICKED(IDC_NEXT, &FilerDialog::OnClickedNext)
	ON_BN_CLICKED(IDC_UP, &FilerDialog::OnClickedUp)
	ON_BN_CLICKED(IDC_GET, &FilerDialog::OnClickedGet)
	ON_COMMAND(ID_ADD_FAVORITE, &FilerDialog::OnAddFavorite)
	ON_COMMAND(ID_DELETE_FAVORITE, &FilerDialog::OnDeleteFavorite)
	ON_COMMAND(ID_SHOW_NAV_PANE, &FilerDialog::OnShowNavPane)
	ON_COMMAND(ID_PLAY_VOICE, &FilerDialog::OnPlayVoice)
	ON_COMMAND(ID_USE_COMMON_DIALOG, &FilerDialog::OnUseCommonDialog)
	ON_UPDATE_COMMAND_UI(ID_SHOW_NAV_PANE, &FilerDialog::OnUpdateShowNavPane)
	ON_UPDATE_COMMAND_UI(ID_PLAY_VOICE, &FilerDialog::OnUpdatePlayVoice)
	ON_UPDATE_COMMAND_UI(ID_USE_COMMON_DIALOG, &FilerDialog::OnUpdateUseCommonDialog)
	ON_CBN_SELCHANGE(IDC_URL, &FilerDialog::OnSelChangeUrl)
END_MESSAGE_MAP()

#if 0
void FilerDialog::loadSettings()
{
	MY_TRACE(_T("FilerDialog::loadSettings()\n"));

	m_url.ResetContent();

	WCHAR filePath[MAX_PATH] = {};
	::GetModuleFileNameW(AfxGetInstanceHandle(), filePath, MAX_PATH);
	::PathRemoveExtensionW(filePath);
	::StringCbCatW(filePath, sizeof(filePath), L"Settings.xml");
	MY_TRACE_WSTR(filePath);

	try
	{
		MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

		if (document->load(filePath) == VARIANT_FALSE)
		{
			MY_TRACE(_T("%ws file loading failed\n"), filePath);

			return;
		}

		MSXML2::IXMLDOMElementPtr element = document->documentElement;

		getPrivateProfileBool(element, L"isNavPaneVisible", m_isNavPaneVisible);
		getPrivateProfileBool(element, L"isVoiceEnabled", m_isVoiceEnabled);
		getPrivateProfileBool(element, L"usesCommonDialog", m_usesCommonDialog);

		::SetProp(GetSafeHwnd(), _T("usesCommonDialog"), (HANDLE)m_usesCommonDialog);

		_bstr_t path;
		getPrivateProfileString(element, L"path", path);
		MY_TRACE_WSTR((BSTR)path);
		if ((BSTR)path)
			m_url.SetWindowText(path);

		{
			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"favorite");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t path;
				getPrivateProfileString(element, L"path", path);
				MY_TRACE_WSTR((BSTR)path);
				if ((BSTR)path)
				{
					m_url.AddString(path);
				}
			}
		}

		m_isSettingsLoaded = TRUE;
	}
	catch (_com_error& e)
	{
		MY_TRACE(_T("%s\n"), e.ErrorMessage());
	}
}

void FilerDialog::saveSettings()
{
	MY_TRACE(_T("FilerDialog::saveSettings()\n"));

	if (!m_isSettingsLoaded)
	{
		MY_TRACE(_T("初期化に失敗しているので保存処理をスキップします\n"));

		return;
	}

	WCHAR filePath[MAX_PATH] = {};
	::GetModuleFileNameW(AfxGetInstanceHandle(), filePath, MAX_PATH);
	::PathRemoveExtensionW(filePath);
	::StringCbCatW(filePath, sizeof(filePath), L"Settings.xml");
	MY_TRACE_WSTR(filePath);

	try
	{
		MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

		MSXML2::IXMLDOMElementPtr parentElement = appendElement(document, document, L"ObjectExplorerSettings");

		setPrivateProfileString(parentElement, L"path", (LPCTSTR)m_currentFolderPath);
		setPrivateProfileBool(parentElement, L"isNavPaneVisible", m_isNavPaneVisible);
		setPrivateProfileBool(parentElement, L"isVoiceEnabled", m_isVoiceEnabled);
		setPrivateProfileBool(parentElement, L"usesCommonDialog", m_usesCommonDialog);

		int c = m_url.GetCount();
		for (int i = 0; i < c; i++)
		{
			MSXML2::IXMLDOMElementPtr element = appendElement(document, parentElement, L"favorite");

			CString path; m_url.GetLBText(i, path);
			setPrivateProfileString(element, L"path", (LPCTSTR)path);
		}

		saveXMLDocument(document, filePath);
	}
	catch (_com_error& e)
	{
		MY_TRACE(_T("%s\n"), e.ErrorMessage());
	}
}
#endif
void FilerDialog::createExplorer()
{
	MY_TRACE(_T("FilerDialog::createExplorer()\n"));

	// エクスプローラを作成する。
	HRESULT hr = m_explorer.CreateInstance(CLSID_ExplorerBrowser);

	{
		// エクスプローラを初期化する。

		CWnd* placeHolder = GetDlgItem(IDC_PLACE_HOLDER);
		CRect rc; placeHolder->GetWindowRect(&rc);
		ScreenToClient(&rc);

		FOLDERSETTINGS fs = {};
		fs.ViewMode = FVM_AUTO;
		fs.fFlags = 0;//FWF_NOBROWSERVIEWSTATE;

		hr = m_explorer->Initialize(GetSafeHwnd(), &rc, &fs);
		hr = m_explorer->SetPropertyBag(L"AviUtl.ObjectExplorer");
	}

	// ハンドラを追加する。
	hr = ::IUnknown_SetSite(m_explorer, static_cast<IServiceProvider*>(this));
	hr = m_explorer->Advise(static_cast<IExplorerBrowserEvents*>(this), &m_cookie);
	IFolderFilterSitePtr folderFilterSite = m_explorer;
	MY_TRACE_HEX(folderFilterSite.GetInterfacePtr());
	hr = folderFilterSite->SetFilter(static_cast<IFolderFilter*>(this));
	MY_TRACE_COM_ERROR(hr);

	if (m_isNavPaneVisible)
	{
		// フレームを表示する。
		hr = m_explorer->SetOptions(EBO_SHOWFRAMES);
	}

	CString url; m_url.GetWindowText(url);
	MY_TRACE_TSTR((LPCTSTR)url);

	{
		// 一旦デスクトップを表示する。
		// こうしないと次の browseToPath(url) でフリーズする。

		PIDLIST_ABSOLUTE pidlDesktop;
		::SHGetKnownFolderIDList(FOLDERID_Desktop, 0, NULL, &pidlDesktop);
		m_explorer->BrowseToIDList(pidlDesktop, SBSP_ABSOLUTE);
		::ILFree(pidlDesktop);
	}

	MY_TRACE(_T("URL に移動します\n"));

	if (!url.IsEmpty())
		browseToPath(url);

	MY_TRACE(_T("フィルタウィンドウにブラウザのウィンドウハンドルをセットします\n"));

	IShellBrowserPtr browser = m_explorer;
	MY_TRACE_HEX(browser.GetInterfacePtr());
	HWND hwnd = 0; browser->GetWindow(&hwnd);
	MY_TRACE_HEX(hwnd);
	Share::Filer::FilerWindow::setBrowser(filerWindow, hwnd);

	MY_TRACE(_T("FilerDialog::createExplorer() end\n"));
}

void FilerDialog::destroyExplorer()
{
	MY_TRACE(_T("FilerDialog::destroyExplorer()\n"));

	m_explorer->Unadvise(m_cookie);
	::IUnknown_SetSite(m_explorer, 0);
	m_explorer->Destroy();
	m_explorer = 0;
	m_shellView = 0;

	MY_TRACE(_T("FilerDialog::destroyExplorer() end\n"));
}

void FilerDialog::browseToPath(LPCTSTR path)
{
	MY_TRACE(_T("FilerDialog::browseToPath(%s)\n"), path);

	if (::lstrcmpi(path, m_currentFolderPath) == 0)
	{
		// 現在のフォルダを再読み込みする。

		if (m_shellView)
		{
			HRESULT hr = m_shellView->Refresh();
			MY_TRACE_COM_ERROR(hr);
		}
	}
	else
	{
		// 指定されたフォルダを表示する。

		PIDLIST_ABSOLUTE pidl = ::ILCreateFromPath(path);
		if (pidl)
		{
			m_explorer->BrowseToIDList(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
			::ILFree(pidl);
		}
	}
}

void FilerDialog::playVoice(LPCTSTR voice)
{
	MY_TRACE(_T("FilerDialog::playVoice(%s)\n"), voice);
#if 0
	if (!m_isVoiceEnabled)
		return;

	// WavPlayer が存在するかチェックする。
	if (::GetFileAttributes(theApp.m_wavPlayerFileName) == INVALID_FILE_ATTRIBUTES)
		return;

	// wav ファイルのパスを取得する。
	TCHAR wavFileName[MAX_PATH] = {};
	::GetModuleFileName(AfxGetInstanceHandle(), wavFileName, MAX_PATH);
	::PathRemoveFileSpec(wavFileName);
	::PathAppend(wavFileName, voice);
	MY_TRACE_TSTR(wavFileName);

	// wav ファイルが存在するかチェックする。
	if (::GetFileAttributes(wavFileName) == INVALID_FILE_ATTRIBUTES)
		return;

	// wav ファイルを再生する。
	SHELLEXECUTEINFO sei = { sizeof(sei) };
	sei.lpFile = theApp.m_wavPlayerFileName;
	sei.lpParameters = wavFileName;
	BOOL result = ::ShellExecuteEx(&sei);
	MY_TRACE_HEX(result);
#endif
}

void FilerDialog::DoDataExchange(CDataExchange* pDX)
{
	MY_TRACE(_T("FilerDialog::DoDataExchange()\n"));

	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_URL, m_url);
	DDX_Control(pDX, IDC_SEARCH, m_search);
}

BOOL FilerDialog::PreTranslateMessage(MSG* pMsg)
{
	if (m_shellView && m_shellView->TranslateAccelerator(pMsg) == S_OK)
		return TRUE;

	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case _T('F'):
			{
				if (::GetKeyState(VK_CONTROL) < 0)
				{
					m_search.SetFocus();
					return TRUE;
				}

				break;
			}
		case _T('G'):
			{
				if (::GetKeyState(VK_CONTROL) < 0)
				{
					m_url.SetFocus();
					return TRUE;
				}

				break;
			}
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

BOOL FilerDialog::OnInitDialog()
{
	MY_TRACE(_T("FilerDialog::OnInitDialog()\n"));

	CDialogEx::OnInitDialog();

	// エクスプローラを作成します。
	createExplorer();

	// フィルタダイアログの作成が完了したことをフィルタウィンドウに通知します。
	// これにより、フィルタウィンドウがフィルタダイアログを参照することができるようになります。
	fire_FilerDialogCreated();

	return TRUE;
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
	destroyExplorer();

	CDialogEx::OnDestroy();
}

void FilerDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	CWnd* placeHolder = GetDlgItem(IDC_PLACE_HOLDER);
	if (placeHolder)
	{
		CRect rc; placeHolder->GetWindowRect(&rc);
		ScreenToClient(&rc);
		m_explorer->SetRect(0, rc);
	}
}

void FilerDialog::OnAppCommand(CWnd* pWnd, UINT nCmd, UINT nDevice, UINT nKey)
{
	switch (nCmd)
	{
	case APPCOMMAND_BROWSER_BACKWARD:
		{
			MY_TRACE(_T("APPCOMMAND_BROWSER_BACKWARD\n"));

			OnClickedPrev();
			break;
		}
	case APPCOMMAND_BROWSER_FORWARD:
		{
			MY_TRACE(_T("APPCOMMAND_BROWSER_FORWARD\n"));

			OnClickedNext();
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

void FilerDialog::OnContextMenu(CWnd* pWnd, CPoint point)
{
	MY_TRACE(_T("FilerDialog::OnContextMenu()\n"));

	CMenu menu; menu.LoadMenu(IDR_POPUP_MENU);
	CMenu* subMenu = menu.GetSubMenu(0);
	subMenu->CheckMenuItem(ID_SHOW_NAV_PANE, m_isNavPaneVisible ? MF_CHECKED : MF_UNCHECKED);
//	subMenu->CheckMenuItem(ID_PLAY_VOICE, m_isVoiceEnabled ? MF_CHECKED : MF_UNCHECKED);
//	subMenu->CheckMenuItem(ID_USE_COMMON_DIALOG, m_usesCommonDialog ? MF_CHECKED : MF_UNCHECKED);
	subMenu->TrackPopupMenu(0, point.x, point.y, this);
}

void FilerDialog::OnOK()
{
	MY_TRACE(_T("FilerDialog::OnOK()\n"));

	CString path; m_url.GetWindowText(path);
	MY_TRACE_TSTR(path);

	browseToPath(path);

//	__super::OnOK();
}

void FilerDialog::OnCancel()
{
	MY_TRACE(_T("FilerDialog::OnCancel()\n"));

//	ShowWindow(SW_HIDE);

//	__super::OnCancel();
}

void FilerDialog::OnClickedPrev()
{
	MY_TRACE(_T("FilerDialog::OnClickedPrev()\n"));

	m_explorer->BrowseToIDList(NULL, SBSP_NAVIGATEBACK);

	playVoice(_T("Prev.wav"));
}

void FilerDialog::OnClickedNext()
{
	MY_TRACE(_T("FilerDialog::OnClickedNext()\n"));

	m_explorer->BrowseToIDList(NULL, SBSP_NAVIGATEFORWARD);

	playVoice(_T("Next.wav"));
}

void FilerDialog::OnClickedUp()
{
	MY_TRACE(_T("FilerDialog::OnClickedUp()\n"));

	m_explorer->BrowseToIDList(NULL, SBSP_PARENT);

	playVoice(_T("Up.wav"));
}

void FilerDialog::OnClickedGet()
{
	MY_TRACE(_T("FilerDialog::OnClickedGet()\n"));

	fire_GetUrl();

	playVoice(_T("Get.wav"));
}

void FilerDialog::OnAddFavorite()
{
	MY_TRACE(_T("FilerDialog::OnAddFavorite()\n"));

	int index = m_url.FindString(0, m_currentFolderPath);
	if (index == CB_ERR)
	{
		m_url.AddString(m_currentFolderPath);

		playVoice(_T("AddFavorite.wav"));
	}
}

void FilerDialog::OnDeleteFavorite()
{
	MY_TRACE(_T("FilerDialog::OnDeleteFavorite()\n"));

	int index = m_url.FindString(0, m_currentFolderPath);
	if (index != CB_ERR)
	{
		m_url.DeleteString(index);

		playVoice(_T("DeleteFavorite.wav"));
	}
}

void FilerDialog::OnShowNavPane()
{
	MY_TRACE(_T("FilerDialog::OnShowNavPane()\n"));

	if (m_isNavPaneVisible)
	{
		m_isNavPaneVisible = FALSE;
		destroyExplorer();
		createExplorer();
	}
	else
	{
		m_isNavPaneVisible = TRUE;
		destroyExplorer();
		createExplorer();
	}
}

void FilerDialog::OnPlayVoice()
{
	MY_TRACE(_T("FilerDialog::OnPlayVoice()\n"));
#if 0
	if (m_isVoiceEnabled)
	{
		m_isVoiceEnabled = FALSE;
	}
	else
	{
		m_isVoiceEnabled = TRUE;
	}
#endif
}

void FilerDialog::OnUseCommonDialog()
{
	MY_TRACE(_T("FilerDialog::OnUseCommonDialog()\n"));
#if 0
	if (m_usesCommonDialog)
	{
		m_usesCommonDialog = FALSE;
	}
	else
	{
		m_usesCommonDialog = TRUE;
	}

	::SetProp(GetSafeHwnd(), _T("usesCommonDialog"), (HANDLE)m_usesCommonDialog);
#endif
}

void FilerDialog::OnUpdateShowNavPane(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_isNavPaneVisible);
}

void FilerDialog::OnUpdatePlayVoice(CCmdUI* pCmdUI)
{
//	pCmdUI->SetCheck(m_isVoiceEnabled);
}

void FilerDialog::OnUpdateUseCommonDialog(CCmdUI* pCmdUI)
{
//	pCmdUI->SetCheck(m_usesCommonDialog);
}

void FilerDialog::OnSelChangeUrl()
{
	MY_TRACE(_T("FilerDialog::OnSelChangeUrl()\n"));

	int index = m_url.GetCurSel();
	MY_TRACE_INT(index);
	if (index != CB_ERR)
	{
		CString path; m_url.GetLBText(index, path);
		MY_TRACE_TSTR((LPCTSTR)path);

		browseToPath(path);
	}
}
