#pragma once

namespace fs = std::filesystem;

class MyEditBrowseCtrl : public CMFCEditBrowseCtrl
{
public:

	void init()
	{
		int h = m_rectBtn.Height();
		m_nBrowseButtonWidth = h;
		m_rectBtn.left = m_rectBtn.right - h;
		SetWindowPos(0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOZORDER | SWP_NOMOVE);
	}
};

class InstallDialog : public CDialogEx
{
public:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MAIN_DIALOG };
#endif

	MyEditBrowseCtrl target_edit;

	CString target = _T("");
	BOOL with_runtime = FALSE;
	BOOL with_plugins = FALSE;
	BOOL with_dark = FALSE;

	//
	// 標準コンストラクタです。
	//
	InstallDialog(CWnd* parent = 0)
		: CDialogEx(IDD_MAIN_DIALOG, parent)
	{
	}

	//
	// デストラクタです。
	//
	virtual ~InstallDialog() override
	{
		// メモリリーク対策の処理です。
		CMFCVisualManager::DestroyInstance();
	}

	//
	// 指定されたファイルのチェックサムを返します。
	//
	static DWORD checksum(const std::filesystem::path& path)
	{
#if 1
		DWORD headersum = 0, checksum = 0;
		::MapFileAndCheckSumW(path.c_str(), &headersum, &checksum);
		return checksum;
#else
		std::ifstream ifs(path);
		std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		return std::hash<std::string>()(str);
#endif
	};

	//
	// アルティメットプラグインをインストールします。
	//
	BOOL install()
	{
		try
		{
			constexpr DWORD c_aviutl_checksum = 0x0009690c;
			constexpr DWORD c_exedit_checksum = 0x000d7a64;

			fs::path target((LPCTSTR)this->target);
			auto target_dir = target.parent_path();

			if (!fs::exists(target))
			{
				AfxMessageBox(_T("インストール先が無効です\naviutl.exeを選択してください"));
				return FALSE;
			}

			if (checksum(target) != c_aviutl_checksum)
			{
				if (IDYES != AfxMessageBox(_T("AviUtlのバージョンが無効です\nインストールを続けますか？"), MB_YESNO))
					return FALSE;
			}

			if (checksum(target_dir / L"exedit.auf") != c_exedit_checksum)
			{
				if (IDYES != AfxMessageBox(_T("拡張編集のバージョンが無効です\nインストールを続けますか？"), MB_YESNO))
					return FALSE;
			}

			if (!fs::exists(target_dir / L"patch.aul"))
			{
				if (IDYES != AfxMessageBox(_T("patch.aulが見つかりませんでした\nインストールを続けますか？"), MB_YESNO))
					return FALSE;
			}

			auto from = fs::current_path() / L"aviutl";
			auto to = target_dir;
			auto options = fs::copy_options::overwrite_existing | fs::copy_options::recursive;

			fs::copy(from / L"ultimate.auf", to, options);
			fs::copy(from / L"ultimate.aul", to, options);
			fs::copy(from / L"ultimate", to / L"ultimate", options);

			if (with_runtime)
			{
				auto from = fs::current_path() / L"runtime";

				for (auto it : std::filesystem::directory_iterator(from))
					fs::copy(it.path(), to, options);
			}

			if (with_plugins)
			{
				fs::create_directories(to / L"Plugins");
			}

			if (with_dark)
			{
				auto dark = to / L"aviutl_dark.exe";
				auto manifest = from / L"ultimate" / L"assets" / L"aviutl_dark.exe.manifest";

				fs::copy(target, dark, options);

				auto resource = ::BeginUpdateResource(dark.c_str(), FALSE);
#if 1
				// aviutl_dark.exeを外部マニフェスト形式にします。
				::UpdateResource(
					resource,
					RT_MANIFEST,
					MAKEINTRESOURCE(1),
					MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
					0, 0);
				fs::copy(manifest, to, options);
#else
				// aviutl_dark.exeを内部マニフェスト形式にします。
				std::ifstream ifs(manifest);
				std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
				::UpdateResource(
					resource,
					RT_MANIFEST,
					MAKEINTRESOURCE(1),
					MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
					str.data(), str.size());
#endif
				::EndUpdateResource(resource, FALSE);
			}

			return TRUE;
		}
		catch (fs::filesystem_error& error)
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

		DDX_Control(dx, IDC_TARGET, target_edit);

		DDX_Text(dx, IDC_TARGET, target);
		DDX_Check(dx, IDC_WITH_RUNTIME, with_runtime);
		DDX_Check(dx, IDC_WITH_PLUGINS, with_plugins);
		DDX_Check(dx, IDC_WITH_DARK, with_dark);
	}

	//
	// ダイアログを初期化します。
	//
	virtual BOOL OnInitDialog() override
	{
		__super::OnInitDialog();

		target_edit.init();

		HICON icon = 0;
		::ExtractIconEx(_T("shell32.dll"), 3, 0, &icon, 1);
		target_edit.SetBrowseButtonImage(icon);

		target = AfxGetApp()->GetProfileString(_T("config"), _T("target"));
		with_runtime = AfxGetApp()->GetProfileInt(_T("config"), _T("with_runtime"), with_runtime);
		with_plugins = AfxGetApp()->GetProfileInt(_T("config"), _T("with_plugins"), with_plugins);
		with_dark = AfxGetApp()->GetProfileInt(_T("config"), _T("with_dark"), with_dark);

		UpdateData(FALSE);

		return TRUE;
	}

	//
	// OKボタンが押されたときの処理です。
	//
	virtual void OnOK() override
	{
		UpdateData(TRUE);

		AfxGetApp()->WriteProfileString(_T("config"), _T("target"), target);
		AfxGetApp()->WriteProfileInt(_T("config"), _T("with_runtime"), with_runtime);
		AfxGetApp()->WriteProfileInt(_T("config"), _T("with_plugins"), with_plugins);
		AfxGetApp()->WriteProfileInt(_T("config"), _T("with_dark"), with_dark);

		if (install())
		{
			AfxMessageBox(_T("インストールが完了しました"));

			// インストールに成功したのでダイアログを閉じてプロセスを終了します。
			__super::OnOK();
		}
		else
		{
			AfxMessageBox(_T("インストールに失敗しました"));

			// インストールに失敗したときはダイアログを閉じません。
		}
	}

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(InstallDialog, CDialogEx)
END_MESSAGE_MAP()
