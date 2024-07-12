#pragma once

inline class InstallApp : public CWinApp
{
public:

	MainWindow main_window;

	//
	// 初期化処理を実行します。
	//
	BOOL init()
	{
		hive.module_folder_name = my::get_module_file_name(nullptr).replace_extension();
		hive.config_file_name = hive.module_folder_name / L"config.json";
		hive.spec_file_name = hive.module_folder_name / L"spec.json";

		INITCOMMONCONTROLSEX icc = { sizeof(icc) };
		icc.dwICC = ICC_WIN95_CLASSES;
		::InitCommonControlsEx(&icc);

		if (!main_window.init())
		{
			AfxMessageBox(_T("メインウィンドウの初期化に失敗しました"));

			return FALSE;
		}
		m_pMainWnd = &main_window;

		return TRUE;
	}

	//
	// 後始末処理を実行します。
	//
	BOOL exit()
	{
		main_window.exit();

		return TRUE;
	}

	//
	// コンフィグを読み込みます。
	//
	BOOL read_config()
	{
		if (!std::filesystem::exists(hive.config_file_name))
			return TRUE;

		try
		{
			std::ifstream ifs(hive.config_file_name);
			ptree root;
			read_json(ifs, root);

			get_file_name(root, "aviutl_file_name", hive.aviutl_file_name);
			get_bool(root, "install_new_version", hive.install_new_version);
			get_bool(root, "uninstall_old_version", hive.uninstall_old_version);
			get_bool(root, "deploy_runtime", hive.deploy_runtime);
			get_bool(root, "create_dark_exe", hive.create_dark_exe);

			return TRUE;
		}
		catch (const std::exception& error)
		{
			AfxMessageBox(std::format(
				L"{}を読込中にエラーが発生しました" L"\n"
				L"{}",
				hive.config_file_name,
				my::ws(error.what())).c_str());

			return FALSE;
		}
	}

	//
	// コンフィグを書き込みます。
	//
	BOOL write_config()
	{
		try
		{
			ptree root;

			set_file_name(root, "aviutl_file_name", hive.aviutl_file_name);
			set_bool(root, "install_new_version", hive.install_new_version);
			set_bool(root, "uninstall_old_version", hive.uninstall_old_version);
			set_bool(root, "deploy_runtime", hive.deploy_runtime);
			set_bool(root, "create_dark_exe", hive.create_dark_exe);

			std::ofstream ofs(hive.config_file_name);
			write_json(ofs, root);

			return TRUE;
		}
		catch (const std::exception& error)
		{
			AfxMessageBox(std::format(
				L"{}を保存中にエラーが発生しました" L"\n"
				L"{}",
				hive.config_file_name,
				my::ws(error.what())).c_str());

			return FALSE;
		}
	}

	//
	// メイン処理を実行します。
	//
	BOOL execute()
	{
		OptionDialog dialog;
		if (IDOK != dialog.DoModal())
		{
			AfxMessageBox(_T("インストールを中止しました"));

			return FALSE;
		}

		return TRUE;
	}

	//
	// MFCの初期化関数です。
	// 実質的なWinMain()です。
	//
	virtual BOOL InitInstance() override
	{
		__super::InitInstance();

		if (!init()) return FALSE;

		read_config();
		execute();
		write_config();

		exit();

		return FALSE;
	}

	DECLARE_MESSAGE_MAP()
} theApp;

BEGIN_MESSAGE_MAP(InstallApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()
