#pragma once
#include "resource.h"
#include "Hive.h"

class FilerDialog
	: public CDialogEx
	, public IServiceProvider
	, public IExplorerBrowserEvents
	, public IExplorerPaneVisibility
	, public IFolderFilter
{
public:

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** ppv)
	{
		if (::IsEqualIID(iid, IID_IUnknown))
		{
			*ppv = static_cast<IUnknown*>(static_cast<IServiceProvider*>(this));
		}
		else if (::IsEqualIID(iid, IID_IServiceProvider))
		{
			*ppv = static_cast<IServiceProvider*>(this);
		}
		else if (::IsEqualIID(iid, IID_IExplorerBrowserEvents))
		{
			*ppv = static_cast<IExplorerBrowserEvents*>(this);
		}
		else if (::IsEqualIID(iid, IID_IExplorerPaneVisibility))
		{
			*ppv = static_cast<IExplorerPaneVisibility*>(this);
		}
		else if (::IsEqualIID(iid, IID_IFolderFilter))
		{
			*ppv = static_cast<IFolderFilter*>(this);
		}
		else
		{
			*ppv = 0;
			return E_NOINTERFACE;
		}

		this->AddRef();
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return 1;
	}

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		return 1;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryService(REFGUID service, REFIID iid, void** ppv)
	{
		if (::IsEqualIID(service, IID_IExplorerPaneVisibility))
		{
			return this->QueryInterface(iid, ppv);
		}

		*ppv = 0;
		return E_NOINTERFACE;
	}

	virtual HRESULT STDMETHODCALLTYPE OnNavigationPending(PCIDLIST_ABSOLUTE pidlFolder)
	{
		MY_TRACE(_T("FilerDialog::OnNavigationPending()\n"));

		return S_OK;
	}
        
	virtual HRESULT STDMETHODCALLTYPE OnViewCreated(IShellView* shellView)
	{
		MY_TRACE(_T("FilerDialog::OnViewCreated()\n"));

		m_shellView = shellView;

		return S_OK;
	}
        
	virtual HRESULT STDMETHODCALLTYPE OnNavigationComplete(PCIDLIST_ABSOLUTE pidlFolder)
	{
		MY_TRACE(_T("FilerDialog::OnNavigationComplete()\n"));

		TCHAR path[MAX_PATH] = {};
		::SHGetPathFromIDList(pidlFolder, path);
		m_currentFolderPath = path;
		m_url.SetWindowText(path);
		m_url.SetEditSel(0, -1);

		IShellBrowserPtr browser = m_explorer;
		HWND hwnd = 0;
		browser->GetWindow(&hwnd);
		::SetWindowText(hwnd, path);

		return S_OK;
	}
        
	virtual HRESULT STDMETHODCALLTYPE OnNavigationFailed(PCIDLIST_ABSOLUTE pidlFolder)
	{
		MY_TRACE(_T("FilerDialog::OnNavigationFailed()\n"));

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE* peps)
	{
		MY_TRACE(_T("FilerDialog::GetPaneState()\n"));
#if 0
		if (ep == EP_NavPane) MY_TRACE(_T("EP_NavPane\n"));
		else if (ep == EP_Commands) MY_TRACE(_T("EP_Commands\n"));
		else if (ep == EP_Commands_Organize) MY_TRACE(_T("EP_Commands_Organize\n"));
		else if (ep == EP_Commands_View) MY_TRACE(_T("EP_Commands_View\n"));
		else if (ep == EP_DetailsPane) MY_TRACE(_T("EP_DetailsPane\n"));
		else if (ep == EP_PreviewPane) MY_TRACE(_T("EP_PreviewPane\n"));
		else if (ep == EP_QueryPane) MY_TRACE(_T("EP_QueryPane\n"));
		else if (ep == EP_AdvQueryPane) MY_TRACE(_T("EP_AdvQueryPane\n"));
		else if (ep == EP_StatusBar) MY_TRACE(_T("EP_StatusBar\n"));
		else if (ep == EP_Ribbon) MY_TRACE(_T("EP_Ribbon\n"));
#endif
		if (ep == EP_NavPane)
			*peps = EPS_DEFAULT_ON;
		else
			*peps = EPS_DEFAULT_OFF | EPS_FORCE;

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE ShouldShow( 
		/* [in] */ __RPC__in_opt IShellFolder *psf,
		/* [unique][in] */ __RPC__in_opt PCIDLIST_ABSOLUTE pidlFolder,
		/* [in] */ __RPC__in PCUITEMID_CHILD pidlItem)
	{
		IShellItemPtr si;
		HRESULT hr = ::SHCreateItemWithParent(0, psf, pidlItem, IID_PPV_ARGS(&si));

		LPWSTR displayName = 0;
		si->GetDisplayName(SIGDN_NORMALDISPLAY, &displayName);

		SFGAOF attributes = 0;
		si->GetAttributes(SFGAO_FOLDER, &attributes);

		MY_TRACE(_T("FilerDialog::ShouldShow(%ws, 0x%08X)\n"), displayName, attributes);

		WCHAR searchString[MAX_PATH] = {};
		::GetWindowTextW(m_search, searchString, MAX_PATH);

		HRESULT retValue = S_OK;

		if (attributes & SFGAO_FOLDER)
		{
			// フォルダの場合
		}
		else
		{
			// ファイルの場合

			if (::lstrlenW(searchString) != 0)
				retValue = ::StrStrIW(displayName, searchString) ? S_OK : S_FALSE;
		}

		::CoTaskMemFree(displayName);

		return retValue;
	}
        
	virtual HRESULT STDMETHODCALLTYPE GetEnumFlags( 
		/* [in] */ __RPC__in_opt IShellFolder *psf,
		/* [in] */ __RPC__in PCIDLIST_ABSOLUTE pidlFolder,
		/* [out] */ __RPC__deref_out_opt HWND *phwnd,
		/* [out][in] */ __RPC__inout DWORD *pgrfFlags)
	{
		MY_TRACE(_T("FilerDialog::GetEnumFlags()\n"));

		return S_OK;
	}

public:

	HWND filerWindow = 0;
	IExplorerBrowserPtr m_explorer;
	IShellViewPtr m_shellView;
	DWORD m_cookie = 0;
	CString m_currentFolderPath;
	CComboBox m_url;
	CEdit m_search;
	BOOL m_isNavPaneVisible = TRUE;

public:

	FilerDialog(HWND filerWindow)
		: filerWindow(filerWindow)
	{
		MY_TRACE(_T("FilerDialog::FilerDialog(0x%08X)\n"), filerWindow);

		if (!Create(IDD_TALENT, CWnd::FromHandle(filerWindow)))
			throw _T("ファイラダイアログの作成に失敗しました");
	}

	~FilerDialog()
	{
		MY_TRACE(_T("FilerDialog::~FilerDialog()\n"));
	}

	void fire_FilerDialogCreated()
	{
		::PostMessage(filerWindow, Share::Filer::Message::FilerDialogCreated, (WPARAM)GetSafeHwnd(), 0);
	}

	void fire_GetUrl()
	{
		::PostMessage(filerWindow, Share::Filer::Message::GetUrl, (WPARAM)GetSafeHwnd(), (LPARAM)m_url.GetSafeHwnd());
	}

	void loadSettings();
	void saveSettings();

	void createExplorer();
	void destroyExplorer();
	void browseToPath(LPCTSTR path);
	void playVoice(LPCTSTR voice);

protected:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TALENT };
#endif
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnAppCommand(CWnd* pWnd, UINT nCmd, UINT nDevice, UINT nKey);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnClickedPrev();
	afx_msg void OnClickedNext();
	afx_msg void OnClickedUp();
	afx_msg void OnClickedGet();
	afx_msg void OnAddFavorite();
	afx_msg void OnDeleteFavorite();
	afx_msg void OnShowNavPane();
	afx_msg void OnPlayVoice();
	afx_msg void OnUseCommonDialog();
	afx_msg void OnUpdateShowNavPane(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePlayVoice(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUseCommonDialog(CCmdUI* pCmdUI);
	afx_msg void OnSelChangeUrl();
	DECLARE_MESSAGE_MAP()
};

//--------------------------------------------------------------------
