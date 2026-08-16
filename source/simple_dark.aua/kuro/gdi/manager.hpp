#pragma once

namespace apn::dark::kuro::gdi
{
	//
	// このクラスはGDIレンダラーを管理します。
	//
	inline struct manager_t : entry_t::node_t
	{
		virtual BOOL on_init() { return init(); }
		virtual BOOL on_exit() { return exit(); }

		//
		// ウィンドウのクラスブラシを変更します。
		//
		inline static BOOL change_class_brush(HWND hwnd)
		{
			// ウィンドウのクラスブラシを取得します。
			auto brush = (HBRUSH)::GetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND);

			//
			// この関数はウィンドウのクラスブラシを変更します。
			//
			const auto change_brush = [&](int color_id, const paint::palette_t& palette, int part_id, int state_id)
			{
				// ブラシがカラーIDと一致する場合は
				if (brush == (HBRUSH)((INT_PTR)color_id + 1) || brush == ::GetSysColorBrush(color_id))
				{
					// ピグメントを取得できた場合は
					if (auto pigment = palette.get(part_id, state_id))
					{
						// ウィンドウのクラスブラシを変更します。
						::SetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)pigment->background.get_brush());

						return TRUE;
					}
				}

				return FALSE;
			};

			// カラーID毎にウィンドウのクラスブラシを変更します。
			if (change_brush(COLOR_3DFACE, paint::dialog_material.palette, WP_DIALOG, ETS_NORMAL)) return TRUE;
			if (change_brush(COLOR_WINDOW, paint::editbox_material.palette, EP_EDITTEXT, ETS_NORMAL)) return TRUE;

			return FALSE;
		}

		//
		// 指定されたウィンドウに対応するGDIレンダラーを返します。
		//
		std::shared_ptr<renderer_t> find_renderer(
			HWND hwnd, const renderer_t::normalized_class_name_t& class_name, BOOL force)
		{
			// 既にアタッチ済みの場合は除外します。
			if (renderer_t::from_handle(hwnd)) return nullptr;

			//
			// この関数は指定された文字列が指定された正規表現パターンにマッチする場合はTRUEを返します。
			//
			const auto match = [](const std::wstring& text, const std::wstring& pattern) -> BOOL
			{
				if (pattern.empty()) return FALSE;

				return std::regex_search(text, std::wregex(pattern));
			};

//			if (class_name == L"#32768") return std::make_shared<menu_renderer_t>();
			if (class_name == L"#32770")
			{
				auto instance = (HINSTANCE)::GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
				auto comdlg32 = ::GetModuleHandleW(L"comdlg32.dll");

				// コモンダイアログの場合は
				if (instance == comdlg32)
					return std::make_shared<comdlg32::dialog_renderer_t>();

				return std::make_shared<dialog_renderer_t>();
			}

			if (class_name == WC_STATIC) return std::make_shared<static_renderer_t>();
			if (class_name == WC_BUTTON) return std::make_shared<button_renderer_t>();
			if (class_name == WC_EDIT) return std::make_shared<editbox_renderer_t>();
			if (class_name == RICHEDIT_CLASS) return std::make_shared<richedit_renderer_t>();
			if (class_name == WC_LISTBOX) return std::make_shared<listbox_renderer_t>();
			if (class_name == L"ListviewPopup") return std::make_shared<listbox_renderer_t>();
			if (class_name == L"ComboLBox") return std::make_shared<listbox_renderer_t>();
			if (class_name == WC_COMBOBOX) return std::make_shared<combobox_renderer_t>();
			if (class_name == WC_COMBOBOXEX) return std::make_shared<combobox_ex_renderer_t>();
			if (class_name == TOOLTIPS_CLASS) return std::make_shared<tooltip_renderer_t>();
			if (class_name == TRACKBAR_CLASS) return std::make_shared<trackbar_renderer_t>();
			if (class_name == UPDOWN_CLASS) return std::make_shared<spin_renderer_t>();
			if (class_name == WC_TABCONTROL) return std::make_shared<tab_renderer_t>();
			if (class_name == WC_HEADER) return std::make_shared<header_renderer_t>();
			if (class_name == WC_LISTVIEW) return std::make_shared<listview_renderer_t>();
			if (class_name == WC_TREEVIEW)
			{
				auto instance = (HINSTANCE)::GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
				auto ExplorerFrame = ::GetModuleHandleW(L"ExplorerFrame.dll");

				// エクスプローラのツリービューの場合は
				if (instance == ExplorerFrame)
				{
					// ウィンドウの縁にボーダーを追加します。
					my::modify_style(hwnd, 0, WS_BORDER);
//					my::modify_ex_style(hwnd, 0, WS_EX_CLIENTEDGE);
					::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
						SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
				}

				return std::make_shared<treeview_renderer_t>();
			}
			if (class_name == TOOLBARCLASSNAME) return std::make_shared<toolbar_renderer_t>();

			// aviutlのメインウィンドウまたはプラグインウィンドウです。
			if (class_name == _T("AviUtl")) return std::make_shared<aviutl_renderer_t>();

			// aviutlのボタンです。
			if (class_name == _T("AviUtlButton")) return std::make_shared<aviutl_button_renderer_t>();

			// 設定ダイアログです。
			if (class_name == _T("ExtendedFilterClass")) return std::make_shared<setting_dialog_renderer_t>();

			// ワークスペース化のコンテナです。
			if (class_name == _T("apn::workspace::container")) return std::make_shared<renderer_nc_t>();

			// ファイル選択ダイアログのツリービューの親ウィンドウです。
			if (class_name == L"NamespaceTreeControl") return std::make_shared<renderer_t>();

			// エクスプローラウィンドウです。
//			if (class_name == L"ExplorerBrowserControl") return std::make_shared<comdlg32::dui_view_renderer_t>();

			// ファイル選択ダイアログのリストビューなどの親ウィンドウです。
			if (class_name == L"DirectUIHWND") ::SetPropW(hwnd, renderer_t::k_prop_is_dialog, (HANDLE)TRUE);

			// ファイル選択ダイアログのリストビューの場合は
			if (class_name == L"SHELLDLL_DefView")
			{
				// ウィンドウの縁にボーダーを追加します。
				my::modify_style(hwnd, 0, WS_BORDER);
				::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			}

			// コモンダイアログが表示されている場合は
			if (hive.comdlg32_visible_count)
			{
				// ファイル選択ダイアログのリストビューなどの親ウィンドウです。
//				if (class_name == L"DirectUIHWND") return std::make_shared<comdlg32::direct_ui_hwnd_renderer_t>();

				// DirectUIHWNDの親ウィンドウです。
//				if (class_name == L"DUIViewWndClassName") return std::make_shared<comdlg32::dui_view_renderer_t>();

				// ファイル選択ダイアログのコンボボックスの親ウィンドウです。
//				if (class_name == L"FloatNotifySink") return std::make_shared<renderer_t>();

				// コマンドモジュールがドロップダウン表示するポップアップメニューのようなウィンドウです。
//				if (class_name == L"ViewControlClass") return std::make_shared<renderer_t>();

				// フォルダ選択ダイアログのツリービューの親ウィンドウです。
				if (class_name == L"SHBrowseForFolder ShellNameSpace Control") return std::make_shared<renderer_t>();

				// 「拡張 x264 出力(GUI) Ex」の設定ダイアログです。
				{
					//
					// この関数は指定文字列から始まる場合はTRUEを返します。
					//
					const auto starts_with = [](LPCWSTR lhs, LPCWSTR rhs) {
						return ::StrStrIW(lhs, rhs) == lhs;
					};

					// 検索対象のクラス名のプレフィックスです。
					const auto prefix = std::wstring(L"WindowsForms10.");

					// クラス名がプレフィックスから始まる場合は
					if (starts_with(class_name.c_str(), prefix.c_str()))
					{
						// クラス名のプレフィックス以降を取得します。
						auto p = &class_name[prefix.size()];

						if (starts_with(p, L"Window"))
						{
							if (my::get_style(hwnd) & WS_CAPTION)
								return std::make_shared<rigaya::dialog_renderer_t>();
							else
								return std::make_shared<dialog_renderer_t>();
						}

						if (starts_with(p, WC_BUTTON)) return std::make_shared<button_renderer_t>();
						if (starts_with(p, WC_EDIT)) return std::make_shared<editbox_renderer_t>();
						if (starts_with(p, TRACKBAR_CLASS)) return std::make_shared<trackbar_renderer_t>();
					}
				}
			}

			// クラスブラシを変更します。
			change_class_brush(hwnd);

			// 強制的に関連付ける場合は
			if (force)
			{
				// 汎用レンダラーを返します。
				return std::make_shared<renderer_t>();
			}

			// レンダラーが見つからなかった場合はnullptrを返します。
			return nullptr;
		}

		//
		// GDIレンダラーをウィンドウに関連付けされます。
		//
		void attach_renderer(HWND hwnd, BOOL force = FALSE)
		{
			// クラス名を取得します。
			auto class_name = my::get_class_name(hwnd);

			MY_TRACE_FUNC("{/hex}, {/}", hwnd, class_name);

			// ウィンドウに対応するGDIレンダラーが見つかった場合は
			if (auto renderer = find_renderer(hwnd, class_name, force))
			{
				// ウィンドウにGDIレンダラーを関連付けます。
				renderer->attach(hwnd);

				// 親ウィンドウが存在する場合は
				if (auto parent = ::GetParent(hwnd))
				{
					// 強制的に親ウィンドウにGDIレンダラーを関連付けます。
					attach_renderer(parent, TRUE);
				}

				// ウィンドウがキャプションを持つ場合は
				if (my::get_style(hwnd) & WS_CAPTION)
				{
					// レンダラーがロックされていない場合は
					if (!hive.is_renderer_locked)
						::SendMessage(hwnd, WM_ACTIVATE, FALSE, 0);
				}
				// ウィンドウがボタンの場合は
				else if (class_name == WC_BUTTON)
				{
					if (my::get_ex_style(hwnd) & WS_EX_STATICEDGE)
						hive.exedit.static_edge_buttons.emplace_back(hwnd);
				}
				// ウィンドウがスタティックテキストの場合は
				else if (class_name == WC_STATIC)
				{
					// ラウドネスメーター(全体)のスタティックテキストを微調整します。

					auto parent = ::GetParent(hwnd);
					auto window_name = my::get_window_text(parent);

					if (window_name == _T("ラウドネスメーター(全体)"))
						my::modify_style(hwnd, 0, SS_CENTERIMAGE);
				}
			}
		}
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto hwnd = hive.theme_window;

			// 既存のウィンドウにアタッチするために
			// カレントスレッドのトップレベルウィンドウを列挙します。
			::EnumThreadWindows(::GetCurrentThreadId(), [](HWND hwnd, LPARAM l_param)
			{
				// トップレベルウィンドウにレンダラーをアタッチします。
				manager.attach_renderer(hwnd);

				// トップレベルウィンドウの子孫ウィンドウを列挙します。
				::EnumChildWindows(hwnd, [](HWND hwnd, LPARAM l_param)
				{
					// 子孫ウィンドウにレンダラーをアタッチします。
					manager.attach_renderer(hwnd);

					// 列挙を続けます。
					return TRUE;
				}, 0);

				// 列挙を続けます。
				return TRUE;
			}, 0);

			// aviutl2ウィンドウがアクティブな場合は
			// WM_ACTIVATEを送信してNC領域の配色を更新させます。
			if (::GetActiveWindow() == hwnd)
				::SendMessage(hwnd, WM_ACTIVATE, TRUE, 0);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 現存するすべてのレンダラーを削除します。
			renderer_t::clear();

			return TRUE;
		}
	} manager;
}
