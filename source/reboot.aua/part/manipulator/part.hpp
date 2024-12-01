#pragma once

namespace apn::reboot::manipulator
{
	//
	// このクラスはマニピュレータのパートです。
	//
	inline struct Reloader : Part
	{
		//
		// このパートの名前を返します。
		//
		virtual std::wstring on_get_name()
		{
			return L"manipulator";
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_read(n_json& root) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL on_write(n_json& root) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// パートの状態を更新します。
		//
		virtual BOOL on_update(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コマンドを実行します。
		//
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, control_id, control);

			switch (control_id)
			{
			case IDC_MANIPULATOR_WRITE: return project::write();
			case IDC_MANIPULATOR_WRITE_AS: return project::write_as();
			case IDC_MANIPULATOR_OPEN: return project::open();
			case IDC_MANIPULATOR_CLOSE: return project::close();
			case IDC_MANIPULATOR_OPEN_AVIUTL_FOLDER: return open_aviutl_folder();
			case IDC_MANIPULATOR_SHOW_PSD_TOOL_KIT: return show_psd_tool_kit();
			}

			return FALSE;
		}

		//
		// オプションを変更します。
		//
		virtual BOOL on_option(AddinDialogInterface* addin_dialog, uint32_t control_id, HWND control, const POINT& point) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}", control_id, control);

			return TRUE;
		}

		//
		// aviutlの実行ファイルがあるフォルダを開きます。
		//
		BOOL open_aviutl_folder()
		{
			MY_TRACE_FUNC("");

			// aviutlの実行ファイルがあるフォルダのパスを取得します。
			auto folder_name = my::get_module_file_name(nullptr).parent_path();

			// フォルダを開きます。
			return share::shell_execute(_T("open"), folder_name.c_str(), nullptr);
		}

		//
		// PSDToolKitウィンドウを表示します。
		//
		BOOL show_psd_tool_kit()
		{
			MY_TRACE_FUNC("");

			auto aviutl_window = hive.aviutl_window;
			auto menu = ::GetMenu(aviutl_window);
			auto edit_menu = find_menu(menu, _T("編集"));
			auto psd_tool_kit_menu = find_menu(edit_menu, _T("PSDToolKit"));
			auto id = find_menu_item(psd_tool_kit_menu, _T("ウィンドウを表示"));
			if (!id) return FALSE;
			return ::PostMessage(aviutl_window, WM_COMMAND, id, 0);
		}
	} part;
}
