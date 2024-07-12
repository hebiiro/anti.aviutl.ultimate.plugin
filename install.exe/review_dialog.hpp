#pragma once

class ReviewDialog : public CDialogEx
{
public:

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_REVIEW_DIALOG };
#endif

	inline static constexpr struct Column {
		inline static constexpr int c_op = 0;
		inline static constexpr int c_from = 1;
		inline static constexpr int c_to = 2;
	} c_col;

	CListCtrl reviews;

	//
	// 標準コンストラクタです。
	//
	ReviewDialog(CWnd* parent = nullptr)
		: CDialogEx(IDD_REVIEW_DIALOG, parent)
	{
	}

	//
	// デストラクタです。
	//
	virtual ~ReviewDialog() override
	{
		// メモリリーク対策の処理です。
		CMFCVisualManager::DestroyInstance();
	}

	//
	// レビューをリストに追加します。
	//
	void insert_review(int review_index, const std::wstring& op, const std::wstring& from, const std::wstring& to)
	{
		auto item_index = reviews.InsertItem(review_index, op.c_str());
		reviews.SetItemText(item_index, c_col.c_from, from.c_str());
		reviews.SetItemText(item_index, c_col.c_to, to.c_str());
		reviews.SetCheck(item_index, TRUE);
	}

	//
	// 入力されたレビューを適用します。
	//
	BOOL apply_review()
	{
		CWaitCursor wait_cursor;

		auto options =
			std::filesystem::copy_options::overwrite_existing |
			std::filesystem::copy_options::recursive;

		auto c = reviews.GetItemCount();
		for (decltype(c) i = 0; i < c; i++)
		{
			if (!reviews.GetCheck(i)) continue;

			auto op = std::filesystem::path((LPCTSTR)reviews.GetItemText(i, c_col.c_op));
			auto from = std::filesystem::path((LPCTSTR)reviews.GetItemText(i, c_col.c_from));
			auto to = std::filesystem::path((LPCTSTR)reviews.GetItemText(i, c_col.c_to));

			try
			{
				std::filesystem::create_directories(to.parent_path());

				if (op == L"移動")
				{
					std::filesystem::copy(from, to, options);
					std::filesystem::remove_all(from);
				}
				else if (op == L"コピー")
				{
					std::filesystem::copy(from, to, options);
				}

				hive.results.emplace_back(TRUE, from, to);
			}
			catch (...)
			{
				hive.results.emplace_back(FALSE, from, to);
			}
		}

		if (hive.create_dark_exe)
		{
			auto dark_from = hive.aviutl_file_name;
			auto dark_to = hive.aviutl_dir / L"aviutl_dark.exe";
			auto manifest_from = hive.module_folder_name / L"assets" / L"aviutl_dark.exe.manifest";
			auto manifest_to = hive.aviutl_dir / L"aviutl_dark.exe.manifest";

			BOOL succeeded = FALSE;

			try
			{
				std::filesystem::copy(dark_from, dark_to, options);

				auto resource = ::BeginUpdateResource(dark_to.c_str(), FALSE);

				// aviutl_dark.exeを外部マニフェスト形式にします。
				::UpdateResource(
					resource,
					RT_MANIFEST,
					MAKEINTRESOURCE(1),
					MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
					0, 0);
				std::filesystem::copy(manifest_from, manifest_to, options);

				::EndUpdateResource(resource, FALSE);

				succeeded = TRUE;
			}
			catch (...)
			{
			}

			hive.results.emplace_back(succeeded, dark_from, dark_to);
			hive.results.emplace_back(succeeded, manifest_from, manifest_to);
		}

		return TRUE;
	}

	//
	// メンバ変数をコントロールにバインドします。
	//
	virtual void DoDataExchange(CDataExchange* dx) override
	{
		__super::DoDataExchange(dx);

		DDX_Control(dx, IDC_REVIEWS, reviews);
	}

	//
	// ダイアログを初期化します。
	//
	virtual BOOL OnInitDialog() override
	{
		__super::OnInitDialog();

		CRect rc; reviews.GetClientRect(&rc);
		reviews.SetExtendedStyle(reviews.GetExtendedStyle() | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
		reviews.InsertColumn(c_col.c_op, _T("処理"), LVCFMT_LEFT, 80);
		reviews.InsertColumn(c_col.c_from, _T("変更前"), LVCFMT_LEFT, rc.Width() / 2);
		reviews.InsertColumn(c_col.c_to, _T("変更後"), LVCFMT_LEFT, rc.Width());

		auto review_index = 0;

		try
		{
			std::ifstream ifs(hive.spec_file_name);
			ptree root;
			read_json(ifs, root);

			if (hive.uninstall_old_version)
			{
				auto op = std::wstring(L"移動");

				if (auto rename_nodes_op = root.get_child_optional("rename"))
				{
					for (const auto& pair : rename_nodes_op.value())
					{
						const auto& rename_node = pair.second;

						std::filesystem::path from;
						get_file_name(rename_node, "from", from);
						if (from.empty()) continue;

						std::filesystem::path to;
						get_file_name(rename_node, "to", to);
						if (to.empty()) continue;

						from = hive.aviutl_dir / from;
						to = hive.aviutl_dir / to;

						from = from.make_preferred();
						to = to.make_preferred();

						if (!std::filesystem::exists(from)) continue;

						insert_review(review_index++, op, from, to);
					}
				}

				if (auto erase_nodes_op = root.get_child_optional("erase"))
				{
					for (const auto& pair : erase_nodes_op.value())
					{
						const auto& erase_node = pair.second;

						std::wstring path;
						get_string(erase_node, "path", path);
						if (path.empty()) continue;

						auto from = hive.aviutl_dir / path;
						auto to = hive.aviutl_dir / L"アンインストール済み" / path;

						from = from.make_preferred();
						to = to.make_preferred();

						if (!std::filesystem::exists(from)) continue;

						insert_review(review_index++, op, from, to);
					}
				}
			}

			if (hive.install_new_version)
			{
				auto op = std::wstring(L"コピー");

				if (auto deploy_nodes_op = root.get_child_optional("deploy"))
				{
					for (const auto& pair : deploy_nodes_op.value())
					{
						const auto& deploy_node = pair.second;

						std::filesystem::path from;
						get_file_name(deploy_node, "from", from);
						if (from.empty()) continue;

						std::filesystem::path to;
						get_file_name(deploy_node, "to", to);
						if (to.empty()) continue;

						from = std::filesystem::current_path() / from;
						to = hive.aviutl_dir / to;

						from = from.make_preferred();
						to = to.make_preferred();

						if (!std::filesystem::exists(from)) continue;

						insert_review(review_index++, op, from, to);
					}
				}
			}

			if (hive.deploy_runtime)
			{
				auto op = std::wstring(L"コピー");
				auto dir = hive.module_folder_name / L"runtime";

				for (auto it : std::filesystem::directory_iterator(dir))
				{
					auto from = it.path();
					auto to = hive.aviutl_dir / it.path().filename();

					insert_review(review_index++, op, from, to);
				}
			}
		}
		catch (const std::exception& error)
		{
			AfxMessageBox(std::format(
				L"{}を読込中にエラーが発生しました" L"\n"
				L"{}",
				hive.spec_file_name,
				my::cp_to_wide(error.what(), CP_ACP)).c_str());

			return FALSE;
		}

		return TRUE;
	}

	//
	// OKボタンが押されたときの処理です。
	//
	virtual void OnOK() override
	{
		// レビューが適用できた場合は
		if (apply_review())
		{
			// 結果ダイアログを表示します。
			ResultDialog dialog;
			dialog.DoModal();
		}

		// レビューダイアログを閉じます。
		__super::OnOK();
	}

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(ReviewDialog, CDialogEx)
END_MESSAGE_MAP()
