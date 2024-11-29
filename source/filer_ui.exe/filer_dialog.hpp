#pragma once

namespace apn::filer_ui
{
	class FilerDialog : public ExplorerDialog
	{
	public:

		inline static std::vector<std::shared_ptr<FilerDialog>> collection;

		HWND filer_window = nullptr;
		CToolTipCtrl tooltip;

		//
		// ファイラダイアログのインスタンスを作成して返します。
		//
		inline static std::shared_ptr<FilerDialog> create_instance(HWND filer_window, BOOL full)
		{
			MY_TRACE_FUNC("{:#010x}, {}", filer_window, full);

			return collection.emplace_back(std::make_shared<FilerDialog>(
				my::get_window_text(filer_window).c_str(), filer_window, full));
		}

		//
		// コンストラクタです。
		// 内部的に使用されます。create_instance()から呼ばれます。
		//
		FilerDialog(LPCTSTR name, HWND filer_window, BOOL full)
			: filer_window(filer_window)
		{
			MY_TRACE_FUNC("{}, {:#010x}", name, filer_window);

			if (!__super::Create(IDD_FILER_DIALOG, AfxGetMainWnd()))
				throw _T("ファイラダイアログの作成に失敗しました");

			// ウィンドウ名を設定します。
			SetWindowText(name);

			if (full)
			{
				// エクスプローラを作成します。
				init_explorer();
			}

			// ファイラダイアログの作成が完了したことをファイラウィンドウに通知します。
			// これにより、ファイラウィンドウがファイラダイアログを参照することができるようになります。
			fire_init(full);
		}

		//
		// デストラクタです。
		//
		virtual ~FilerDialog() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// ファイラウィンドウに初期化が完了したことを通知します。
		//
		void fire_init(BOOL full)
		{
			::PostMessage(filer_window, share::message::c_init, (WPARAM)GetSafeHwnd(), (LPARAM)full);
		}

		//
		// ファイラウィンドウにファイル名を取得する必要があることを通知します。
		//
		void fire_get_file_name()
		{
			::PostMessage(filer_window, share::message::c_get_file_name, (WPARAM)GetSafeHwnd(), (LPARAM)folder_control.GetSafeHwnd());
		}

		//
		// コンフィグを読み込みます。
		//
		void read(const n_json& node)
		{
			MY_TRACE_FUNC("");

			// 一旦ブックマークを削除します。
			folder_control.ResetContent();

			auto nav_pane = true;
			get_bool(node, "nav_pane", nav_pane);
			nav_pane_control.SetCheck(nav_pane ? BST_CHECKED : BST_UNCHECKED);

			std::wstring path;
			get_string(node, "path", path);
			folder_control.SetWindowText(path.c_str());

			// ブックマークを読み込みます。
			get_child_nodes(node, "bookmarks",
				[&](const n_json& bookmark_node, size_t i)
			{
				std::wstring path;
				get_string(bookmark_node, path);
				folder_control.AddString(path.c_str());

				return TRUE;
			});

			// エクスプローラを作り直します。
			exit_explorer();
			init_explorer();
		}

		//
		// コンフィグを書き込みます。
		//
		void write(n_json& node)
		{
			MY_TRACE_FUNC("");

			auto nav_pane = this->nav_pane_control.GetCheck() == BST_CHECKED;

			set_bool(node, "nav_pane", nav_pane);
			set_string(node, "path", hot_folder);

			std::vector<std::wstring> bookmarks;
			auto c = folder_control.GetCount();
			MY_TRACE_INT(c);
			for (decltype(c) i = 0; i < c; i++)
			{
				CString path; folder_control.GetLBText(i, path);
				MY_TRACE_STR((LPCTSTR)path);

				bookmarks.emplace_back((LPCTSTR)path);
			}

			set_child_nodes(node, "bookmarks", bookmarks,
				[&](n_json& bookmark_node, const auto& bookmark, size_t i)
			{
				set_string(bookmark_node, bookmark);

				return TRUE;
			});
		}

		//
		// エクスプローラを作成します。
		//
		void init_explorer()
		{
			MY_TRACE_FUNC("開始");

			__super::init_explorer(GetDlgItem(IDC_PLACE_HOLDER));

			MY_TRACE("フィルタウィンドウにブラウザのウィンドウハンドルをセットします\n");

			IShellBrowserPtr browser = explorer;
			MY_TRACE_HEX(browser.GetInterfacePtr());
			HWND hwnd = nullptr; browser->GetWindow(&hwnd);
			MY_TRACE_HEX(hwnd);
			share::filer_window::set_browser(filer_window, hwnd);

			MY_TRACE_FUNC("完了");
		}

		//
		// エクスプローラを削除します。
		//
		void exit_explorer()
		{
			MY_TRACE_FUNC("開始");

			__super::exit_explorer();

			MY_TRACE_FUNC("完了");
		}

