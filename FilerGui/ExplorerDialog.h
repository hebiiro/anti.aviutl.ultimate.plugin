#pragma once

class ExplorerDialog
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
		MY_TRACE(_T("ExplorerDialog::OnNavigationPending()\n"));

		return S_OK;
	}
        
	virtual HRESULT STDMETHODCALLTYPE OnViewCreated(IShellView* shellView)
	{
		MY_TRACE(_T("ExplorerDialog::OnViewCreated()\n"));

		this->shellView = shellView;

		return S_OK;
	}
        
	virtual HRESULT STDMETHODCALLTYPE OnNavigationComplete(PCIDLIST_ABSOLUTE pidlFolder)
	{
		MY_TRACE(_T("ExplorerDialog::OnNavigationComplete()\n"));

		TCHAR path[MAX_PATH] = {};
		::SHGetPathFromIDList(pidlFolder, path);
		hotFolder = path;
		folder.SetWindowText(path);
		folder.SetEditSel(0, -1);

		IShellBrowserPtr browser = explorer;
		HWND hwnd = 0;
		browser->GetWindow(&hwnd);
		::SetWindowText(hwnd, path);

		return S_OK;
	}
        
	virtual HRESULT STDMETHODCALLTYPE OnNavigationFailed(PCIDLIST_ABSOLUTE pidlFolder)
	{
		MY_TRACE(_T("ExplorerDialog::OnNavigationFailed()\n"));

		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE* peps)
	{
//		MY_TRACE(_T("ExplorerDialog::GetPaneState()\n"));
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

//		MY_TRACE(_T("ExplorerDialog::ShouldShow(%ws, 0x%08X)\n"), displayName, attributes);

		WCHAR searchString[MAX_PATH] = {};
		::GetWindowTextW(search, searchString, std::size(searchString));

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
		MY_TRACE(_T("ExplorerDialog::GetEnumFlags()\n"));

		return S_OK;
	}

public:

	IExplorerBrowserPtr explorer;
	IShellViewPtr shellView;
	DWORD cookie = 0;

	CString hotFolder;
	CComboBox folder;
	CEdit search;
	CButton hasNavPane;

public:

	void initExplorer(CWnd* placeHolder)
	{
		// エクスプローラを作成します。
		HRESULT hr = explorer.CreateInstance(CLSID_ExplorerBrowser);

		{
			// エクスプローラを初期化します。

			CRect rc; placeHolder->GetWindowRect(&rc);
			ScreenToClient(&rc);

			FOLDERSETTINGS fs = {};
			fs.ViewMode = FVM_AUTO;
			fs.fFlags = 0;//FWF_NOBROWSERVIEWSTATE;

			hr = explorer->Initialize(GetSafeHwnd(), &rc, &fs);
			hr = explorer->SetPropertyBag(L"AviUtl.Ultimate.Filer");
		}

		// エクスプローラにハンドラを追加します。
		hr = ::IUnknown_SetSite(explorer, static_cast<IServiceProvider*>(this));
		hr = explorer->Advise(static_cast<IExplorerBrowserEvents*>(this), &cookie);
		IFolderFilterSitePtr folderFilterSite = explorer;
		MY_TRACE_HEX(folderFilterSite.GetInterfacePtr());
		hr = folderFilterSite->SetFilter(static_cast<IFolderFilter*>(this));
		MY_TRACE_COM_ERROR(hr);

		if (this->hasNavPane.GetCheck() == BST_CHECKED)
		{
			// ナビゲーションペインを表示します。
			hr = explorer->SetOptions(EBO_SHOWFRAMES);
		}

		CString folderName; folder.GetWindowText(folderName);
		MY_TRACE_TSTR((LPCTSTR)folderName);

		{
			// 一旦デスクトップを表示します。
			// こうしないと次のbrowseToPath(folderName)でフリーズしてしまいます。

			PIDLIST_ABSOLUTE pidlDesktop;
			::SHGetKnownFolderIDList(FOLDERID_Desktop, 0, NULL, &pidlDesktop);
			explorer->BrowseToIDList(pidlDesktop, SBSP_ABSOLUTE);
			::ILFree(pidlDesktop);
		}

		MY_TRACE(_T("指定されたフォルダに移動します\n"));

		if (!folderName.IsEmpty())
			browseToPath(folderName);
	}

	void exitExplorer()
	{
		if (!explorer) return;

		shellView = 0;
		explorer->Unadvise(cookie), cookie = 0;
		::IUnknown_SetSite(explorer, 0);
		explorer->Destroy(), explorer = 0;
	}

	void resizeExplorer(CWnd* placeHolder)
	{
		if (!explorer) return;

		if (!placeHolder) return;

		CRect rc; placeHolder->GetWindowRect(&rc);
		ScreenToClient(&rc);
		explorer->SetRect(0, rc);
	}

	void browseToPath(LPCTSTR path)
	{
		MY_TRACE(_T("ExplorerDialog::browseToPath(%s)\n"), path);

		if (!explorer) return;

		// pathが現在のフォルダと同じなら
		if (::lstrcmpi(path, hotFolder) == 0)
		{
			// 現在のフォルダを再読み込みします。

			if (shellView)
			{
				HRESULT hr = shellView->Refresh();
				MY_TRACE_COM_ERROR(hr);
			}
		}
		else
		{
			// 指定されたフォルダを表示します。

			PIDLIST_ABSOLUTE pidl = ::ILCreateFromPath(path);
			if (pidl)
			{
				explorer->BrowseToIDList(pidl, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
				::ILFree(pidl);
			}
		}
	}

protected:

	virtual BOOL PreTranslateMessage(MSG* pMsg) override
	{
		if (shellView && shellView->TranslateAccelerator(pMsg) == S_OK)
			return TRUE;

		return CDialogEx::PreTranslateMessage(pMsg);
	}
};
