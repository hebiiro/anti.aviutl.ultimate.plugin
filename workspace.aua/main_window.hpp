#pragma once

namespace apn::workspace
{
	//
	// このクラスはメインウィンドウです。
	// このウィンドウはシャトルをドッキングさせることができます。
	//
	struct MainWindow : DockSite<my::Window>
	{
		inline static constexpr struct CommandID {
			inline static constexpr struct Shuttle {
				inline static constexpr uint32_t c_begin = 100;
				inline static constexpr uint32_t c_end = 300;
			} c_shuttle;
			inline static constexpr uint32_t c_show_config_dialog = 1000;
			inline static constexpr uint32_t c_import_layout = 1001;
			inline static constexpr uint32_t c_export_layout = 1002;
			inline static constexpr uint32_t c_create_sub_window = 1003;

			inline static constexpr uint32_t c_maximum_play = 30000;
		} c_command_id;

		//
		// カラー選択ダイアログのカスタムカラーです。
		//
		inline static COLORREF custom_colors[16] = {};

		//
		// システムメニューのサブメニューです。
		//
		HMENU sub_menu[c_category.c_max_size] = {};

		//
		// コンストラクタです。
		//
		MainWindow()
		{
			// カラー選択ダイアログで使用するカスタムカラーを初期化します。
			std::fill(std::begin(custom_colors), std::end(custom_colors), RGB(0xff, 0xff, 0xff));
		}

		//
		// デストラクタです。
		//
		virtual ~MainWindow() override
		{
			MY_TRACE_FUNC("{:#010x}", (HWND)*this);
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			//「AoiSupport」用の処理です。
			// クラス名を"AviUtl"にして、AviUtlウィンドウに偽装します。
			static const auto c_class_name = _T("AviUtl");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = c_class_name;
			::RegisterClass(&wc);

			// メインウィンドウを作成します。
			return my::Window::create(
				0,
				c_class_name,
				hive.c_name,
				WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				0, 0, hive.instance, 0);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// シャトルを初期化します。
		//
		BOOL init_shuttle(HWND hwnd, const std::wstring& window_name)
		{
			// クラス名を取得します。取得できなかった場合は何もしません。
			auto class_name = my::get_class_name(hwnd);
			if (class_name.empty()) return FALSE;

			// ウィンドウ名を取得します。取得できなかった場合は何もしません。
			if (window_name.empty()) return FALSE;

			MY_TRACE_FUNC("{:#010x}, {}, {}", hwnd, class_name, window_name);

			if (hwnd == hive.aviutl_window)
			{
				if (*aviutl_window) return FALSE;

				MY_TRACE("AviUtlウィンドウ用のシャトルを初期化します\n");

				aviutl_window->init(_T("* AviUtl"), hwnd);

				return TRUE;
			}

			if (hwnd == hive.exedit_window)
			{
				if (*exedit_window) return FALSE;

				MY_TRACE("拡張編集ウィンドウ用のシャトルを初期化します\n");

				exedit_window->init(_T("* 拡張編集"), hwnd);

				return TRUE;
			}

			if (hwnd == hive.setting_dialog)
			{
				if (*setting_dialog) return FALSE;

				MY_TRACE("設定ダイアログ用のシャトルを初期化します\n");

				setting_dialog->init(_T("* 設定ダイアログ"), hwnd);

				// 最初のレイアウト計算を行う準備が整ったので、
				// メインウィンドウにメッセージをポストして通知します。
				::PostMessage(hive.main_window, hive.c_message.c_post_init, 0, 0);

				return TRUE;
			}

			if (class_name == _T("AviUtl"))
			{
				// 拡張編集ウィンドウのダミーかもしれないのでチェックしておきます。
				if (hwnd == exedit_window->dummy) return FALSE;

				MY_TRACE("「{}」用のシャトルを初期化します\n", window_name);

				auto shuttle = std::make_shared<Shuttle>();

				shuttle->init(window_name, hwnd);

				return TRUE;
			}

			return FALSE;
		}

		//
		// システムメニューを初期化します。
		//
		void init_system_menu()
		{
			MY_TRACE_FUNC("");

			auto menu = ::GetSystemMenu(*this, FALSE);
			auto index = 0;

			// サブメニューを作成し、システムメニューに追加します。
			for (size_t i = 0; i < std::size(sub_menu); i++)
			{
				sub_menu[i] = ::CreatePopupMenu();

				::InsertMenu(menu, index++, MF_BYPOSITION | MF_POPUP, (UINT)sub_menu[i], c_category.labels[i]);
			}

			::InsertMenu(menu, index++, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, c_command_id.c_create_sub_window, _T("サブウィンドウを新規作成"));
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, c_command_id.c_import_layout, _T("レイアウトのインポート"));
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, c_command_id.c_export_layout, _T("レイアウトのエクスポート"));
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_STRING, c_command_id.c_show_config_dialog, _T("ワークスペース化アドインの設定"));
			::InsertMenu(menu, index++, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
		}

