#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはテーマレンダラーを管理します。
	//
	inline struct manager_t : entry_t::node_t
	{
		my::theme::unique_ptr<> shell_status_bar_separator;

		virtual BOOL on_init() { return init(); }
		virtual BOOL on_exit() { return exit(); }

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto hwnd = hive.theme_window;

			// テーマハンドルとレンダラーを関連付けます。
			// テーマハンドルを開くとフック関数内でレンダラーと関連付けされます。
			{
				// ウィンドウのDPIを取得します。
				auto dpi = ::GetDpiForWindow(hwnd);
				MY_TRACE_INT(dpi);

				//
				// この関数はテーマレンダラーを登録します。
				//
				const auto register_renderer = [&](LPCWSTR class_list, renderer_t* renderer)
				{
					// レンダラーをマップに追加します。
					from_vsclass.set(class_list, renderer);

					// テーマハンドルを開きます。
					my::theme::unique_ptr<> theme(::OpenThemeData(hwnd, class_list));
					MY_TRACE("{/} = {/hex}\n", class_list, theme.get());

					// 高DPI用のテーマハンドルを開きます。
					my::theme::unique_ptr<> theme_for_dpi(::OpenThemeDataForDpi(hwnd, class_list, dpi));
					MY_TRACE("{/} {/} = {/hex}\n", class_list, dpi, theme_for_dpi.get());

					// テーマハンドルを閉じるとハンドル値が変わってしまうので、閉じないようにします。
					theme.release();
					theme_for_dpi.release();
				};

				// 通常のテーマ
				register_renderer(VSCLASS_MENU, &menu_renderer);
				register_renderer(VSCLASS_SCROLLBAR, &scrollbar_renderer);
				register_renderer(VSCLASS_STATIC, &static_renderer);
				register_renderer(VSCLASS_BUTTON, &button_renderer);
				register_renderer(VSCLASS_EDIT, &editbox_renderer);
				register_renderer(VSCLASS_COMBOBOX, &combobox_renderer);
				register_renderer(VSCLASS_LISTBOX, &listbox_renderer);
				register_renderer(VSCLASS_TOOLTIP, &tooltip_renderer);
				register_renderer(VSCLASS_TRACKBAR, &trackbar_renderer);
				register_renderer(VSCLASS_SPIN, &spin_renderer);
				register_renderer(VSCLASS_TAB, &tab_renderer);
				register_renderer(VSCLASS_TOOLBAR, &toolbar_renderer);
				register_renderer(VSCLASS_HEADER, &header_renderer);
				register_renderer(VSCLASS_LISTVIEW, &listview_renderer);
				register_renderer(VSCLASS_TREEVIEW, &treeview_renderer);
				register_renderer(VSCLASS_REBAR, &rebar_renderer);
				register_renderer(VSCLASS_WINDOW, &window_renderer);
				register_renderer(L"Explorer::" VSCLASS_MENU, &menu_renderer);
				register_renderer(L"Explorer::" VSCLASS_SCROLLBAR, &scrollbar_renderer);
				register_renderer(L"Explorer::" VSCLASS_STATIC, &static_renderer);
				register_renderer(L"Explorer::" VSCLASS_BUTTON, &button_renderer);
				register_renderer(L"Explorer::" VSCLASS_EDIT, &editbox_renderer);
				register_renderer(L"Explorer::" VSCLASS_COMBOBOX, &combobox_renderer);
				register_renderer(L"Explorer::" VSCLASS_LISTBOX, &listbox_renderer);
				register_renderer(L"Explorer::" VSCLASS_TOOLTIP, &tooltip_renderer);
				register_renderer(L"Explorer::" VSCLASS_TRACKBAR, &trackbar_renderer);
				register_renderer(L"Explorer::" VSCLASS_SPIN, &spin_renderer);
				register_renderer(L"Explorer::" VSCLASS_TAB, &tab_renderer);
				register_renderer(L"Explorer::" VSCLASS_TOOLBAR, &toolbar_renderer);
				register_renderer(L"Explorer::" VSCLASS_HEADER, &header_renderer);
				register_renderer(L"Explorer::" VSCLASS_LISTVIEW, &listview_renderer);
				register_renderer(L"Explorer::" VSCLASS_TREEVIEW, &treeview_renderer);
				register_renderer(L"Explorer::" VSCLASS_REBAR, &rebar_renderer);
				register_renderer(L"Explorer::" VSCLASS_WINDOW, &window_renderer);

				// ファイル選択ダイアログのテーマ
				{
					// リストビュー
					register_renderer(L"ItemsView", &itemsview_renderer);
					register_renderer(L"ItemsView::" VSCLASS_HEADER, &header_renderer);
					register_renderer(L"ItemsView::" VSCLASS_LISTVIEW, &listview_renderer);
					register_renderer(L"ItemsViewAccessible::" VSCLASS_HEADER, &header_renderer); // ヘッダーのドロップダウンボタンをクリックしたときに使用されます。

					// その他のコントロール
					register_renderer(L"Button-OK;Button", &button_renderer);
					register_renderer(L"Button-CANCEL;Button", &button_renderer);
					register_renderer(L"Combobox::" VSCLASS_LISTBOX, &listbox_renderer);
//					register_renderer(L"CFD::" VSCLASS_EDIT, &editbox_renderer);
					register_renderer(L"CFD::" VSCLASS_COMBOBOX, &combobox_renderer);
					register_renderer(L"CFD::" VSCLASS_HEADER, &header_renderer);
					register_renderer(L"CFD::" VSCLASS_LISTVIEW, &listview_renderer);
					register_renderer(L"CFD::" VSCLASS_TREEVIEW, &treeview_renderer);
//					register_renderer(L"EditComposited::" VSCLASS_EDIT, &editbox_renderer);
					register_renderer(L"Placesbar::" VSCLASS_TOOLBAR, &toolbar_renderer);
					register_renderer(L"AddressComposited::" VSCLASS_COMBOBOX, &combobox_renderer);

					// ツリービューの上に配置されているコントロール (厳密にはウィンドウではない)
					register_renderer(L"CommandModule", &command_module_renderer);
//					register_renderer(L"CPLCommandModule::CommandModule", &command_module_renderer);
//					register_renderer(L"ItemsView::CommandModule", &command_module_renderer);

					// 不明
					register_renderer(L"CommonItemsDialog", &common_items_dialog_renderer);
//					register_renderer(L"BBComposited::" VSCLASS_TOOLBAR, &toolbar_renderer);
//					register_renderer(L"BBComposited::" VSCLASS_TOOLTIP, &tooltip_renderer);
					register_renderer(L"PreviewPane", &preview_pane_renderer);
//					register_renderer(L"NavbarComposited::" L"PreviewPane", &preview_pane_renderer);
//					register_renderer(L"InactiveNavbarComposited::" L"PreviewPane", &preview_pane_renderer);

					// 上部のリバーのバンド群
					register_renderer(L"ExplorerNavPane", &explorer_nav_pane_renderer);

					// 一番左のバンド (トラベルバンド)
//					register_renderer(L"Navigation", &navigation_renderer);
// 
					// 中央のバンド (アドレスバンド)
					register_renderer(L"AddressBand", &address_band_renderer);

					// 一番右のバンド (サーチバンド)
					register_renderer(L"SearchBox", &search_box_renderer);
					register_renderer(L"SearchBoxComposited::SearchBox", &search_box_renderer);
					register_renderer(L"SearchEditBox", &editbox_renderer);
				}

				// シェルのテーマ
				register_renderer(L"ImmersiveStart::" VSCLASS_MENU, &immersive_start::menu_renderer);

				// エクスプローラのステータスバーのセパレータ
				shell_status_bar_separator.reset(::OpenThemeDataForDpi(hwnd, L"ShellStatusBarSeparator", dpi));
#if 0
				// ウィンドウを再描画してカスタムテーマを反映させます。
				::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
					SWP_FRAMECHANGED | SWP_NOCOPYBITS);
#endif
			}

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
	} manager;
}
