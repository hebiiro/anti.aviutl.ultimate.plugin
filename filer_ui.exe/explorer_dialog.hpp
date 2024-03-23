#pragma once

namespace apn::filer_ui
{
	//
	// このクラスはエクスプローラダイアログです。
	//
	class ExplorerDialog
		: public CDialogEx
		, public IServiceProvider
		, public IExplorerBrowserEvents
		, public IExplorerPaneVisibility
		, public IFolderFilter
	{
	public:

		IExplorerBrowserPtr explorer;
		IShellViewPtr shell_view;
		DWORD cookie = 0;

		std::basic_string<TCHAR> hot_folder;
		CComboBox folder_control;
		CEdit search_control;
		CButton nav_pane_control;

		inline static auto get_path_from_idlist(PCIDLIST_ABSOLUTE idlist, size_t buffer_length = MAX_PATH)
		{
			std::basic_string<TCHAR> path;
			path.resize(buffer_length);
			::SHGetPathFromIDList(idlist, path.data());
			path.resize(wcslen(path.data()));
			return path;
		}

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

		virtual HRESULT STDMETHODCALLTYPE OnNavigationPending(PCIDLIST_ABSOLUTE idlist_folder)
		{
			MY_TRACE_FUNC("");

			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE OnViewCreated(IShellView* shell_view)
		{
			MY_TRACE_FUNC("");

			this->shell_view = shell_view;

			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE OnNavigationComplete(PCIDLIST_ABSOLUTE idlist_folder)
		{
			MY_TRACE_FUNC("");

			auto path = get_path_from_idlist(idlist_folder);
			hot_folder = path;
			folder_control.SetWindowText(path.c_str());
			folder_control.SetEditSel(0, -1);

			IShellBrowserPtr browser = explorer;
			HWND hwnd = 0; browser->GetWindow(&hwnd);
			::SetWindowText(hwnd, path.c_str());

			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE OnNavigationFailed(PCIDLIST_ABSOLUTE idlist_folder)
		{
			MY_TRACE_FUNC("");

			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE GetPaneState(REFEXPLORERPANE ep, EXPLORERPANESTATE* peps)
		{
//			MY_TRACE_FUNC("");
#if 0
			if (ep == EP_NavPane) MY_TRACE("EP_NavPane\n");
			else if (ep == EP_Commands) MY_TRACE("EP_Commands\n");
			else if (ep == EP_Commands_Organize) MY_TRACE("EP_Commands_Organize\n");
			else if (ep == EP_Commands_View) MY_TRACE("EP_Commands_View\n");
			else if (ep == EP_DetailsPane) MY_TRACE("EP_DetailsPane\n");
			else if (ep == EP_PreviewPane) MY_TRACE("EP_PreviewPane\n");
			else if (ep == EP_QueryPane) MY_TRACE("EP_QueryPane\n");
			else if (ep == EP_AdvQueryPane) MY_TRACE("EP_AdvQueryPane\n");
			else if (ep == EP_StatusBar) MY_TRACE("EP_StatusBar\n");
			else if (ep == EP_Ribbon) MY_TRACE("EP_Ribbon\n");
#endif
			if (ep == EP_NavPane)
				*peps = EPS_DEFAULT_ON;
			else
				*peps = EPS_DEFAULT_OFF | EPS_FORCE;

			return S_OK;
		}

		template <typename T>
		struct CoTaskMem
		{
			T p;
			explicit CoTaskMem(T p = 0) : p(p) {}
			~CoTaskMem() { if (p) ::CoTaskMemFree(p); }
			T* operator&() { return &p; }
			operator T() { return p; }
		};

		virtual HRESULT STDMETHODCALLTYPE ShouldShow( 
			/* [in] */ __RPC__in_opt IShellFolder *psf,
			/* [unique][in] */ __RPC__in_opt PCIDLIST_ABSOLUTE idlist_folder,
			/* [in] */ __RPC__in PCUITEMID_CHILD pidlItem)
		{
			IShellItemPtr si;
			HRESULT hr = ::SHCreateItemWithParent(0, psf, pidlItem, IID_PPV_ARGS(&si));

			CoTaskMem<LPWSTR> display_name;
			si->GetDisplayName(SIGDN_NORMALDISPLAY, &display_name);

			SFGAOF attributes = 0;
			si->GetAttributes(SFGAO_FOLDER, &attributes);

	//		MY_TRACE_FUNC("{}, {:#010x}", (LPWSTR)display_name, attributes);

			auto search_str = my::get_window_text(search_control);

			HRESULT ret_value = S_OK;

			// フォルダの場合は
			if (attributes & SFGAO_FOLDER)
			{
				// 何もしません。
			}
			// ファイルの場合は
			else
			{
				// フィルタを適用します。

				if (!search_str.empty())
					ret_value = ::StrStrIW(display_name, search_str.c_str()) ? S_OK : S_FALSE;
			}

			return ret_value;
		}

		virtual HRESULT STDMETHODCALLTYPE GetEnumFlags( 
			/* [in] */ __RPC__in_opt IShellFolder *psf,
			/* [in] */ __RPC__in PCIDLIST_ABSOLUTE idlist_folder,
			/* [out] */ __RPC__deref_out_opt HWND *phwnd,
			/* [out][in] */ __RPC__inout DWORD *pgrfFlags)
		{
			MY_TRACE_FUNC("");

			return S_OK;
		}

		void init_explorer(CWnd* placeholder)
		{
			MY_TRACE_FUNC("");

			// エクスプローラを作成します。
			HRESULT hr = explorer.CreateInstance(CLSID_ExplorerBrowser);

			{
				// エクスプローラを初期化します。

				CRect rc; placeholder->GetWindowRect(&rc);
				ScreenToClient(&rc);

				FOLDERSETTINGS fs = {};
				fs.ViewMode = FVM_AUTO;
				fs.fFlags = 0;//FWF_NOBROWSERVIEWSTATE;

				hr = explorer->Initialize(GetSafeHwnd(), &rc, &fs);
				hr = explorer->SetPropertyBag(L"apn::filer");
			}

			// エクスプローラにハンドラを追加します。
			hr = ::IUnknown_SetSite(explorer, static_cast<IServiceProvider*>(this));
			hr = explorer->Advise(static_cast<IExplorerBrowserEvents*>(this), &cookie);
			IFolderFilterSitePtr folderFilterSite = explorer;
			MY_TRACE_HEX(folderFilterSite.GetInterfacePtr());
			hr = folderFilterSite->SetFilter(static_cast<IFolderFilter*>(this));
			MY_TRACE_COM_ERROR(hr);

			if (nav_pane_control.GetCheck() == BST_CHECKED)
			{
				// ナビゲーションペインを表示します。
				hr = explorer->SetOptions(EBO_SHOWFRAMES);
			}

			auto folder_name = my::get_window_text(folder_control);
			MY_TRACE_STR(folder_name);

			{
				// 一旦デスクトップを表示します。
				// こうしないと次のbrowse_to_path(folder_name)でフリーズしてしまいます。

				PIDLIST_ABSOLUTE idlist_desktop;
				::SHGetKnownFolderIDList(FOLDERID_Desktop, 0, 0, &idlist_desktop);
				explorer->BrowseToIDList(idlist_desktop, SBSP_ABSOLUTE);
				::ILFree(idlist_desktop);
			}

			MY_TRACE("指定されたフォルダに移動します\n");

			if (!folder_name.empty())
				browse_to_path(folder_name.c_str());
		}

		void exit_explorer()
		{
			MY_TRACE_FUNC("");

			if (!explorer) return;

			shell_view = 0;
			explorer->Unadvise(cookie), cookie = 0;
			::IUnknown_SetSite(explorer, 0);
			explorer->Destroy(), explorer = 0;
		}

		void resize_explorer(CWnd* placeholder)
		{
			MY_TRACE_FUNC("");

			if (!explorer) return;
			if (!placeholder) return;

			CRect rc; placeholder->GetWindowRect(&rc);
			ScreenToClient(&rc);
			explorer->SetRect(0, rc);
		}

		void browse_to_path(LPCTSTR path)
		{
			MY_TRACE_FUNC("{}", path);

			if (!explorer) return;

			// pathが現在のフォルダと同じなら
			if (::StrCmpI(path, hot_folder.c_str()) == 0)
			{
				// 現在のフォルダを再読み込みします。

				if (shell_view)
				{
					HRESULT hr = shell_view->Refresh();
					MY_TRACE_COM_ERROR(hr);
				}
			}
			else
			{
				// 指定されたフォルダを表示します。

				PIDLIST_ABSOLUTE idlist = ::ILCreateFromPath(path);
				if (idlist)
				{
					explorer->BrowseToIDList(idlist, SBSP_ABSOLUTE | SBSP_SAMEBROWSER);
					::ILFree(idlist);
				}
			}
		}

		virtual BOOL PreTranslateMessage(MSG* msg) override
		{
			if (shell_view && shell_view->TranslateAccelerator(msg) == S_OK)
				return TRUE;

			return CDialogEx::PreTranslateMessage(msg);
		}
	};
}
