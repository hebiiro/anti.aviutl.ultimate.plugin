#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはビジュアルです。
	// 音声を視覚化して表示するためのブラウザを保持します。
	//
	struct Visual : my::Window2
	{
		struct VisualBrowser : Browser
		{
			//
			// ビジュアルです。
			//
			Visual& visual;

			//
			// イベントトークンです。
			//
			EventRegistrationToken context_menu_requested_token = {};

			//
			// コンストラクタです。
			//
			VisualBrowser(Visual& visual)
				: visual(visual)
			{
			}

			//
			// 初期化処理を実行します。
			//
			BOOL init()
			{
				MY_TRACE_FUNC("");

				if (!create())
				{
					MY_TRACE("ビジュアルブラウザの作成に失敗しました\n");

					return FALSE;
				}

				return __super::init(hive.instance);
			}

			//
			// 後始末処理を実行します。
			//
			BOOL exit()
			{
				MY_TRACE_FUNC("");

				return __super::exit();
			}

			//
			// ウィンドウを作成します。
			//
			BOOL create()
			{
				MY_TRACE_FUNC("");

				WNDCLASSEX wc = { sizeof(wc) };
				wc.style = CS_HREDRAW | CS_VREDRAW;
				wc.lpfnWndProc = ::DefWindowProc;
				wc.hInstance = hive.instance;
				wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
				wc.lpszClassName = _T("audio_visualizer.visual.browser");
				::RegisterClassEx(&wc);

				return __super::create(
					WS_EX_NOPARENTNOTIFY,
					wc.lpszClassName,
					wc.lpszClassName,
					WS_VISIBLE | WS_CHILD |
					WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
					100, 100, 600, 600,
					visual, nullptr, hive.instance, nullptr);
			}

			//
			// スクリプトにビジュアルの設定を要求します。
			//
			BOOL request_preference(BOOL notify)
			{
				MY_TRACE_FUNC("{}", notify);

				return post_web_message_as_json(
					create_web_message_as_json("request_preference",
					{
						{ "notify", !!notify },
					}));
			}

			//
			// スクリプトにビジュアルの設定を送信します。
			//
			BOOL post_preference(BOOL notify)
			{
				MY_TRACE_FUNC("{}", notify);

				return post_web_message_as_json(
					create_web_message_as_json("update_preference",
					{
						{ "notify", !!notify },
						{ "preference", visual.editor.preference },
					}));
			}

			//
			// この仮想関数は、スクリプトからメッセージが発行されたときに呼ばれます。
			//
			virtual BOOL on_web_message_as_json(const std::string& key, const n_json& json) override
			{
				try
				{
					if (key == "response_preference")
					{
						// スクリプトからプリファレンスを受け取ります。
						visual.editor.preference = json["preference"];

						// 通知が有効の場合は
						if (json["notify"].get<bool>())
						{
							// ビジュアルエディタにプリファレンスの変更を通知します。
							::PostMessage(visual.editor, hive.c_message.c_notify_change_preference, 0, 0);
						}
					}
				}
				catch (...)
				{
				}

				return TRUE;
			}

			//
			// この仮想関数は、初期化が完了したあとに呼ばれます。
			//
			virtual BOOL on_post_init() override
			{
				MY_TRACE_FUNC("");

				// コンテキストメニューをカスタマイズします。
				webview->add_ContextMenuRequested(
					Callback<ICoreWebView2ContextMenuRequestedEventHandler>(
					[this](ICoreWebView2* sender, ICoreWebView2ContextMenuRequestedEventArgs* args) -> HRESULT
				{
					wil::com_ptr<ICoreWebView2ContextMenuItemCollection> items;
					args->get_MenuItems(&items);

					UINT32 item_count = 0;
					items->get_Count(&item_count);

					wil::com_ptr<ICoreWebView2ContextMenuItem> separator_menu_item;
					env->CreateContextMenuItem(
						nullptr,
						nullptr,
						COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_SEPARATOR,
						&separator_menu_item);
					items->InsertValueAtIndex(item_count++, separator_menu_item.get());

					wil::com_ptr<ICoreWebView2ContextMenuItem> select_contents_file_menu_item;
					env->CreateContextMenuItem(
						L"スキームを選択",
						nullptr,
						COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_COMMAND,
						&select_contents_file_menu_item);

					select_contents_file_menu_item->add_CustomItemSelected(
						Callback<ICoreWebView2CustomItemSelectedEventHandler>(
						[this](ICoreWebView2ContextMenuItem* sender, IUnknown* args) -> HRESULT
					{
						::PostMessage(visual, hive.c_message.c_select_scheme, 0, 0);

						return S_OK;
					}).Get(), nullptr);

					items->InsertValueAtIndex(item_count++, select_contents_file_menu_item.get());

					wil::com_ptr<ICoreWebView2ContextMenuItem> show_visual_editor_menu_item;
					env->CreateContextMenuItem(
						L"ビジュアルの設定",
						nullptr,
						COREWEBVIEW2_CONTEXT_MENU_ITEM_KIND_COMMAND,
						&show_visual_editor_menu_item);

					show_visual_editor_menu_item->add_CustomItemSelected(
						Callback<ICoreWebView2CustomItemSelectedEventHandler>(
						[this](ICoreWebView2ContextMenuItem* sender, IUnknown* args) -> HRESULT
					{
						::PostMessage(visual, hive.c_message.c_show_visual_editor, 0, 0);

						return S_OK;
					}).Get(), nullptr);

					items->InsertValueAtIndex(item_count++, show_visual_editor_menu_item.get());

					return S_OK;
				}).Get(), &context_menu_requested_token);

				MY_TRACE_STR(hive.config_file_name);
				MY_TRACE_INT(!std::filesystem::exists(hive.config_file_name));

				// コンフィグファイルが存在しない場合は
				if (!std::filesystem::exists(hive.config_file_name))
				{
					// デフォルトのコンフィグを適用します。
					visual.apply_default_config();
				}

				// デフォルト処理を実行します。
				return __super::on_post_init();
			}

			//
			// この仮想関数は、ナビゲーションが完了したあとに呼ばれます。
			//
			virtual BOOL on_navigation_completed(BOOL success) override
			{
				MY_TRACE_FUNC("{}", success);

				if (success)
				{
					// ホストプロセスにオプションを要求します。
					::PostMessage(hive.host_window, share::c_message.c_request_option, (WPARAM)(HWND)visual, 0);

					// ビジュアルの設定が有効の場合は
					if (!visual.editor.preference.is_null())
					{
						// ビジュアルの設定をスクリプトに送信します。通知はオフにします。
						post_preference(FALSE);
					}
					// ビジュアルの設定が無効の場合は
					else
					{
						// ビジュアルの設定をスクリプトに要求します。
						// 受け取ったビジュアルの設定をビジュアルエディタにも
						// 送らないといけないので、通知はオンにします。
						request_preference(TRUE);
					}
				}

				// デフォルト処理を実行します。
				return __super::on_navigation_completed(success);
			}
		} browser;

		//
		// スキーム(json)ファイルのパスです。
		//
		std::filesystem::path scheme_file_name;

		//
		// ビジュアルエディタです。
		//
		VisualEditor editor;

		//
		// コンストラクタです。
		//
		Visual()
			: browser(*this)
		{
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND co_window)
		{
			MY_TRACE_FUNC("{:#010x}", co_window);

			if (!create(co_window))
			{
				MY_TRACE("ビジュアルウィンドウの作成に失敗しました\n");

				return FALSE;
			}

			if (!browser.init()) return FALSE;
			if (!editor.init(*this)) return FALSE;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			editor.exit();
			browser.exit();

			return TRUE;
		}

		//
		// ウィンドウを作成します。
		//
		BOOL create(HWND co_window)
		{
			MY_TRACE_HEX(co_window);

			auto name = my::get_window_text(co_window);

			WNDCLASSEX wc = { sizeof(wc) };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = _T("audio_visualizer.visual");
			::RegisterClassEx(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				wc.lpszClassName,
				name.c_str(),
				WS_VISIBLE | WS_CHILD |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 600, 600,
				co_window, nullptr, hive.instance, nullptr);
		}

		//
		// スキームを読み込みます。
		//
		BOOL read_scheme()
		{
			MY_TRACE_FUNC("");

			try
			{
				if (scheme_file_name.empty()) return FALSE;

				// スキームファイルを読み込みます。
				std::ifstream ifs(scheme_file_name);
				auto root = n_json::parse(ifs);
				auto visual_node = root.at("visual");
				auto editor_node = root.at("editor");

				auto origin = scheme_file_name.parent_path();
				MY_TRACE_STR(origin);

				// ビジュアルのコンテンツファイルを読み込みます。
				browser.contents_file_name = get_abs_path(
					origin, my::cp_to_wide(visual_node.at("path"), CP_UTF8));
				MY_TRACE_STR(browser.contents_file_name);
				browser.navigate();

				// ビジュアルエディタのサイズを読み込みます。
				auto size = SIZE { editor_node.at("width"), editor_node.at("height") };
				MY_TRACE_SIZE(size);
				::SetWindowPos(editor, nullptr,
					0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);

				// ビジュアルエディタのコンテンツファイルを読み込みます。
				editor.browser.contents_file_name = get_abs_path(
					origin, my::cp_to_wide(editor_node.at("path"), CP_UTF8));
				MY_TRACE_STR(editor.browser.contents_file_name);
				editor.browser.navigate();

				return TRUE;
			}
			catch (const std::exception& error)
			{
				hive.message_box(my::ws(error.what()), *this);

				return FALSE;
			}
		}

		//
		// ファイル選択ダイアログを表示してスキームを切り替えます。
		//
		BOOL select_scheme()
		{
			// ファイル選択ダイアログ用のバッファを確保します。
			auto file_name = scheme_file_name.wstring();
			file_name.resize(MAX_PATH, L'\0');
			MY_TRACE_STR(file_name);

			// ファイル選択ダイアログ用の構造体を設定します。
			OPENFILENAMEW ofn = { sizeof(ofn) };
			ofn.hwndOwner = hive.main_window;
			ofn.Flags = OFN_FILEMUSTEXIST;
			ofn.lpstrTitle = L"スキームを選択";
			ofn.lpstrFile = file_name.data();
			ofn.nMaxFile = file_name.size();
			ofn.lpstrFilter = L"jsonファイル (*.json)\0*.json\0" "すべてのファイル (*.*)\0*.*\0";
			ofn.lpstrDefExt = L"json";

			// 初期ディレクトリを設定します。
			auto dir = magi.get_assets_file_name(hive.c_name);
			ofn.lpstrInitialDir = dir.c_str();
			MY_TRACE_STR(ofn.lpstrInitialDir);

			// ファイル選択ダイアログを表示します。
			if (!::GetOpenFileNameW(&ofn)) return FALSE;

			// 選択されたスキームを取得します。
			// c_str()を使用しているのは、余剰分のヌル文字がコピーされないようにするためです。
			scheme_file_name = file_name.c_str();
			MY_TRACE_STR(scheme_file_name);

			// プリファレンスを初期値に戻します。
			editor.preference = {};

			// 選択されたスキームを読み込みます。
			return read_scheme();
		}

		//
		// ファイル選択ダイアログを表示してスキームを切り替えます。
		//
		BOOL show_visual_editor()
		{
			// ビジュアルエディタの表示位置を算出します。
			auto editor_rc = my::get_window_rect(editor);
			auto editor_w = my::get_width(editor_rc);
			auto editor_h = my::get_height(editor_rc);
			auto visual_rc = my::get_window_rect(*this);
			auto x = my::get_center_x(visual_rc) - editor_w / 2;
			auto y = my::get_center_y(visual_rc) - editor_h / 2;

			// ビジュアルエディタを表示します。
			::SetWindowPos(editor, HWND_TOP,
				x, y, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

			return TRUE;
		}

		//
		// 既定のコンフィグを適用します。
		//
		BOOL apply_default_config()
		{
			MY_TRACE_FUNC("");

			auto assets_folder_name = std::filesystem::path(magi.get_assets_file_name(hive.c_name));
			MY_TRACE_STR(assets_folder_name);

			auto name = my::get_window_text(::GetParent(*this));

			if (name == L"左右Lv")
			{
				scheme_file_name = assets_folder_name / L"simple_bar.json";
				editor.preference = { { "channel_mode", "both" } };
			}
			else if (name == L"左Lv")
			{
				scheme_file_name = assets_folder_name / L"simple_bar.json";
				editor.preference = { { "channel_mode", "left" } };
			}
			else if (name == L"右Lv")
			{
				scheme_file_name = assets_folder_name / L"simple_bar.json";
				editor.preference = { { "channel_mode", "right" } };
			}
			else if (name == L"パワースペクトル")
			{
				scheme_file_name = assets_folder_name / L"simple_spectre.json";
				editor.preference = { { "channel_mode", "both" } };
			}

			// スキームを読み込みます。
			return read_scheme();
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {:#010x}, {:#010x}", wParam, lParam);

					// ブラウザをクライアント領域全体に広げます。
					auto rc = my::get_client_rect(hwnd);
					auto x = rc.left;
					auto y = rc.top;
					auto w = my::get_width(rc);
					auto h = my::get_height(rc);
					::SetWindowPos(browser, nullptr, x, y, w, h, SWP_NOZORDER);

					break;
				}
			case hive.c_message.c_select_scheme:
				{
					MY_TRACE_FUNC("c_select_scheme, {:#010x}, {:#010x}", wParam, lParam);

					// ユーザーにスキームの選択を促します。
					select_scheme();

					break;
				}
			case hive.c_message.c_show_visual_editor:
				{
					MY_TRACE_FUNC("c_show_visual_editor, {:#010x}, {:#010x}", wParam, lParam);

					// ビジュアルエディタを表示します。
					show_visual_editor();

					break;
				}
			case hive.c_message.c_notify_change_preference:
				{
					MY_TRACE_FUNC("c_notify_change_preference, {:#010x}, {:#010x}", wParam, lParam);

					// ビジュアルの設定をスクリプトに送信します。
					browser.post_preference(FALSE);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
