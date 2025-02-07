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
			inline static constexpr uint32_t c_begin = 1;
			inline static constexpr uint32_t c_end = c_begin + 100;
			inline static constexpr uint32_t c_fullscreen_preview = c_begin + 0;
			inline static constexpr uint32_t c_show_config_dialog = c_begin + 1;
			inline static constexpr uint32_t c_import_layout = c_begin + 2;
			inline static constexpr uint32_t c_export_layout = c_begin + 3;
			inline static constexpr uint32_t c_create_sub_window = c_begin + 4;
			inline static constexpr uint32_t c_reset_float_shuttles = c_begin + 5;
			inline static constexpr uint32_t c_shuttle_begin = c_begin + 10;
			inline static constexpr uint32_t c_shuttle_end = c_end;
		} c_command_id;

		//
		// コンストラクタです。
		//
		MainWindow()
		{
			MY_TRACE_FUNC("");
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
			constexpr auto c_class_name = _T("AviUtl");

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
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
				nullptr, nullptr, hive.instance, nullptr);
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
		// aviutlの初期化が完了したあとに追加の初期化処理を実行します。
		//
		BOOL post_init()
		{
			MY_TRACE_FUNC("開始");

			// エキストラメニューを初期化します。
			init_extra_menu();

			// レイアウトリストのフォントをセットします。
			layout_list.set_font();

			// レイアウトリストを更新します。
			layout_list.update_layout_list();

			// プリファレンスを読み込みます。
			read_preference();

			MY_TRACE_FUNC("終了");

			// メインウィンドウのタイトルを更新します。
			refresh_title();

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

				return TRUE;
			}

			if (class_name == _T("AviUtl"))
			{
				// 拡張編集ウィンドウのダミーかもしれないのでチェックしておきます。
				if (hwnd == exedit_window->dummy) return FALSE;

				if (window_name == L"再生ウィンドウ")
				{
					if (*player_window) return FALSE;

					MY_TRACE("再生ウィンドウ用のシャトルを初期化します\n");

					player_window->init(window_name, hwnd);

					return TRUE;
				}

				MY_TRACE("「{}」用のシャトルを初期化します\n", window_name);

				auto shuttle = std::make_shared<Shuttle>();

				shuttle->init(window_name, hwnd);

				return TRUE;
			}

			return FALSE;
		}

		//
		// エキストラメニューを初期化します。
		//
		void init_extra_menu()
		{
			MY_TRACE_FUNC("");

			auto menu = ::GetMenu(hwnd);
			auto etc_menu = ::GetSubMenu(menu, hive.c_main_menu_item_index.c_etc);
			::AppendMenu(etc_menu, MF_STRING, hive.c_command_id.c_show_extra_menu, hive.c_display_name);
		}

		//
		// エキストラメニューを表示します。
		//
		BOOL show_extra_menu()
		{
			MY_TRACE_FUNC("");

			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			if (hive.use_fullscreen_preview)
				::AppendMenu(menu.get(), MF_STRING, c_command_id.c_fullscreen_preview, _T("再生時にプレビューを最大化"));

			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_create_sub_window, _T("サブウィンドウを新規作成"));
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_reset_float_shuttles, _T("フローティングウィンドウをリセット"));
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_import_layout, _T("レイアウトのインポート"));
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_export_layout, _T("レイアウトのエクスポート"));
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_show_config_dialog, std::format(_T("『{}』の設定"), hive.c_display_name).c_str());
			::AppendMenu(menu.get(), MF_SEPARATOR, 0, nullptr);

			// シャトルメニューはドッキングサイトで構築します。
			__super::add_shuttle_list(menu.get(), c_command_id.c_shuttle_begin,
				[=](HMENU sub_menu, UINT_PTR id, auto shuttle)
			{
				if (::IsWindowVisible(*shuttle))
					::CheckMenuItem(sub_menu, id, MF_CHECKED);
			});

			if (hive.fullscreen_preview)
				::CheckMenuItem(menu.get(), c_command_id.c_fullscreen_preview, MF_CHECKED);

			auto cursor_pos = my::get_cursor_pos();
			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD,
				cursor_pos.x, cursor_pos.y, *this, nullptr);
			if (!id) return FALSE;

			if (id >= c_command_id.c_shuttle_begin && id < c_command_id.c_shuttle_end)
			{
				// メニューアイテムのテキストを取得します。
				auto text = my::get_menu_item_text(menu.get(), id, MF_BYCOMMAND);
				MY_TRACE_STR(text);

				// テキストからシャトルを取得します。
				auto shuttle = shuttle_manager.get(text);
				if (!shuttle) return FALSE;

				// シャトルがドッキング中かもしれないので、ドッキングを解除します。
				shuttle->fire_release_shuttle();

				// シャトルを表示状態にします。
				shuttle->show();

				return TRUE;
			}

			switch (id)
			{
			case c_command_id.c_fullscreen_preview:
				{
					// 再生時最大化の有効/無効を切り替えます。
					hive.fullscreen_preview = !hive.fullscreen_preview;

					// コンフィグを更新します。
					update_config();

					break;
				}
			case c_command_id.c_create_sub_window:
				{
					// サブウィンドウを新規作成します。
					create_sub_window(hwnd);

					break;
				}
			case c_command_id.c_reset_float_shuttles:
				{
					// フローティングウィンドウの表示位置をリセットします。
					reset_float_shuttles(hwnd);

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
			case c_command_id.c_show_config_dialog:
				{
					// コンフィグダイアログを開きます。
					show_config_dialog();

					break;
				}
			}

			return TRUE;
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
		// コンフィグを更新します。
		//
		void update_config()
		{
			MY_TRACE_FUNC("");

			hive.app->enable_fullscreen_preview(hive.use_fullscreen_preview && hive.fullscreen_preview);
		}

		//
		// すべてのドッキングサイトのレイアウトを更新します。
		//
		void update_all_layouts()
		{
			MY_TRACE_FUNC("");

			__super::update_dock_site();

			for (auto& sub_window : SubWindow::collection)
				sub_window->update_dock_site();
		}

		//
		// プリファレンスを読み込みます。
		//
		BOOL read_preference()
		{
			MY_TRACE_FUNC("");

			hive.app->read_preference();

			// コンフィグを更新します。
			update_config();

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
		BOOL show_config_dialog()
		{
			ConfigDialog dialog;

			// コンフィグダイアログを表示します。
			if (IDOK != dialog.do_modal(*this)) return FALSE;

			// コンフィグを更新します。
			update_config();

			// レイアウトを更新します。
			update_all_layouts();

			return TRUE;
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
		// すべてのフローティングウィンドウの表示位置をリセットします。
		//
		BOOL reset_float_shuttles(HWND parent)
		{
			// 処理を実行するかユーザーに確認します。
			if (IDOK != hive.message_box(
				L"すべてのフローティングウィンドウの表示位置をリセットします",
				parent, MB_OKCANCEL | MB_ICONWARNING))
			{
				return FALSE;
			}

			constexpr auto min_size = 10;
			constexpr auto default_w = 800;
			constexpr auto default_h = 600;

			auto monitor_rc = my::get_monitor_rect(parent);
			auto monitor_x = monitor_rc.left;
			auto monitor_y = monitor_rc.top;
			auto monitor_w = my::get_width(monitor_rc);
			auto monitor_h = my::get_height(monitor_rc);

			const auto get_centered_pos = [&](int32_t w, int32_t h) -> POINT
			{
				return {
					monitor_x + (monitor_w - w) / 2,
					monitor_y + (monitor_h - h) / 2,
				};
			};

			const auto reset_window_pos = [&](HWND hwnd)
			{
				// ウィンドウが最大化されている場合は除外します。
				if (::IsZoomed(hwnd)) return FALSE;

				// ウィンドウの現在の位置を取得します。
				auto rc = my::get_window_rect(hwnd);
				auto w = my::get_width(rc);
				auto h = my::get_height(rc);

				// ウィンドウサイズを正規化します。
				if (w < min_size || w > monitor_w) w = default_w;
				if (h < min_size || h > monitor_h) h = default_h;

				// ウィンドウをモニタ中央に表示するための座標を取得します。
				auto pos = get_centered_pos(w, h);

				// ウィンドウの表示位置を変更します。
				::SetWindowPos(hwnd, nullptr, pos.x, pos.y, w, h, SWP_NOZORDER);

				return TRUE;
			};

			// すべてのシャトルを走査します。
			for (auto& pair : shuttle_manager.collection)
			{
				// シャトルを取得します。
				auto& shuttle = pair.second;

				// シャトルがドッキング状態の場合は除外します。
				if (shuttle->is_docking()) continue;

				// フローティングウィンドウの位置をリセットします。
				reset_window_pos(*shuttle->float_container);
			}

			return TRUE;
		}

		//
		// この仮想関数はワークエリア矩形が必要なときに呼び出されます。
		//
		virtual RECT get_workarea() override
		{
			auto rc = __super::get_workarea();

			switch (hive.layout_list_mode)
			{
			default:
			case hive.c_layout_list_mode.c_none: break;
			case hive.c_layout_list_mode.c_top: rc.top += hive.tab_height; break;
			case hive.c_layout_list_mode.c_bottom: rc.bottom -= hive.tab_height; break;
			}

			return rc;
		}

		//
		// この仮想関数はドッキングサイトのレイアウトを更新する必要があるときに呼び出されます。
		//
		virtual void update_dock_site(const RECT& rc) override
		{
			MY_TRACE_FUNC("");

			switch (hive.layout_list_mode)
			{
			default:
			case hive.c_layout_list_mode.c_none:
				{
					// レイアウトリストを非表示にします。
					::ShowWindow(layout_list, SW_HIDE);

					break;
				}
			case hive.c_layout_list_mode.c_top:
				{
					// レイアウトリストをメインウィンドウの上側に配置します。
					auto x = rc.left;
					auto y = rc.top - hive.tab_height;
					auto w = my::get_width(rc);
					auto h = hive.tab_height;

					// レイアウトリストを表示します。
					::SetWindowPos(layout_list, HWND_TOP,
						x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

					break;
				}
			case hive.c_layout_list_mode.c_bottom:
				{
					// レイアウトリストをメインウィンドウの下側に配置します。
					auto x = rc.left;
					auto y = rc.bottom;
					auto w = my::get_width(rc);
					auto h = hive.tab_height;

					// レイアウトリストを表示します。
					::SetWindowPos(layout_list, HWND_TOP,
						x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

					break;
				}
			}

			__super::update_dock_site(rc);
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
			case WM_ACTIVATE:
				{
					MY_TRACE_FUNC("WM_ACTIVATE, {:#010x}, {:#010x}", wParam, lParam);

					// 「patch.aul」用の処理です。
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
			case WM_MENUSELECT:
				{
					MY_TRACE_FUNC("WM_MENUSELECT, {:#010x}, {:#010x}", wParam, lParam);

					// 「patch.aul」用の処理です。
					return ::SendMessage(hive.aviutl_window, message, wParam, lParam);
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, {:#010x}, {:#010x}", wParam, lParam);

					return ::SendMessage(hive.aviutl_window, message, wParam, lParam);
				}
			case WM_COMMAND:
				{
					MY_TRACE_FUNC("WM_COMMAND, {:#010x}, {:#010x}", wParam, lParam);

					// 「PSDToolKit」用の処理です。
					// WM_COMMAND終了時にマウスメッセージがPSDToolKitに飛ぶとフリーズしてしまいます。
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

					switch (wParam)
					{
					case SC_RESTORE:
					case SC_MINIMIZE:
						{
							// 「patch.aul」用の処理です。
							::SendMessage(hive.aviutl_window, message, wParam, lParam);

							break;
						}
					}

					break;
				}
			case WM_NOTIFY: // レイアウトリストから通知されます。
				{
					// レイアウトリストのタブが切り替わった場合は
					auto header = (NMHDR*)lParam;
					if (header->hwndFrom == layout_list && header->code == TCN_SELCHANGE)
					{
						// レイアウトを変更します。
						hive.app->change_layout(layout_list.get_current_file_name());

						// レイアウトを更新します。
						update_all_layouts();
					}

					break;
				}
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE");

					hive.main_window = hwnd;
					MY_TRACE_HEX(hive.main_window);

					hive.theme.reset(::OpenThemeData(hwnd, VSCLASS_WINDOW));
					MY_TRACE_HEX(hive.theme.get());

					hive.theme_tav.reset(::OpenThemeData(hwnd, VSCLASS_TAB));
					MY_TRACE_HEX(hive.theme_tav.get());

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY");

					hive.theme_tav.reset();
					hive.theme.reset();

					break;
				}
			case hive.c_message.c_post_init: // 初期化を完了させるために通知されます。
				{
					post_init();

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
			case hive.c_message.c_show_extra_menu: // エキストラメニューを表示するために通知されます。
				{
					show_extra_menu();

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
				auto tav_display_mode = hive.tav.display_mode - 1;
				auto tav_select_mode = hive.tav.select_mode - 1;
				auto tav_stretch_mode = hive.tav.stretch_mode - 1;
				auto tav_location = hive.tav.location - 1;
				auto tav_node_align = hive.tav.node_align - 1;

				create(hive.instance, MAKEINTRESOURCE(IDD_CONFIG), parent);

				// 配色
				bind_uint(IDC_FILL_COLOR, hive.fill_color);
				bind_uint(IDC_BORDER_COLOR, hive.border_color);
				bind_uint(IDC_HOT_BORDER_COLOR, hive.hot_border_color);
				bind_uint(IDC_ACTIVE_CAPTION_COLOR, hive.active_caption_color);
				bind_uint(IDC_ACTIVE_CAPTION_TEXT_COLOR, hive.active_caption_text_color);
				bind_uint(IDC_INACTIVE_CAPTION_COLOR, hive.inactive_caption_color);
				bind_uint(IDC_INACTIVE_CAPTION_TEXT_COLOR, hive.inactive_caption_text_color);

				// タブの設定
				bind_combobox_index(IDC_TAV_DISPLAY_MODE, tav_display_mode, _T("手動"), _T("半自動"), _T("自動"), _T("全自動"));
				bind_combobox_index(IDC_TAV_SELECT_MODE, tav_select_mode, _T("クリック"), _T("ホバー"));
				bind_combobox_index(IDC_TAV_STRETCH_MODE, tav_stretch_mode, _T("内側"), _T("外側"));
				bind_combobox_index(IDC_TAV_LOCATION, tav_location, _T("左辺"), _T("上辺"), _T("右辺"), _T("下辺"));
				bind_combobox_index(IDC_TAV_NODE_ALIGN, tav_node_align, _T("左または上"), _T("右または下"), _T("中央"));
				bind_uint(IDC_TAB_HEIGHT, hive.tab_height);

				// その他の設定
				bind_combobox_index(IDC_LAYOUT_LIST_MODE, hive.layout_list_mode, _T("なし"), _T("上"), _T("下"));
				bind_uint(IDC_BORDER_WIDTH, hive.border_width);
				bind_uint(IDC_CAPTION_HEIGHT, hive.caption_height);

				// チェックボックス
				bind_check(IDC_USE_THEME, hive.use_theme);
				bind_check(IDC_SCROLL_FORCE, hive.scroll_force);
				bind_check(IDC_SHOW_PLAYER, hive.use_fullscreen_preview);
				bind_check(IDC_SHOW_TAB_FORCE, hive.show_tab_force);
				bind_check(IDC_BYPASS_KEYBOARD_MESSAGE, hive.bypass_keyboard_message);
				bind_check(IDC_IGNORE_CTRL_KEY_UP, hive.ignore_ctrl_key_up);

				auto result = do_modal2(parent);
				if (result != IDOK) return result;

				hive.tav.display_mode = tav_display_mode + 1;
				hive.tav.select_mode = tav_select_mode + 1;
				hive.tav.stretch_mode = tav_stretch_mode + 1;
				hive.tav.location = tav_location + 1;
				hive.tav.node_align = tav_node_align + 1;

				return result;
			}

			virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						auto id = LOWORD(wParam);
						auto control = (HWND)lParam;

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
								try
								{
									// カラー選択ダイアログを表示してカラーを取得します。
									auto color = magi.choose_color(hwnd, get_uint(id));

									// 取得した色をコントロールに適用します。
									set_uint(id, color);
									my::invalidate(control);
								}
								catch (...)
								{
								}

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

								my::gdi::unique_ptr<HBRUSH> brush(
									::CreateSolidBrush(get_uint(id)));
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
}