		//
		// エクスプローラをリサイズします。
		//
		void resize_explorer()
		{
			MY_TRACE_FUNC("開始");

			__super::resize_explorer(GetDlgItem(IDC_PLACE_HOLDER));

			MY_TRACE_FUNC("完了");
		}

		//
		// 指定されたコントロールに指定されたツールチップテキストを付与します。
		//
		BOOL add_tool(UINT id, LPCTSTR text)
		{
			TOOLINFO ti { sizeof(ti) };
			ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | TTF_TRANSPARENT;
			ti.hwnd = GetSafeHwnd();
			ti.uId = (UINT)GetDlgItem(id)->GetSafeHwnd();
			ti.lpszText = const_cast<LPTSTR>(text);
			return tooltip.SendMessage(TTM_ADDTOOL, 0, (LPARAM)&ti);
		}

#ifdef AFX_DESIGN_TIME
		enum { IDD = IDD_FILER_DIALOG };
#endif

		virtual void DoDataExchange(CDataExchange* pDX) override
		{
			MY_TRACE_FUNC("");

			CDialogEx::DoDataExchange(pDX);
			DDX_Control(pDX, IDC_FOLDER, folder_control);
			DDX_Control(pDX, IDC_SEARCH, search_control);
			DDX_Control(pDX, IDC_HAS_NAV_PANE, nav_pane_control);
		}

		virtual BOOL PreTranslateMessage(MSG* msg) override
		{
			if (msg->message == WM_KEYDOWN)
			{
				switch (msg->wParam)
				{
				case _T('F'):
					{
						if (::GetKeyState(VK_CONTROL) < 0)
						{
							search_control.SetFocus();
							return TRUE;
						}

						break;
					}
				case _T('G'):
					{
						if (::GetKeyState(VK_CONTROL) < 0)
						{
							folder_control.SetFocus();
							return TRUE;
						}

						break;
					}
				}
			}

			return CDialogEx::PreTranslateMessage(msg);
		}

		virtual void PostNcDestroy() override
		{
			MY_TRACE_FUNC("");

			CDialogEx::PostNcDestroy();

			// thisをコレクションから取り除きます。この処理は実質的にdelete thisと同じです。
			std::erase_if(collection, [this](const auto& x){ return x.get() == this; });
		}

		virtual BOOL OnInitDialog() override
		{
			MY_TRACE_FUNC("");

			CDialogEx::OnInitDialog();

			tooltip.Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
			tooltip.SetMaxTipWidth(INT_MAX);
			add_tool(IDC_PREV_FOLDER, _T("前のフォルダに戻ります"));
			add_tool(IDC_NEXT_FOLDER, _T("次のフォルダに進みます"));
			add_tool(IDC_PARENT_FOLDER, _T("一つ上の階層へ移動します"));
			add_tool(IDC_GET_FILE_NAME, _T("AviUtlのカレントアイテムからフォルダを取得します"));
			add_tool(IDC_ADD_BOOKMARK, _T("現在のフォルダをブックマークに追加します"));
			add_tool(IDC_REMOVE_BOOKMARK, _T("現在のフォルダをブックマークから削除します"));
			add_tool(IDC_HAS_NAV_PANE, _T("ツリービューの表示/非表示を切り替えます"));
			add_tool(IDC_SEARCH, _T("ファイルをフィルタリングするための文字列です"));
			add_tool(IDC_FOLDER, _T("現在表示中のフォルダです")
				_T("\n手動で変更した場合はエンターキーを押して確定してください")
				_T("\nリストダウンするとブックマークから選択できます"));

			return TRUE;
		}

		virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) override
		{
			auto nm = (NMHDR*)lParam;
			if (nm->hwndFrom == tooltip.GetSafeHwnd())
			{
				switch (nm->code)
				{
				case TTN_SHOW:
					{
						MY_TRACE_FUNC("TTN_SHOW");

						CRect rc; tooltip.GetWindowRect(&rc);
						auto x = rc.left;
						auto y = rc.top;
						auto w = rc.Width();
						auto h = rc.Height();

						{
							// ツールチップのX座標をマウスカーソルのX座標に設定します。
							auto pt = my::get_cursor_pos();

							x = pt.x;
						}

						{
							// ツールチップのY座標をダイアログの左上に設定します。
							CRect rc; GetClientRect(&rc);
							ClientToScreen(&rc);

							y = rc.top - h;
						}

						tooltip.SetWindowPos(nullptr, x, y, w, h,
							SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

						*pResult = TRUE;
						return TRUE;
					}
				}
			}

			return __super::OnNotify(wParam, lParam, pResult);
		}

		int OnCreate(LPCREATESTRUCT cs)
		{
			MY_TRACE_FUNC("");

			if (CDialogEx::OnCreate(cs) == -1)
				return -1;

			return 0;
		}

		void OnDestroy()
		{
			MY_TRACE_FUNC("");

			// エクスプローラを削除します。
			exit_explorer();

			CDialogEx::OnDestroy();
		}

		void OnSize(UINT nType, int cx, int cy)
		{
			CDialogEx::OnSize(nType, cx, cy);

			// エクスプローラをリサイズします。
			resize_explorer();
		}

		void OnAppCommand(CWnd* pWnd, UINT nCmd, UINT nDevice, UINT nKey)
		{
			switch (nCmd)
			{
			case APPCOMMAND_BROWSER_BACKWARD:
				{
					MY_TRACE_FUNC("APPCOMMAND_BROWSER_BACKWARD");

					on_go_to_prev_folder();
					break;
				}
			case APPCOMMAND_BROWSER_FORWARD:
				{
					MY_TRACE_FUNC("APPCOMMAND_BROWSER_FORWARD");

					on_go_to_next_folder();
					break;
				}
			case APPCOMMAND_BROWSER_REFRESH:
				{
					MY_TRACE_FUNC("APPCOMMAND_BROWSER_REFRESH");

					break;
				}
			}

			__super::OnAppCommand(pWnd, nCmd, nDevice, nKey);
		}

		void OnOK()
		{
			MY_TRACE_FUNC("");

			auto folder_name = my::get_window_text(folder_control);
			MY_TRACE_STR(folder_name);

			browse_to_path(folder_name.c_str());

//			__super::OnOK();
		}

		void OnCancel()
		{
			MY_TRACE_FUNC("");

//			__super::OnCancel();
		}

		void on_go_to_prev_folder()
		{
			MY_TRACE_FUNC("");

			if (!explorer) return;

			explorer->BrowseToIDList(NULL, SBSP_NAVIGATEBACK);
		}

		void on_go_to_next_folder()
		{
			MY_TRACE_FUNC("");

			if (!explorer) return;

			explorer->BrowseToIDList(NULL, SBSP_NAVIGATEFORWARD);
		}

		void on_go_to_parent_folder()
		{
			MY_TRACE_FUNC("");

			if (!explorer) return;

			explorer->BrowseToIDList(NULL, SBSP_PARENT);
		}

		void on_get_file_name()
		{
			MY_TRACE_FUNC("");

			if (!explorer) return;

			fire_get_file_name();
		}

		void on_add_bookmark()
		{
			MY_TRACE_FUNC("");

			auto folder_name = my::get_window_text(folder_control);
			MY_TRACE_STR(folder_name);
			auto index = folder_control.FindStringExact(0, folder_name.c_str());
			if (index == CB_ERR) folder_control.AddString(folder_name.c_str());
		}

		void on_remove_bookmark()
		{
			MY_TRACE("on_remove_bookmark()\n");

			auto folder_name = my::get_window_text(folder_control);
			MY_TRACE_STR(folder_name);
			auto index = folder_control.FindStringExact(0, folder_name.c_str());
			if (index != CB_ERR) folder_control.DeleteString(index);
		}

		void on_nav_pane()
		{
			MY_TRACE_FUNC("");

			exit_explorer();
			init_explorer();
		}

		void on_folder_sel_change()
		{
			MY_TRACE_FUNC("");

			auto index = folder_control.GetCurSel();
			MY_TRACE_INT(index);
			if (index != CB_ERR)
			{
				CString folder_name;
				folder_control.GetLBText(index, folder_name);
				MY_TRACE_STR((LPCTSTR)folder_name);

				browse_to_path(folder_name);
			}
		}

		//
		// ファイラウィンドウが削除されるときにホストプロセスから通知されます。
		// この時点でファイラウィンドウとの親子関係は解除されているので、手動でウィンドウを破壊します。
		//
		LRESULT on_exit_filer_window(WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}", wParam, lParam);

			return DestroyWindow();
		}

		DECLARE_MESSAGE_MAP()
	};

	BEGIN_MESSAGE_MAP(FilerDialog, CDialogEx)
		ON_WM_CREATE()
		ON_WM_DESTROY()
		ON_WM_SIZE()
		ON_WM_APPCOMMAND()
		ON_COMMAND(IDC_PREV_FOLDER, &on_go_to_prev_folder)
		ON_COMMAND(IDC_NEXT_FOLDER, &on_go_to_next_folder)
		ON_COMMAND(IDC_PARENT_FOLDER, &on_go_to_parent_folder)
		ON_COMMAND(IDC_GET_FILE_NAME, &on_get_file_name)
		ON_COMMAND(IDC_ADD_BOOKMARK, &on_add_bookmark)
		ON_COMMAND(IDC_REMOVE_BOOKMARK, &on_remove_bookmark)
		ON_COMMAND(IDC_HAS_NAV_PANE, &on_nav_pane)
		ON_CBN_SELCHANGE(IDC_FOLDER, &on_folder_sel_change)
		ON_MESSAGE(share::message::c_exit_filer_window, on_exit_filer_window)
	END_MESSAGE_MAP()
}
