#pragma once

namespace apn::item_copy
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : AddinDialogInterface
	{
		//
		// シェルブラウザです。
		//
		wil::com_ptr_nothrow<ShellBrowser> shell_browser;

		//
		// シェルビューです。
		//
		wil::com_ptr_nothrow<IShellView> shell_view;

		//
		// シェルビューのウィンドウです。
		//
		HWND shell_view_window = nullptr;

		//
		// シェルビューを作成します。
		//
		BOOL init_shell_view()
		{
			// フォルダのパスを取得します。
			auto folder_name = magi.get_config_file_name(hive.c_name);

			// 表示するフォルダのITEMIDLISTを取得します。
			wil::unique_cotaskmem_ptr<ITEMIDLIST_ABSOLUTE> folder_idl;
			::SHParseDisplayName(folder_name.c_str(), nullptr, wil::out_param(folder_idl), 0, nullptr);
			if (!folder_idl) return FALSE;

			// デスクトップのシェルフォルダを取得します。
			wil::com_ptr_nothrow<IShellFolder> desktop_shell_folder;
			::SHGetDesktopFolder(&desktop_shell_folder);
			if (!desktop_shell_folder) return FALSE;

			// シェルフォルダを取得します。
			wil::com_ptr_nothrow<IShellFolder> shell_folder;
			desktop_shell_folder->BindToObject(folder_idl.get(), nullptr, IID_PPV_ARGS(&shell_folder));
			if (!shell_folder) return FALSE;

			// シェルビューを作成します。
			shell_folder->CreateViewObject(nullptr, IID_PPV_ARGS(&shell_view));
			if (!shell_view) return FALSE;

			// シェルブラウザを作成します。
			shell_browser.attach(ShellBrowser::Allocator::create(*this));

			// シェルビューウィンドウを作成します。
			FOLDERSETTINGS folder_settings = {
				.ViewMode = (UINT)FVM_AUTO, //FVM_DETAILS,
				.fFlags = FWF_SHOWSELALWAYS,
			};
			auto rc = RECT {};
			auto hr = shell_view->CreateViewWindow(nullptr,
				&folder_settings, shell_browser.get(), &rc, &shell_view_window);
			MY_TRACE_STR(my::get_error_message(hr));
			if (!shell_view_window) return FALSE;

			shell_view->UIActivate(SVUIA_ACTIVATE_NOFOCUS);

			return TRUE;
		}

		//
		// シェルビューを終了します。
		//
		BOOL exit_shell_view()
		{
			if (!shell_view) return FALSE;

			if (shell_view_window)
				shell_view->DestroyViewWindow();
			shell_view.reset();

			return TRUE;
		}

		//
		// 選択されているexoファイルを返します。
		//
		std::vector<std::wstring> get_selection()
		{
			if (!shell_view) return {};

			// データオブジェクトを取得します。
			wil::com_ptr_nothrow<IDataObject> data_object;
			shell_view->GetItemObject(SVGIO_SELECTION, IID_PPV_ARGS(&data_object));
			if (!data_object) return {};

			// ドロップファイルデータを取得します。
			FORMATETC formatetc = {};
			formatetc.cfFormat = CF_HDROP;
			formatetc.ptd      = nullptr;
			formatetc.dwAspect = DVASPECT_CONTENT;
			formatetc.lindex   = -1;
			formatetc.tymed    = TYMED_HGLOBAL;
			STGMEDIUM medium = {};
			auto hr = data_object->GetData(&formatetc, &medium);
			auto handle = (HDROP)medium.hGlobal;

			std::vector<std::wstring> selection;

			auto c = ::DragQueryFileW(handle, -1, nullptr, 0);
			for (decltype(c) i = 0; i < c; i++) {
				auto& file_name = selection.emplace_back(MAX_PATH, L'\0');
				::DragQueryFileW(handle, i, file_name.data(), file_name.size());
				file_name.resize(::lstrlenW(file_name.data()));
			}
			::DragFinish(handle);

			return selection;
		}

		BOOL set_selection(const std::wstring& file_name)
		{
			// ファイル名だけを取得します。
			auto file_spec = std::filesystem::path(file_name).filename();

			// フォルダのパスを取得します。
			auto folder_name = magi.get_config_file_name(hive.c_name);

			// 表示するフォルダのITEMIDLISTを取得します。
			wil::unique_cotaskmem_ptr<ITEMIDLIST_ABSOLUTE> folder_idl;
			::SHParseDisplayName(folder_name.c_str(), nullptr, wil::out_param(folder_idl), 0, nullptr);
			if (!folder_idl) return FALSE;

			// デスクトップのシェルフォルダを取得します。
			wil::com_ptr_nothrow<IShellFolder> desktop_shell_folder;
			::SHGetDesktopFolder(&desktop_shell_folder);
			if (!desktop_shell_folder) return FALSE;

			// シェルフォルダを取得します。
			wil::com_ptr_nothrow<IShellFolder> shell_folder;
			desktop_shell_folder->BindToObject(folder_idl.get(), nullptr, IID_PPV_ARGS(&shell_folder));
			if (!shell_folder) return FALSE;

			// ITEMIDLISTを取得します。
			wil::unique_cotaskmem_ptr<ITEMIDLIST> file_spec_idl;
			shell_folder->ParseDisplayName(
				nullptr,
				nullptr,
				file_spec.wstring().data(),
				nullptr,
				wil::out_param(file_spec_idl),
				nullptr);
			if (!file_spec_idl) return FALSE;

			// ファイルを選択します。
			auto hr = shell_view->SelectItem(
				file_spec_idl.get(),
				SVSI_DESELECTOTHERS |
				SVSI_ENSUREVISIBLE |
				SVSI_FOCUSED |
				SVSI_KEYBOARDSELECT);

			return TRUE;
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_int(IDC_INSERT_LAYER, hive.insert_layer);
			get_text(IDC_PREFIX, hive.prefix);
			get_check(IDC_USE_EMPTY_TEXT, hive.use_empty_text);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_int(IDC_INSERT_LAYER, hive.insert_layer);
			set_text(IDC_PREFIX, hive.prefix);
			set_check(IDC_USE_EMPTY_TEXT, hive.use_empty_text);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto size_s = std::make_shared<RelativePos>(base_size * 3);
			auto size_m = std::make_shared<RelativePos>(base_size * 4);
			auto size_l = std::make_shared<RelativePos>(base_size * 6);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_PASTE_ITEM));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_COPY_ITEM));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_READ_FILE));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_WRITE_FILE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_INSERT_LAYER_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_INSERT_LAYER));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_s, margin, ctrl(IDC_PREFIX_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_m, margin, ctrl(IDC_PREFIX));
				node->add_pane(c_axis.c_horz, c_align.c_left, size_l, margin, ctrl(IDC_USE_EMPTY_TEXT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, full);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, shell_view_window);
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, id, control);

			switch (id)
			{
			// ボタン
			case IDC_PASTE_ITEM: app->paste_item(); break;
			case IDC_COPY_ITEM: app->copy_item(); break;
			case IDC_READ_FILE: app->read_file(); break;
			case IDC_WRITE_FILE: app->write_file(); break;

			// チェックボックス
			case IDC_USE_EMPTY_TEXT:
				update_config(); break;

			// エディットボックス
			case IDC_PREFIX:
			case IDC_INSERT_LAYER:
				if (code == EN_UPDATE) update_config(); break;
			}
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					// シェルビューを作成します。
					if (!init_shell_view()) return -1;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// シェルビューを終了します。
					exit_shell_view();

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, {:#010x}, {:#010x}", wParam, lParam);

					// シェルビューにフォーカスを渡します。
					if (shell_view)
						shell_view->UIActivate(SVUIA_ACTIVATE_FOCUS);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
