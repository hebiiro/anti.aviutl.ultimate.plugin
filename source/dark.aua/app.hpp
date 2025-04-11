#pragma once

namespace apn::dark
{
	//
	// 出力ストリームにワイド文字列を書き込みます。
	//
	inline static auto& operator<<(std::ofstream& ofs, const std::wstring& s)
	{
		return ofs.write((LPCSTR)s.c_str(), s.length() * sizeof(wchar_t)), ofs;
	}

	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface, my::FileUpdateChecker2::Handler
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
		App() { app = this; }

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
						auto app = (App*)param;

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
								app->post_change_assets();
						}

						return 0;
					},
					this, 0, &tid);
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

			// コンフィグを書き込みます。
			config_io.write();

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
					app->on_change_assets();
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
				std::wcout << my::format(L"{/}スキンが見つかりませんでした", hive.skin_name) << std::endl;

				return FALSE;
			}

			// 現在のスキームの表示名からモジュール名を取得します。
			auto scheme_module_name = get_module_name(hive.scheme_list, hive.scheme_name);
			if (scheme_module_name.empty())
			{
				std::wcout << my::format(L"{/}スキームが見つかりませんでした", hive.scheme_name) << std::endl;

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
//			MY_TRACE_FUNC("{/hex}", hwnd);

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

		inline static auto& write(std::ofstream& ofs, const std::wstring& s)
		{
			return ofs.write((LPCSTR)s.c_str(), s.length() * sizeof(wchar_t));
		}

		inline static std::wstring to_string(COLORREF color)
		{
#if 0
			return my::format(LR"(#{:1x}{:1x}{:1x})",
				GetRValue(color) >> 4, GetGValue(color) >> 4, GetBValue(color) >> 4);
#else
			return my::format(LR"(#{:02x}{:02x}{:02x})",
				GetRValue(color), GetGValue(color), GetBValue(color));
#endif
		}

		inline static std::wstring to_string(LPCWSTR name, LPCWSTR vsclass_name, int part_id, int state_id)
		{
			auto state = skin::theme::manager.get_state(vsclass_name, part_id, state_id);
			if (!state) return {};
			return my::format(LR"(<NamedColor name="{/}" fillColor="{/}" edgeColor="{/}" textForeColor="{/}" textBackColor="{/}" />)",
				name,
				to_string(state->stuff.fill.color),
				to_string(state->stuff.border.color),
				to_string(state->stuff.text.color),
				to_string(state->stuff.text.shadow_color));
		}

		//
		// 黒窓の設定ファイルを構築します。
		//
		virtual BOOL deploy_darken_window_settings() override
		{
			MY_TRACE_FUNC("");

			auto root_path = my::get_module_file_name(nullptr).parent_path();
			auto plugins_path = root_path / L"plugins";
			auto darken_window_path = plugins_path / L"DarkenWindow";
			auto darken_window_settings_stem = L"DarkenWindowSettings.xml";
			auto darken_window_settings_path = darken_window_path / darken_window_settings_stem;
			auto skin_name = L"dummy"s;
			auto skin_stem = skin_name + L".xml";
			auto skin_relative_path = std::filesystem::path(L"Skin") / skin_stem;
			auto skin_path = darken_window_path / skin_relative_path;
			auto skin_settings_stem = L"_Settings.xml"s;
			auto skin_settings_relative_path = std::filesystem::path(skin_name) / skin_settings_stem;
			auto skin_settings_path = darken_window_path / L"Skin" / skin_settings_relative_path;

			// 黒窓フォルダを作成します。
			std::filesystem::create_directories(skin_settings_path.parent_path());

			// 設定ファイルを作成します。
			{
				std::ofstream ofs(darken_window_settings_path, std::ios::out | std::ios::binary);
				ofs << L"\ufeff"s << my::format(LR"(<?xml version="1.0" encoding="UTF-16" standalone="no"?>)");
				ofs << L"\r\n"s << my::format(LR"(<Settings skin="{/}" shadowMode="{/}" roundMode="{/}" />)",
					skin_relative_path.c_str(),
					hive.draw_shadow ? L"ON" : L"OFF",
					hive.as_round ? L"ON" : L"OFF");
			}

			// スキンファイルを作成します。
			{
				std::ofstream ofs(skin_path, std::ios::out | std::ios::binary);
				ofs << L"\ufeff"s << my::format(LR"(<?xml version="1.0" encoding="UTF-16" standalone="no"?>)");
				ofs << L"\r\n"s << my::format(LR"(<Settings>)");
				ofs << L"\r\n\t"s << my::format(LR"(<Skin fileName="{/}" />)", skin_settings_relative_path.c_str());
				ofs << L"\r\n"s << my::format(LR"(</Settings>)");
			}

			// スキン設定ファイルを作成します。
			{
				std::ofstream ofs(skin_settings_path, std::ios::out | std::ios::binary);
				ofs << L"\ufeff"s << my::format(LR"(<?xml version="1.0" encoding="UTF-16" standalone="no"?>)");
				ofs << L"\r\n"s << my::format(LR"(<Skin>)");
				ofs << L"\r\n\t"s << my::format(LR"(<Attributes>)");
				ofs << L"\r\n\t\t"s << my::format(LR"(<Dwm)");
				ofs << L"\r\n\t\t\t"s << my::format(LR"(activeBorderColor="{/}" activeCaptionColor="{/}" activeTextColor="{/}")",
					to_string(skin::dwm.active.border_color),
					to_string(skin::dwm.active.caption_color),
					to_string(skin::dwm.active.text_color));
				ofs << L"\r\n\t\t\t"s << my::format(LR"(inactiveBorderColor="{/}" inactiveCaptionColor="{/}" inactiveTextColor="{/}")",
					to_string(skin::dwm.inactive.border_color),
					to_string(skin::dwm.inactive.caption_color),
					to_string(skin::dwm.inactive.text_color));
				ofs << L"\r\n\t\t\t"s << my::format(LR"(darkMode="{/}" cornerMode="{/}")", skin::dwm.dark_mode, skin::dwm.corner_mode);
				ofs << L"\r\n\t\t"s << my::format(LR"(/>)");
				ofs << L"\r\n\t\t"s << my::format(LR"(<NamedColors>)");

				ofs << L"\r\n\t\t\t"s << to_string(L"caption_active", VSCLASS_WINDOW, WP_CAPTION, CS_ACTIVE);
				ofs << L"\r\n\t\t\t"s << to_string(L"caption_inactive", VSCLASS_WINDOW, WP_CAPTION, CS_INACTIVE);
				ofs << L"\r\n\t\t\t"s << to_string(L"caption_disabled", VSCLASS_WINDOW, WP_CAPTION, CS_DISABLED);

				ofs << L"\r\n\t\t\t"s << to_string(L"border_active", VSCLASS_WINDOW, WP_SEPARATOR, CS_ACTIVE);
				ofs << L"\r\n\t\t\t"s << to_string(L"border_inactive", VSCLASS_WINDOW, WP_SEPARATOR, CS_INACTIVE);
				ofs << L"\r\n\t\t\t"s << to_string(L"border_disabled", VSCLASS_WINDOW, WP_SEPARATOR, CS_DISABLED);

				ofs << L"\r\n\t\t\t"s << to_string(L"dialog", VSCLASS_WINDOW, WP_DIALOG, PBS_NORMAL);

				ofs << L"\r\n\t\t\t"s << to_string(L"c1_normal", VSCLASS_EDIT, EP_EDITTEXT, ETS_NORMAL);
				ofs << L"\r\n\t\t\t"s << to_string(L"c1_hot", VSCLASS_EDIT, EP_EDITTEXT, ETS_HOT);
				ofs << L"\r\n\t\t\t"s << to_string(L"c1_selected", VSCLASS_EDIT, EP_EDITTEXT, ETS_SELECTED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c1_disabled", VSCLASS_EDIT, EP_EDITTEXT, ETS_DISABLED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c1_readonly", VSCLASS_EDIT, EP_EDITTEXT, ETS_READONLY);
				ofs << L"\r\n\t\t\t"s << to_string(L"c1_statictext", VSCLASS_STATIC, STAT_TEXT, PBS_NORMAL);

				ofs << L"\r\n\t\t\t"s << to_string(L"c2_normal", VSCLASS_BUTTON, BP_PUSHBUTTON, PBS_NORMAL);
				ofs << L"\r\n\t\t\t"s << to_string(L"c2_hot", VSCLASS_BUTTON, BP_PUSHBUTTON, PBS_HOT);
				ofs << L"\r\n\t\t\t"s << to_string(L"c2_selected", VSCLASS_BUTTON, BP_PUSHBUTTON, PBS_PRESSED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c2_disabled", VSCLASS_BUTTON, BP_PUSHBUTTON, PBS_DISABLED);

				ofs << L"\r\n\t\t\t"s << to_string(L"c3_normal", VSCLASS_SCROLLBAR, SBP_THUMBBTNHORZ, SCRBS_NORMAL);
				ofs << L"\r\n\t\t\t"s << to_string(L"c3_hover", VSCLASS_SCROLLBAR, SBP_THUMBBTNHORZ, SCRBS_HOVER);
				ofs << L"\r\n\t\t\t"s << to_string(L"c3_hot", VSCLASS_SCROLLBAR, SBP_THUMBBTNHORZ, SCRBS_HOT);
				ofs << L"\r\n\t\t\t"s << to_string(L"c3_selected", VSCLASS_SCROLLBAR, SBP_THUMBBTNHORZ, SCRBS_PRESSED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c3_disabled", VSCLASS_SCROLLBAR, SBP_THUMBBTNHORZ, SCRBS_DISABLED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c3_gutter", VSCLASS_TRACKBAR, TKP_TRACK, TRS_NORMAL);
				ofs << L"\r\n\t\t\t"s << to_string(L"c3_track", VSCLASS_TRACKBAR, TKP_TRACK, TRS_BACKGROUND);

				ofs << L"\r\n\t\t\t"s << to_string(L"c4_normal", VSCLASS_MENU, MENU_BARITEM, MBI_NORMAL);
				ofs << L"\r\n\t\t\t"s << to_string(L"c4_hot", VSCLASS_MENU, MENU_BARITEM, MBI_HOT);
				ofs << L"\r\n\t\t\t"s << to_string(L"c4_selected", VSCLASS_MENU, MENU_BARITEM, MBI_PUSHED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c4_disabled", VSCLASS_MENU, MENU_BARITEM, MBI_DISABLED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c4_disabled_hot", VSCLASS_MENU, MENU_BARITEM, MBI_DISABLEDHOT);
				ofs << L"\r\n\t\t\t"s << to_string(L"c4_disabled_selected", VSCLASS_MENU, MENU_BARITEM, MBI_DISABLEDPUSHED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c4_gutter", VSCLASS_MENU, MENU_POPUPGUTTER, 0);
				ofs << L"\r\n\t\t\t"s << to_string(L"c4_separator", VSCLASS_MENU, MENU_POPUPSEPARATOR, 0);

				ofs << L"\r\n\t\t\t"s << to_string(L"c5_normal", VSCLASS_STATIC, STAT_TEXT, PBS_NORMAL);
				ofs << L"\r\n\t\t\t"s << to_string(L"c5_hot", VSCLASS_BUTTON, BP_PUSHBUTTON, PBS_HOT);
				ofs << L"\r\n\t\t\t"s << to_string(L"c5_selected", VSCLASS_BUTTON, BP_PUSHBUTTON, PBS_PRESSED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c5_disabled", VSCLASS_STATIC, STAT_TEXT, PBS_DISABLED);
				ofs << L"\r\n\t\t\t"s << to_string(L"c5_checked", VSCLASS_BUTTON, BP_CHECKBOX, CBS_CHECKEDNORMAL);

				ofs << L"\r\n\t\t"s << my::format(LR"(</NamedColors>)");
				ofs << L"\r\n\t"s << my::format(LR"(</Attributes>)");
				ofs << L"\r\n"s << my::format(LR"(</Skin>)");
			}

			return TRUE;
		}

		//
		// 与えられたテーマが同一のレンダラーを使用する場合はTRUEを返します。
		//
		virtual BOOL equal(HTHEME theme1, HTHEME theme2) override
		{
			return theme::manager.get_renderer(theme1) == theme::manager.get_renderer(theme2);
		}

		//
		// この仮想関数はコンフィグファイルが変更されたときに呼び出されます。
		//
		virtual void on_file_update(my::FileUpdateChecker2* checker) override
		{
			on_change_config();
		}
	} app_impl;
}