		//
		// システムメニューのサブメニューを初期化します。
		//
		void init_system_menu_popup()
		{
			MY_TRACE_FUNC("");

			// 一旦すべての項目を削除します。
			for (auto& menu : sub_menu)
				while (::DeleteMenu(menu, 0, MF_BYPOSITION)){}

			// フローティング状態のシャトルをサブメニューに追加します。
			auto id = c_command_id.c_shuttle.c_begin;
			for (auto& pair : shuttle_manager.collection)
			{
				auto& shuttle = pair.second;

				// ドッキング状態のシャトルは除外します。
				if (shuttle->is_docking()) continue;

				// シャトルのカテゴリを取得します。
				auto category = get_category(shuttle.get());
				if (category == c_category.c_none) continue;

				// 追加先メニューを取得します。
				auto menu = sub_menu[category];

				// メニューアイテムを追加します。
				::AppendMenu(menu, MF_STRING, id, shuttle->name.c_str());

				// シャトルが表示されているなら
				if (::IsWindowVisible(*shuttle))
				{
					// メニューアイテムにチェックを入れます。
					::CheckMenuItem(menu, id, MF_CHECKED);
				}

				// メニューアイテムのIDをインクリメントします。
				id++;
			}
		}

		//
		// メインメニューを更新します。
		//
		void update_main_menu()
		{
			MY_TRACE_FUNC("");

			auto menu = ::GetMenu(*this);

			auto text = _T("再生時最大化 OFF");
			if (hive.maximum_play) text = _T("再生時最大化 ON");

			MENUITEMINFO mii = { sizeof(mii) };
			mii.fMask = MIIM_STRING;
			mii.dwTypeData = (LPTSTR)text;
			::SetMenuItemInfo(menu, c_command_id.c_maximum_play, FALSE, &mii);

			::DrawMenuBar(*this);
		}

		//
		// メインウィンドウのタイトル名を更新します。
		//
		void refresh_title()
		{
			MY_TRACE_FUNC("");

			auto orig = my::get_window_text(hive.aviutl_window);
			MY_TRACE_STR(orig);

			auto editp = magi.exin.get_editp();

			std::wstring title;
			if (editp && editp->frame_n)
			{
				auto file_name = std::filesystem::path(editp->project_filename).filename();
				if (file_name.empty()) file_name = _T("無題");
				title = std::format(_T("{} - {}"), file_name.c_str(), orig);
			}
			else
			{
				title = orig;
			}

			::SetWindowText(*this, title.c_str());
		}

		//
		// すべてのドッキングサイトのレイアウトを更新します。
		//
		void update_all_layouts()
		{
			MY_TRACE_FUNC("");

			update_layout(*this);

			for (auto& sub_window : SubWindow::collection)
				update_layout(*sub_window);
		}

		//
		// プリファレンスを読み込みます。
		//
		BOOL read_preference()
		{
			MY_TRACE_FUNC("");

			hive.app->read_preference();

			// メインメニューを更新します。
			update_main_menu();

			// レイアウトを更新します。
			update_all_layouts();

			// メインウィンドウが非表示なら表示します。
			if (!::IsWindowVisible(hwnd))
				::ShowWindow(hwnd, SW_SHOW);

			// メインウィンドウをフォアグラウンドにします。
			::SetForegroundWindow(hwnd);
			::SetActiveWindow(hwnd);

			return TRUE;
		}

