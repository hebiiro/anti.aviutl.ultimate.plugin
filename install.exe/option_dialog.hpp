#pragma once

class MyEditBrowseCtrl : public CMFCEditBrowseCtrl
{
public:

	void init()
	{
		auto h = m_rectBtn.Height();
		m_nBrowseButtonWidth = h;
		m_rectBtn.left = m_rectBtn.right - h;
		SetWindowPos(nullptr, 0, 0, 0, 0,
			SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE);
	}
};

class OptionDialog : public CDialogEx
{
public:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OPTION_DIALOG };
#endif

	MyEditBrowseCtrl aviutl_file_name_edit;
	CString aviutl_file_name = _T("");
	BOOL install_new_version = TRUE;
	BOOL uninstall_old_version = TRUE;
	BOOL deploy_runtime = TRUE;
	BOOL create_dark_exe = TRUE;

	//
	// 標準コンストラクタです。
	//
	OptionDialog(CWnd* parent = nullptr)
		: CDialogEx(IDD_OPTION_DIALOG, parent)
	{
	}

	//
	// デストラクタです。
	//
	virtual ~OptionDialog() override
	{
		// メモリリーク対策の処理です。
		CMFCVisualManager::DestroyInstance();
	}

	//
	// 指定されたファイルのチェックサムを返します。
	//
	inline static DWORD get_checksum(const std::filesystem::path& path)
	{
#if 1
		DWORD headersum = 0, get_checksum = 0;
		::MapFileAndCheckSumW(path.c_str(), &headersum, &get_checksum);
		return get_checksum;
#else
		std::ifstream ifs(path);
		std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		return std::hash<std::string>()(str);
#endif
	}

	//
	// 入力されたオプションを適用します。
	//
	BOOL apply_option()
	{
		try
		{
			constexpr DWORD c_aviutl_checksum = 0x0009690c;
			constexpr DWORD c_exedit_checksum = 0x000d7a64;

			hive.aviutl_dir = hive.aviutl_file_name.parent_path();
			hive.exedit_file_name = hive.aviutl_dir / L"exedit.auf";
			hive.from = std::filesystem::current_path();
			hive.to = hive.aviutl_dir;

			if (!std::filesystem::exists(hive.aviutl_file_name))
			{
				AfxMessageBox(
					_T("インストール先が無効です") _T("\n")
					_T("aviutl.exeを選択してください"));

				return FALSE;
			}

			if (get_checksum(hive.aviutl_file_name) != c_aviutl_checksum)
			{
				AfxMessageBox(
					_T("AviUtlのバージョンが無効です") _T("\n")
					_T("『AviUtl version1.10』のaviutl.exeを選択してください"));

				return FALSE;
			}

			if (get_checksum(hive.exedit_file_name) != c_exedit_checksum)
			{
				AfxMessageBox(
					_T("拡張編集のバージョンが無効です") _T("\n")
					_T("先に『拡張編集 version0.92』をインストールしてください"));

				return FALSE;
			}

			return TRUE;
		}
		catch (std::filesystem::filesystem_error& error)
		{
			AfxMessageBox((CString)error.what());

			return FALSE;
		}
	}

	//
	// メンバ変数をコントロールにバインドします。
	//
	virtual void DoDataExchange(CDataExchange* dx) override
	{
		__super::DoDataExchange(dx);

		DDX_Control(dx, IDC_AVIUTL_FILE_NAME, aviutl_file_name_edit);
		DDX_Text(dx, IDC_AVIUTL_FILE_NAME, aviutl_file_name);
		DDX_Check(dx, IDC_INSTALL_NEW_VERSION, install_new_version);
		DDX_Check(dx, IDC_UNINSTALL_OLD_VERSION, uninstall_old_version);
		DDX_Check(dx, IDC_DEPLOY_RUNTIME, deploy_runtime);
		DDX_Check(dx, IDC_CREATE_DARK_EXE, create_dark_exe);
	}

	//
	// ダイアログを初期化します。
	//
	virtual BOOL OnInitDialog() override
	{
		__super::OnInitDialog();

		aviutl_file_name_edit.init();

		HICON icon = nullptr;
		::ExtractIconEx(_T("shell32.dll"), 3, 0, &icon, 1);
		aviutl_file_name_edit.SetBrowseButtonImage(icon);

		aviutl_file_name = hive.aviutl_file_name.c_str();
		install_new_version = hive.install_new_version;
		uninstall_old_version = hive.uninstall_old_version;
		deploy_runtime = hive.deploy_runtime;
		create_dark_exe = hive.create_dark_exe;

		UpdateData(FALSE);

		return TRUE;
	}

	//
	// OKボタンが押されたときの処理です。
	//
	virtual void OnOK() override
	{
		UpdateData(TRUE);

		hive.aviutl_file_name = (LPCTSTR)aviutl_file_name;
		hive.install_new_version = install_new_version;
		hive.uninstall_old_version = uninstall_old_version;
		hive.deploy_runtime = deploy_runtime;
		hive.create_dark_exe = create_dark_exe;

		// オプションが適用できた場合は
		if (apply_option())
		{
			// 一旦オプションダイアログを非表示にします。
			ShowWindow(SW_HIDE);

			// レビューダイアログを表示します。
			ReviewDialog dialog;

			// レビューが完了した場合は
			if (IDOK == dialog.DoModal())
			{
				// オプションダイアログを閉じます。
				__super::OnOK();
			}
			// レビューが完了しなかった場合は
			else
			{
				// オプションダイアログを再表示します。
				ShowWindow(SW_SHOW);
			}
		}
	}

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(OptionDialog, CDialogEx)
END_MESSAGE_MAP()
