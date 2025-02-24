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

			// LFHなどは既に有効になっているはずなので何もしません。
			//optimize_initial_heap();

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

			read_bool(root, "optimize_heap", agit.optimize_heap);
			read_bool(root, "reset_workingset_size", agit.reset_workingset_size);

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL on_write(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_bool(root, "optimize_heap", agit.optimize_heap);
			write_bool(root, "reset_workingset_size", agit.reset_workingset_size);

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
			case IDC_MANIPULATOR_PLAY: return project::play();
			case IDC_MANIPULATOR_OPEN_AVIUTL_FOLDER: return open_aviutl_folder();
			case IDC_MANIPULATOR_SHOW_PSD_TOOL_KIT: return show_psd_tool_kit();
			case IDC_MANIPULATOR_OPTIMIZE: return optimize();
			}

			return FALSE;
		}

		//
		// オプションを変更します。
		//
		virtual BOOL on_option(AddinDialogInterface* addin_dialog, uint32_t control_id, HWND control, const POINT& point) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}", control_id, control);

			// このパート用のコントロールではない場合は何もしません。
			if (control_id != IDC_MANIPULATOR_OPTIMIZE) return FALSE;

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_optimize_heap = 3000;
			constexpr uint32_t c_reset_workingset_size = 3001;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// optimize_heapを追加します。
			::AppendMenu(menu.get(), MF_STRING, c_optimize_heap, _T("ヒープを最適化する"));
			if (agit.optimize_heap) ::CheckMenuItem(menu.get(), c_optimize_heap, MF_CHECKED);

			// reset_workingset_sizeを追加します。
			::AppendMenu(menu.get(), MF_STRING, c_reset_workingset_size, _T("ワーキングセットサイズをリセットする"));
			if (agit.reset_workingset_size) ::CheckMenuItem(menu.get(), c_reset_workingset_size, MF_CHECKED);

			// ポップアップメニューを表示します。
			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, control, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_optimize_heap: agit.optimize_heap = !agit.optimize_heap; break;
			case c_reset_workingset_size: agit.reset_workingset_size = !agit.reset_workingset_size; break;
			}

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

		//
		// 初期ヒープを最適化します。
		//
		BOOL optimize_initial_heap()
		{
			MY_TRACE_FUNC("");

			// ヒープを取得します。
			auto heap = ::GetProcessHeap();

			auto current_hetoc = ULONG {};
			auto r4 = ::HeapQueryInformation(nullptr, HeapEnableTerminationOnCorruption, &current_hetoc, sizeof(current_hetoc), nullptr);

			auto current_hci = ULONG {};
			auto r5 = ::HeapQueryInformation(heap, HeapCompatibilityInformation, &current_hci, sizeof(current_hci), nullptr);

			// 不明な処理ですが、サンプルコードと同じようにしています。
			auto r1 = ::HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);

			// 低断片化ヒープ(LFH)を有効にします。
			auto hci = ULONG { 2 };
			auto r2 = ::HeapSetInformation(heap, HeapCompatibilityInformation, &hci, sizeof(hci));

			return r1 && r2;
		}

		//
		// ヒープを最適化します。
		//
		BOOL optimize_heap()
		{
			MY_TRACE_FUNC("");

			// ヒープを最適化します。
			// 可能な範囲でメモリのコミットが解除されます。
			auto hori = HEAP_OPTIMIZE_RESOURCES_INFORMATION {
				HEAP_OPTIMIZE_RESOURCES_CURRENT_VERSION
			};
			auto r3 = ::HeapSetInformation(nullptr, HeapOptimizeResources, &hori, sizeof(hori));

			return r3;
		}

		//
		// ワーキングセットサイズをリセットします。
		//
		BOOL reset_workingset_size()
		{
			MY_TRACE_FUNC("");

			return ::SetProcessWorkingSetSize(::GetCurrentProcess(), -1, -1);
		}

		//
		// aviutlを最適化します。
		//
		BOOL optimize()
		{
			MY_TRACE_FUNC("");

			auto result = FALSE;

			if (agit.optimize_heap) result |= optimize_heap();
			if (agit.reset_workingset_size) result |= reset_workingset_size();

			return result;
		}
	} part;
}
