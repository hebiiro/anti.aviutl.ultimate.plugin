#pragma once

namespace apn::dark
{
	//
	// このクラスはHive::Appの実装です。
	//
	inline struct App : Hive::App, my::FileUpdateChecker2::Handler
	{
		//
		// 現在のスキンIDです。
		//
		uint32_t current_skin_id = 0;

		//
		// TRUEの場合は初期化が完了しています。
		//
		BOOL is_initialized = FALSE;

		//
		// コンフィグの変更を監視します。
		//
		my::FileUpdateChecker2 config_checker;

		//
		// コンストラクタです。
		//
		App()
		{
			MY_TRACE_FUNC("");

			hive.app = this;
		}

		//
		// このアドインの初期化処理を実行します。
		//
		virtual BOOL init() override
		{
			MY_TRACE_FUNC("");

			if (is_initialized) return FALSE;
			is_initialized = TRUE;

			// Pyhtonの使用を開始します。
			if (!python.init()) return FALSE;

			// テーマレンダラの利用を開始します。
			if (!theme::client.init()) return FALSE;

			// スキンを開始します。
			if (!skin::manager.init()) return FALSE;

			// コンフィグの監視を開始します。
			config_checker.init(hive.config_file_name.c_str(), hive.main_window, this);

			// コンフィグを読み込みます。
			config_io.read();

			// アセットを読み込みます。
			assets_io.read();

			// ダイアログのスキンリストコントロールを更新します。
			addin_dialog.update_skin_list_control();

			// ダイアログのコントロールを更新します。
			addin_dialog.update_controls();

			// スキンを立ち上げます。
			boot_skin();

			// 再描画します。
			redraw();

			{
				// アセットフォルダを監視するスレッドを作成します。
				DWORD tid = 0;
				::CreateThread(nullptr, 0,
					[](LPVOID param) -> DWORD
					{
						auto folder_name = hive.assets_folder_name;
						std::vector<BYTE> buffer(1024);
						DWORD returned = 0;
						my::handle::unique_ptr<> file(::CreateFile(
							folder_name.c_str(),
							FILE_LIST_DIRECTORY,
							FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
							nullptr,
							OPEN_EXISTING,
							FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
							nullptr));
						while (::ReadDirectoryChangesW(
							file.get(),
							buffer.data(), buffer.size(),
							TRUE,
							FILE_NOTIFY_CHANGE_FILE_NAME |
							FILE_NOTIFY_CHANGE_DIR_NAME |
							FILE_NOTIFY_CHANGE_SIZE |
							FILE_NOTIFY_CHANGE_LAST_WRITE,
							&returned,
							nullptr,
							nullptr))
						{
							MY_TRACE("アセットフォルダの変更を検出しました\n");

							auto fni = (FILE_NOTIFY_INFORMATION*)buffer.data();
							auto length = fni->FileNameLength / sizeof(WCHAR);
							std::wstring file_name(fni->FileName, length);
							if (file_name.find(L"__pycache__") == file_name.npos)
								app.post_change_assets();
						}

						return 0;
					},
					nullptr, 0, &tid);
				MY_TRACE_INT(tid);
			}

			return TRUE;
		}

		//
		// このアドインの後始末処理を実行します。
		//
		virtual BOOL exit() override
		{
			MY_TRACE_FUNC("");

			// テーマレンダラの利用を終了します。
			theme::client.exit();

			// スキンを終了します。
			skin::manager.exit();

			// Pyhtonの使用を終了します。
			python.exit();

			return TRUE;
		}

		//
		// スキンの設定を初期値に戻します。
		//
		virtual BOOL init_skin() override
		{
			MY_TRACE_FUNC("");

			skin::config.clear();
			skin::icon_manager.clear();
			skin::stuff_manager.clear();
			skin::theme::manager.clear();

			return TRUE;
		}

		//
		// スキンを更新します。
		//
		virtual BOOL update_skin() override
		{
			MY_TRACE_FUNC("");

			return python.call_update();
		}

		//
		// ダイアログコントロールが変更されたときに呼び出されます。
		//
		virtual BOOL on_change_controls() override
		{
			MY_TRACE_FUNC("");

			// ダイアログコントロールの値を元にコンフィグを更新します。
			addin_dialog.update_config();

			// コンフィグをファイルに書き込みます。
			// これにより、ダークモード化アドインを使用している外部プロセスも
			// コンフィグの変更を検出することができます。
			config_io.write();

			return TRUE;
		}

		//
		// コンフィグファイルが変更されたときに呼び出されます。
		//
		virtual BOOL on_change_config() override
		{
			MY_TRACE_FUNC("");

			// コンフィグを読み込みます。
			config_io.read();

			// ダイアログのコントロールを更新します。
			addin_dialog.update_controls();

			// スキンを立ち上げます。
			boot_skin();

			// 再描画します。
			redraw();

			return TRUE;
		}

		//
		// アセットファイルが変更されたときに呼び出されます。
		//
		virtual BOOL on_change_assets() override
		{
			MY_TRACE_FUNC("");

			// アセットを読み込みます。
			assets_io.read();

			// ダイアログのスキンリストコントロールを更新します。
			addin_dialog.update_skin_list_control();

			// ダイアログのコントロールを更新します。
			addin_dialog.update_controls();

			// スキンを立ち上げます。
			boot_skin();

			// 再描画します。
			redraw();

			return TRUE;
		}

		//
		// アセットの更新を予約します。
		//
		BOOL post_change_assets()
		{
			MY_TRACE_FUNC("");

			// サブスレッドからタイマーを開始します。
			::SetTimer(hive.main_window, (UINT_PTR)this, 1000,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
				{
					MY_TRACE_FUNC("");

					// メインスレッドでタイマーを終了します。
					::KillTimer(hwnd, timer_id);

					// アセットの更新を適用します。
					hive.app->on_change_assets();
				});

			return TRUE;
		}

		//
		// 表示名からモジュール名を取得して返します。
		//
		auto get_module_name(const auto& list, const auto& display_name)
		{
			auto it = std::find_if(list.begin(), list.end(),
				[display_name](const auto& node) { return node->display_name == display_name; });
			return it != list.end() ? (*it)->module_name : std::wstring();
		}

		//
		// スキンを読み込みます。
		//
		BOOL boot_skin()
		{
			init_skin();

			// 現在のスキンの表示名からモジュール名を取得します。
			auto skin_module_name = get_module_name(hive.skin_list, hive.skin_name);
			if (skin_module_name.empty())
			{
				std::wcout << std::format(L"{}スキンが見つかりませんでした", hive.skin_name) << std::endl;

				return FALSE;
			}

			// 現在のスキームの表示名からモジュール名を取得します。
			auto scheme_module_name = get_module_name(hive.scheme_list, hive.scheme_name);
			if (scheme_module_name.empty())
			{
				std::wcout << std::format(L"{}スキームが見つかりませんでした", hive.scheme_name) << std::endl;

				return FALSE;
			}

			// 各モジュールを読み込みます。
			python.import_modules(skin_module_name, scheme_module_name);

			// スキンによってスキンコンフィグが更新されたかもしれないので、
			// それを適用させます。
			skin::manager.apply_config();

			// スキンが更新されたことを判別できるように、IDも更新(インクリメント)します。
			current_skin_id++;

			return TRUE;
		}

		//
		// 現在のスキンのIDを返します。
		//
		virtual uint32_t get_current_skin_id() override
		{
			return current_skin_id;
		}

		//
		// プロセス内のすべてのウィンドウを再描画します。
		//
		virtual BOOL redraw() override
		{
			MY_TRACE_FUNC("");

			return ::EnumWindows(
				[](HWND hwnd, LPARAM lParam)
				{
					DWORD pid = 0;
					DWORD tid = ::GetWindowThreadProcessId(hwnd, &pid);

					if (pid == ::GetCurrentProcessId())
						redraw_window(hwnd);

					return TRUE;
				}, 0);
		}

		//
		// 指定されたウィンドウとその子ウィンドウを再描画します。
		//
		inline static BOOL redraw_window(HWND hwnd)
		{
//			MY_TRACE_FUNC("{:#010x}", hwnd);

			if (::GetWindowLong(hwnd, GWL_STYLE) & WS_CAPTION)
				skin::dwm.set_window_attribute(hwnd, hwnd == ::GetActiveWindow());

			::RedrawWindow(hwnd, 0, 0,
				RDW_ERASE | RDW_FRAME | RDW_INTERNALPAINT |
				RDW_INVALIDATE | RDW_ALLCHILDREN);
			return ::EnumChildWindows(hwnd,
				[](HWND hwnd, LPARAM lParam)
				{
					auto class_name = my::get_class_name(hwnd);

					if (class_name == TRACKBAR_CLASS)
					{
						// トラックバー用の処理です。
						::SendMessage(hwnd, WM_SETFOCUS, 0, 0);
					}
					else if (class_name == WC_BUTTON)
					{
						// ボタン用の処理です。
						auto icon = (HICON)::SendMessage(hwnd, BM_GETIMAGE, IMAGE_ICON, 0);
						::SendMessage(hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)icon);
					}
					else
					{
						redraw_window(hwnd);
					}

					return TRUE;
				}, 0);
		}

		//
		// この仮想関数はコンフィグファイルが変更されたときに呼び出されます。
		//
		virtual void on_file_update(my::FileUpdateChecker2* checker) override
		{
			on_change_config();
		}
	} app;
}