		//
		// プリファレンスを保存します。
		//
		BOOL write_preference()
		{
			MY_TRACE_FUNC("");

			return hive.app->write_preference();
		}

		//
		// コンフィグダイアログを表示します。
		//
		int show_config_dialog(HWND parent)
		{
			ConfigDialog dialog;

			return dialog.do_modal(parent);
		}

		//
		// サブウィンドウを作成します。
		//
		BOOL create_sub_window(HWND parent)
		{
			auto name = SubWindow::get_available_name();
			MY_TRACE_STR(name);

			auto sub_window = std::make_shared<SubWindow>();
			if (sub_window->create(name, parent))
			{
				sub_window->init(name, *sub_window);
				::ShowWindowAsync(*sub_window, SW_SHOW);
			}

			return TRUE;
		}

		//
		// サブプロセス用のウィンドウを作成します。
		//
		BOOL create_sub_processes(HWND parent)
		{
			if (hive.psdtoolkit.dock) {
				const auto name = Hive::PSDToolKit::c_name;
				auto shuttle = psdtoolkit = std::make_shared<PSDToolKit>();
				shuttle->create(name, *this);
				shuttle->init(name, *shuttle);
			}

			if (hive.bouyomisan.dock) {
				const auto name = Hive::Bouyomisan::c_name;
				auto shuttle = bouyomisan = std::make_shared<Bouyomisan>();
				shuttle->create(name, *this);
				shuttle->init(name, *shuttle);
			}

			if (hive.console.dock) {
				const auto name = Hive::Console::c_name;
				auto shuttle = console = std::make_shared<Console>();
				shuttle->create(name, *this);
				shuttle->init(name, *shuttle);
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			case WM_DEADCHAR:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_SYSCHAR:
			case WM_SYSDEADCHAR:
				{
					MY_TRACE_FUNC("WM_***KEY***, {:#010x}, {:#010x}, {:#010x}", message, wParam, lParam);

					return ::SendMessage(hive.aviutl_window, message, wParam, lParam);
				}
			case WM_NCACTIVATE:
				{
					MY_TRACE_FUNC("WM_NCACTIVATE, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_ACTIVATE: // 「patch.aul」用の処理です。
				{
					MY_TRACE_FUNC("WM_ACTIVATE, {:#010x}, {:#010x}", wParam, lParam);

					if (LOWORD(wParam) == WA_INACTIVE)
						::SendMessage(hive.aviutl_window, message, wParam, lParam);

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_KILLFOCUS:
				{
					MY_TRACE_FUNC("WM_KILLFOCUS, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_MENUSELECT: // 「patch.aul」用の処理です。
				{
					MY_TRACE_FUNC("WM_MENUSELECT, {:#010x}, {:#010x}", wParam, lParam);

					::SendMessage(hive.aviutl_window, message, wParam, lParam);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, {:#010x}, {:#010x}", wParam, lParam);

					return ::SendMessage(hive.aviutl_window, message, wParam, lParam);
				}
			case WM_COMMAND:
				{
					MY_TRACE_FUNC("WM_COMMAND, {:#010x}, {:#010x}", wParam, lParam);

					auto id = LOWORD(wParam);

					switch (id)
					{
					case c_command_id.c_maximum_play:
						{
							MY_TRACE("c_command_id.c_maximum_play\n");

							hive.maximum_play = !hive.maximum_play;

							update_main_menu();

							return 0;
						}
					}

					// 「PSDToolKit」用の処理です。
					// WM_COMMAND終了時にマウスメッセージがPSDToolKitに飛ぶとフリーズします。
					if (psdtoolkit && ::IsWindowVisible(*psdtoolkit))
					{
						// PSDToolKitを非表示にしてマウスメッセージが飛ばないようにします。
						::ShowWindow(*psdtoolkit, SW_HIDE);

						// AviUtlにWM_COMMANDを処理させます。
						auto result = ::SendMessage(hive.aviutl_window, message, wParam, lParam);

						// 表示コマンドをポストします。
						// これにより、若干遅れてPSDToolKitが表示状態になります。
						::ShowWindowAsync(*psdtoolkit, SW_SHOW);

						return result;
					}

					return ::SendMessage(hive.aviutl_window, message, wParam, lParam);
				}
			case WM_SYSCOMMAND:
				{
					MY_TRACE_FUNC("WM_SYSCOMMAND, {:#010x}, {:#010x}", wParam, lParam);

					if (wParam >= c_command_id.c_shuttle.c_begin && wParam < c_command_id.c_shuttle.c_end)
					{
						auto menu = ::GetSystemMenu(hwnd, FALSE);
						auto id = (UINT)wParam;

						// メニューアイテムのテキストを取得します。
						auto text = my::get_menu_item_text(menu, id, MF_BYCOMMAND);
						MY_TRACE_STR(text);

						// テキストからシャトルを取得します。
						auto shuttle = shuttle_manager.get(text);
						if (!shuttle) break;

						// シャトルの表示状態を切り替えます。
						::SendMessage(*shuttle, WM_CLOSE, 0, 0);

						break;
					}

					switch (wParam)
					{
					case c_command_id.c_show_config_dialog:
						{
							// コンフィグダイアログを開きます。
							if (IDOK == show_config_dialog(hwnd))
							{
								// レイアウトを更新します。
								update_all_layouts();

								// 設定が変更された可能性があるので、
								// メインウィンドウのメニューを更新します。
								update_main_menu();
							}

							break;
						}
					case c_command_id.c_import_layout:
						{
							// レイアウトをインポートします。
							hive.app->import_layout();

							break;
						}
					case c_command_id.c_export_layout:
						{
							// レイアウトをエクスポートします。
							hive.app->export_layout();

							break;
						}
					case c_command_id.c_create_sub_window:
						{
							// サブウィンドウを新規作成します。
							create_sub_window(hwnd);

							break;
						}
					case SC_RESTORE:
					case SC_MINIMIZE: // 「patch.aul」用の処理です。
						{
							::SendMessage(hive.aviutl_window, message, wParam, lParam);

							break;
						}
					}

					break;
				}
			case WM_INITMENUPOPUP:
				{
					init_system_menu_popup();

					break;
				}
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE");

					hive.main_window = hwnd;
					MY_TRACE_HEX(hive.main_window);

					hive.theme.reset(::OpenThemeData(hwnd, VSCLASS_WINDOW));
					MY_TRACE_HEX(hive.theme.get());

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY");

					hive.theme.reset();

					break;
				}
			case hive.c_message.c_post_init: // AviUtlの初期化処理が終わったあとに通知されます。
				{
					MY_TRACE_FUNC("{:#010x}, c_post_init, {:#010x}, {:#010x}, c_begin", hwnd, wParam, lParam);

					// メインウィンドウが初期化された時点では
					// まだAviUtlのフォントが作成されていなかったので
					// ここでタブのフォントをセットします。
					get_root_pane(hwnd)->tab.set_font();

					// システムメニューに独自の項目を追加する。
					init_system_menu();

					// プリファレンスを読み込みます。
					read_preference();

					MY_TRACE_FUNC("{:#010x}, c_post_init, {:#010x}, {:#010x}, c_end", hwnd, wParam, lParam);

					// メインウィンドウのタイトルを更新します。
					refresh_title();

					break;
				}
			case hive.c_message.c_read_preference: // プリファレンスを読み込む必要があるときに通知されます。
				{
					// プリファレンスを読み込みます。
					read_preference();

					break;
				}
			case hive.c_message.c_write_preference: // プリファレンスを保存する必要があるときに通知されます。
				{
					// プリファレンスを保存します。
					write_preference();

					break;
				}
			case hive.c_message.c_init_shuttle: // シャトルを初期化するために通知されます。
				{
					auto hwnd = (HWND)wParam;
					auto orig = (Hive::OrigWindow*)lParam;

					// シャトルを初期化します。
					init_shuttle(hwnd, orig->window_name);

					// メモリを開放します。
					delete orig;

					break;
				}
			case hive.c_message.c_refresh_title: // タイトルを更新するために通知されます。
				{
					refresh_title();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// このクラスは設定を変更するためのダイアログです。
		//
		struct ConfigDialog : my::Dialog2
		{
			int do_modal(HWND parent)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_CONFIG), parent);

				set_uint(IDC_FILL_COLOR, hive.fill_color);
				set_uint(IDC_BORDER_COLOR, hive.border_color);
				set_uint(IDC_HOT_BORDER_COLOR, hive.hot_border_color);
				set_uint(IDC_ACTIVE_CAPTION_COLOR, hive.active_caption_color);
				set_uint(IDC_ACTIVE_CAPTION_TEXT_COLOR, hive.active_caption_text_color);
				set_uint(IDC_INACTIVE_CAPTION_COLOR, hive.inactive_caption_color);
				set_uint(IDC_INACTIVE_CAPTION_TEXT_COLOR, hive.inactive_caption_text_color);
				set_uint(IDC_BORDER_WIDTH, Pane::border_width);
				set_uint(IDC_CAPTION_HEIGHT, Pane::caption_height);
				set_uint(IDC_TAB_HEIGHT, Pane::tab_height);
				set_combobox(IDC_TAB_MODE, Pane::tab_mode, _T("タイトル"), _T("上"), _T("下"));
				set_check(IDC_USE_THEME, hive.use_theme);
				set_check(IDC_FORCE_SCROLL, hive.scroll_force);
				set_check(IDC_SHOW_PLAYER, hive.maximum_play);
				set_check(IDC_SHOW_TAB_FORCE, hive.show_tab_force);

				return do_modal2(parent);
			}

			virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						auto id = LOWORD(wParam);

						switch (id)
						{
						case IDC_FILL_COLOR:
						case IDC_BORDER_COLOR:
						case IDC_HOT_BORDER_COLOR:
						case IDC_ACTIVE_CAPTION_COLOR:
						case IDC_ACTIVE_CAPTION_TEXT_COLOR:
						case IDC_INACTIVE_CAPTION_COLOR:
						case IDC_INACTIVE_CAPTION_TEXT_COLOR:
							{
								auto control = (HWND)lParam;

								auto color = (COLORREF)::GetDlgItemInt(hwnd, id, 0, FALSE);

								CHOOSECOLOR cc { sizeof(cc) };
								cc.hwndOwner = hwnd;
								cc.lpCustColors = custom_colors;
								cc.rgbResult = color;
								cc.Flags = CC_RGBINIT | CC_FULLOPEN;
								if (!::ChooseColor(&cc)) return TRUE;

								color = cc.rgbResult;

								::SetDlgItemInt(hwnd, id, color, FALSE);
								::InvalidateRect(control, 0, FALSE);

								return TRUE;
							}
						}

						break;
					}
				case WM_DRAWITEM:
					{
						auto id = (UINT)wParam;

						switch (id)
						{
						case IDC_FILL_COLOR:
						case IDC_BORDER_COLOR:
						case IDC_HOT_BORDER_COLOR:
						case IDC_ACTIVE_CAPTION_COLOR:
						case IDC_ACTIVE_CAPTION_TEXT_COLOR:
						case IDC_INACTIVE_CAPTION_COLOR:
						case IDC_INACTIVE_CAPTION_TEXT_COLOR:
							{
								auto dis = (DRAWITEMSTRUCT*)lParam;

								auto color = (COLORREF)::GetDlgItemInt(hwnd, id, 0, FALSE);

								my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(color));

								::FillRect(dis->hDC, &dis->rcItem, brush.get());

								return TRUE;
							}
						}

						break;
					}
				}

				return __super::on_dlg_proc(hwnd, message, wParam, lParam);
			}
		};
	}; inline auto main_window = std::make_shared<MainWindow>();

	inline struct SubProcessManager
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// サブプロセス用のドッキングウィンドウを作成します。
			main_window->create_sub_processes(hive.main_window);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} sub_process_manager;
}
