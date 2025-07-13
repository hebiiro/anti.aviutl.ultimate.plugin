#pragma once

namespace apn::settings_browser
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
		struct : my::Window {
			//
			// ウィンドウプロシージャです。
			//
			virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_KEYDOWN:
					{
						if (wParam == VK_RETURN)
						{
							// コンフィグを更新します。
							addin_dialog.update_config();

							// ブラウザを更新します。
							app->navigate();
						}

						break;
					}
				}

				return __super::on_wnd_proc(hwnd, message, wParam, lParam);
			}
		} url;

		//
		// 『取得』のオプションメニューを表示します。
		//
		BOOL show_get_settings_option(const POINT& point)
		{
			MY_TRACE_FUNC("");

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_auto_get = 1;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			::AppendMenu(menu.get(), MF_STRING, c_auto_get, _T("自動的に取得"));
			if (hive.auto_get)
				::CheckMenuItem(menu.get(), c_auto_get, MF_CHECKED);

			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, *this, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_auto_get:
				{
					hive.auto_get = !hive.auto_get;

					break;
				}
			}

			return TRUE;
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_text(IDC_URL, hive.url);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_text(IDC_URL, hive.url);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			url.subclass(ctrl(IDC_URL));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto button = std::make_shared<RelativePos>(base_size * 2);
			auto spin = std::make_shared<RelativePos>(base_size * 1);
			auto rest = std::make_shared<AbsolutePos>(1, 1, 1);
			std::shared_ptr<AbsolutePos> q[24 + 1];
			for (auto i = 0; i < std::size(q); i++)
				q[i] = std::make_shared<AbsolutePos>(i, std::size(q) - 1);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_GET_SETTINGS));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_URL_DIR));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, rest, {}, ctrl(IDC_URL));
				}
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, rest, margin, browser);
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);

			switch (id)
			{
			case IDC_GET_SETTINGS:
				{
					// 選択アイテムからjsonを生成します。
					app->generate_settings_json();

					break;
				}
			case IDC_URL_DIR:
				{
					// ユーザーが指定したファイル名が格納されるバッファです。
					std::wstring file_name(MAX_PATH, L'\0');

					// ファイルダイアログを表示します。
					OPENFILENAMEW ofn = { sizeof(ofn) };
					ofn.hwndOwner = hive.main_window;
					ofn.Flags = OFN_FILEMUSTEXIST;
					ofn.lpstrInitialDir = hive.assets_folder_name.c_str();
					ofn.lpstrTitle = L"htmlファイルを選択";
					ofn.lpstrFile = file_name.data();
					ofn.nMaxFile = (DWORD)file_name.size();
					ofn.lpstrFilter = L"htmlファイル (*.html)\0*.html\0" "すべてのファイル (*.*)\0*.*\0";
					ofn.lpstrDefExt = L"html";
					if (!::GetOpenFileNameW(&ofn))
						break;

					// URLを更新します。
					hive.url = file_name;

					// コントロールを更新します。
					update_controls();

					// ブラウザを更新します。
					app->navigate();

					break;
				}
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
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					if (!browser.init(hwnd))
					{
						MY_TRACE("ブラウザの作成に失敗しました\n");

						return -1;
					}

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					browser.exit();

					break;
				}
			case WM_CONTEXTMENU:
				{
					MY_TRACE_FUNC("WM_CONTEXTMENU, {/hex}, {/hex}", wParam, lParam);

					auto control = (HWND)wParam;
					auto control_id = ::GetDlgCtrlID(control);
					auto point = my::lp_to_pt(lParam);

					switch (control_id)
					{
					case IDC_GET_SETTINGS: return show_get_settings_option(point);
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
