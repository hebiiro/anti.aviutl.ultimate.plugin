#pragma once

namespace apn::workspace
{
	//
	// このクラスはメインウィンドウです。
	// このウィンドウはシャトルをドッキングさせることができます。
	//
	struct MainWindow : DockSite<my::Window>
	{
		//
		// カラー選択ダイアログのカスタムカラーです。
		//
		inline static COLORREF custom_colors[16] = {};

		//
		// メインメニューに独自に追加したポップアップメニューです。
		//
		HMENU extra_menu = nullptr;

		//
		// メインメニューの『その他』にextra_menuを挿入した位置です。
		//
		uint32_t extra_menu_index = -1;

		//
		// コンストラクタです。
		//
		MainWindow()
		{
			MY_TRACE_FUNC("");

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
		// エキストラメニューを初期化します。
		//
		void init_extra_menu()
		{
			MY_TRACE_FUNC("");

			auto menu = ::GetMenu(hwnd);
			auto etc_menu = ::GetSubMenu(menu, hive.c_main_menu_item_index.c_etc);
			extra_menu = ::CreatePopupMenu();
			extra_menu_index = ::GetMenuItemCount(etc_menu);
			::InsertMenu(etc_menu, extra_menu_index,
				MF_BYPOSITION | MF_POPUP, (UINT_PTR)extra_menu, hive.c_name);
		}

		//
		// エキストラメニューを更新します。
		//
		void update_extra_menu()
		{
			MY_TRACE_FUNC("");

			// 一旦すべての項目を削除します。
			while (::DeleteMenu(extra_menu, 0, MF_BYPOSITION)) {}

			if (hive.use_fullscreen_preview)
				::AppendMenu(extra_menu, MF_STRING, hive.c_command_id.c_fullscreen_preview, _T("再生時にプレビューを最大化"));

			::AppendMenu(extra_menu, MF_STRING, hive.c_command_id.c_create_sub_window, _T("サブウィンドウを新規作成"));
			::AppendMenu(extra_menu, MF_STRING, hive.c_command_id.c_import_layout, _T("レイアウトのインポート"));
			::AppendMenu(extra_menu, MF_STRING, hive.c_command_id.c_export_layout, _T("レイアウトのエクスポート"));
			::AppendMenu(extra_menu, MF_STRING, hive.c_command_id.c_show_config_dialog, (hive.c_name + _T("の設定"s)).c_str());
			::AppendMenu(extra_menu, MF_SEPARATOR, 0, nullptr);

			// シャトルメニューはドッキングサイトで構築します。
			__super::add_shuttle_list(extra_menu, hive.c_command_id.c_shuttle_begin,
				[=](HMENU sub_menu, UINT_PTR id, auto shuttle)
				{
					if (::IsWindowVisible(*shuttle))
						::CheckMenuItem(sub_menu, id, MF_CHECKED);
				});

			if (hive.fullscreen_preview)
				::CheckMenuItem(extra_menu, hive.c_command_id.c_fullscreen_preview, MF_CHECKED);
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

					auto id = LOWORD(wParam);

					if (id >= hive.c_command_id.c_begin && id < hive.c_command_id.c_end)
					{
						if (id >= hive.c_command_id.c_shuttle_begin && id < hive.c_command_id.c_shuttle_end)
						{
							auto menu = ::GetMenu(hwnd);

							// メニューアイテムのテキストを取得します。
							auto text = my::get_menu_item_text(menu, id, MF_BYCOMMAND);
							MY_TRACE_STR(text);

							// テキストからシャトルを取得します。
							auto shuttle = shuttle_manager.get(text);
							if (!shuttle) break;
#if 1
							// シャトルをフローティング状態にします。
							shuttle->float_window();

							// シャトルを表示します。
							::ShowWindow(*shuttle, SW_SHOW);
#else
							// シャトルの表示状態を切り替えます。
							::SendMessage(*shuttle, WM_CLOSE, 0, 0);
#endif
							break;
						}

						switch (id)
						{
						case hive.c_command_id.c_fullscreen_preview:
							{
								// 再生時最大化の有効/無効を切り替えます。
								hive.fullscreen_preview = !hive.fullscreen_preview;

								// コンフィグを更新します。
								update_config();

								break;
							}
						case hive.c_command_id.c_create_sub_window:
							{
								// サブウィンドウを新規作成します。
								create_sub_window(hwnd);

								break;
							}
						case hive.c_command_id.c_import_layout:
							{
								// レイアウトをインポートします。
								hive.app->import_layout();

								break;
							}
						case hive.c_command_id.c_export_layout:
							{
								// レイアウトをエクスポートします。
								hive.app->export_layout();

								break;
							}
						case hive.c_command_id.c_show_config_dialog:
							{
								// コンフィグダイアログを開きます。
								if (IDOK == show_config_dialog(hwnd))
								{
									// コンフィグを更新します。
									update_config();

									// レイアウトを更新します。
									update_all_layouts();
								}

								break;
							}
						}

						break;
					}

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
			case WM_INITMENUPOPUP:
				{
					MY_TRACE_FUNC("WM_INITMENUPOPUP, {:#010x}, {:#010x}", wParam, lParam);

					auto menu = (HMENU)wParam;

					// エキストラメニューが表示されようとしているので更新します。
					if (menu == extra_menu) update_extra_menu();

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
			case hive.c_message.c_post_init: // AviUtlの初期化処理が終わったあとに通知されます。
				{
					MY_TRACE_FUNC("{:#010x}, c_post_init, {:#010x}, {:#010x}, c_begin", hwnd, wParam, lParam);

					// エキストラメニューを初期化します。
					init_extra_menu();

					// レイアウトリストのフォントをセットします。
					layout_list.set_font();

					// レイアウトリストを更新します。
					layout_list.update_layout_list();

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
				auto tav_display_mode = hive.tav.display_mode - 1;
				auto tav_select_mode = hive.tav.select_mode - 1;
				auto tav_stretch_mode = hive.tav.stretch_mode - 1;
				auto tav_location = hive.tav.location - 1;
				auto tav_node_align = hive.tav.node_align - 1;

				create(hive.instance, MAKEINTRESOURCE(IDD_CONFIG), parent);

				// 配色
				set_uint(IDC_FILL_COLOR, hive.fill_color);
				set_uint(IDC_BORDER_COLOR, hive.border_color);
				set_uint(IDC_HOT_BORDER_COLOR, hive.hot_border_color);
				set_uint(IDC_ACTIVE_CAPTION_COLOR, hive.active_caption_color);
				set_uint(IDC_ACTIVE_CAPTION_TEXT_COLOR, hive.active_caption_text_color);
				set_uint(IDC_INACTIVE_CAPTION_COLOR, hive.inactive_caption_color);
				set_uint(IDC_INACTIVE_CAPTION_TEXT_COLOR, hive.inactive_caption_text_color);

				// タブの設定
				set_combobox_index(IDC_TAV_DISPLAY_MODE, tav_display_mode, _T("手動"), _T("半自動"), _T("自動"), _T("全自動"));
				set_combobox_index(IDC_TAV_SELECT_MODE, tav_select_mode, _T("クリック"), _T("ホバー"));
				set_combobox_index(IDC_TAV_STRETCH_MODE, tav_stretch_mode, _T("内側"), _T("外側"));
				set_combobox_index(IDC_TAV_LOCATION, tav_location, _T("左辺"), _T("上辺"), _T("右辺"), _T("下辺"));
				set_combobox_index(IDC_TAV_NODE_ALIGN, tav_node_align, _T("左または上"), _T("右または下"), _T("中央"));
				set_uint(IDC_TAB_HEIGHT, hive.tab_height);

				// その他の設定
				set_combobox_index(IDC_LAYOUT_LIST_MODE, hive.layout_list_mode, _T("なし"), _T("上"), _T("下"));
				set_uint(IDC_BORDER_WIDTH, hive.border_width);
				set_uint(IDC_CAPTION_HEIGHT, hive.caption_height);

				// チェックボックス
				set_check(IDC_USE_THEME, hive.use_theme);
				set_check(IDC_SCROLL_FORCE, hive.scroll_force);
				set_check(IDC_SHOW_PLAYER, hive.use_fullscreen_preview);
				set_check(IDC_SHOW_TAB_FORCE, hive.show_tab_force);
				set_check(IDC_BYPASS_KEYBOARD_MESSAGE, hive.bypass_keyboard_message);
				set_check(IDC_IGNORE_CTRL_KEY_UP, hive.ignore_ctrl_key_up);

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
								CHOOSECOLOR cc { sizeof(cc) };
								cc.hwndOwner = hwnd;
								cc.lpCustColors = custom_colors;
								cc.rgbResult = get_uint(id);
								cc.Flags = CC_RGBINIT | CC_FULLOPEN;
								if (!::ChooseColor(&cc)) return TRUE;

								set_uint(id, cc.rgbResult);

								my::invalidate(control);

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
