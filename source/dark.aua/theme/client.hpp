#pragma once

namespace apn::dark::theme
{
	//
	// このクラスはテーマレンダラの利用者です。
	//
	inline struct Client
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto hwnd = hive.main_window;
			MY_TRACE_HWND(hwnd);

			manager.add_renderer_nc(hwnd, VSCLASS_MENU, &menu_renderer);
			manager.add_renderer(hwnd, VSCLASS_MENU, &menu_renderer);
			manager.add_renderer(hwnd, VSCLASS_SCROLLBAR, &scrollbar_renderer);
			manager.add_renderer(hwnd, VSCLASS_STATIC, &static_renderer);
			manager.add_renderer(hwnd, VSCLASS_BUTTON, &button_renderer);
			manager.add_renderer(hwnd, VSCLASS_EDIT, &editbox_renderer);
			manager.add_renderer(hwnd, VSCLASS_LISTBOX, &listbox_renderer);
			manager.add_renderer(hwnd, VSCLASS_COMBOBOX, &combobox_renderer);
			manager.add_renderer(hwnd, VSCLASS_TOOLTIP, &tooltip_renderer);
			manager.add_renderer(hwnd, VSCLASS_TRACKBAR, &trackbar_renderer);
			manager.add_renderer(hwnd, VSCLASS_SPIN, &spin_renderer);
			manager.add_renderer(hwnd, VSCLASS_HEADER, &header_renderer);
			manager.add_renderer(hwnd, VSCLASS_LISTVIEW, &listview_renderer);
			manager.add_renderer(hwnd, VSCLASS_TREEVIEW, &treeview_renderer);
			manager.add_renderer(hwnd, VSCLASS_TOOLBAR, &toolbar_renderer);
			manager.add_renderer(hwnd, VSCLASS_TAB, &tab_renderer);
			manager.add_renderer(hwnd, VSCLASS_WINDOW, &window_renderer);

			manager.add_renderer_nc(hwnd, L"Explorer::" VSCLASS_MENU, &menu_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_MENU, &menu_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_SCROLLBAR, &scrollbar_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_STATIC, &static_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_BUTTON, &button_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_EDIT, &editbox_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_LISTBOX, &listbox_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_COMBOBOX, &combobox_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_TOOLTIP, &tooltip_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_TRACKBAR, &trackbar_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_SPIN, &spin_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_HEADER, &header_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_LISTVIEW, &listview_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_TREEVIEW, &treeview_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_TOOLBAR, &toolbar_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_TAB, &tab_renderer);
			manager.add_renderer(hwnd, L"Explorer::" VSCLASS_WINDOW, &window_renderer);
#if 1
			manager.add_renderer(hwnd, L"Combobox::" VSCLASS_LISTBOX, &listbox_renderer);
			manager.add_renderer(hwnd, L"CFD::" VSCLASS_COMBOBOX, &combobox_renderer);
			manager.add_renderer(hwnd, L"CFD::" VSCLASS_HEADER, &header_renderer);
			manager.add_renderer(hwnd, L"CFD::" VSCLASS_LISTVIEW, &listview_renderer);
			manager.add_renderer(hwnd, L"CFD::" VSCLASS_TREEVIEW, &treeview_renderer);
			manager.add_renderer_nc(hwnd, L"ItemsView::" VSCLASS_HEADER, &header_renderer);
			manager.add_renderer_nc(hwnd, L"ItemsView::" VSCLASS_LISTVIEW, &listview_renderer);
			manager.add_renderer(hwnd, L"Placesbar::" VSCLASS_TOOLBAR, &toolbar_renderer);
			manager.add_renderer(hwnd, L"AddressComposited::" VSCLASS_COMBOBOX, &combobox_renderer);
			manager.add_renderer_nc(hwnd, L"CommandModule", &command_module_renderer);
//			manager.add_renderer_nc(hwnd, L"ItemsView::CommandModule", &command_module_renderer);
			manager.add_renderer_nc(hwnd, L"PreviewPane", &preview_pane_renderer);
//			manager.add_renderer_nc(hwnd, L"NavbarComposited::" L"PreviewPane", &preview_pane_renderer);
//			manager.add_renderer_nc(hwnd, L"InactiveNavbarComposited::" L"PreviewPane", &preview_pane_renderer);
			manager.add_renderer_nc(hwnd, L"CommonItemsDialog", &common_items_dialog_renderer);
//			manager.add_renderer(hwnd, L"ImmersiveStart::" VSCLASS_MENU, &menu_renderer);
//			manager.add_renderer(hwnd, L"BBComposited::" VSCLASS_TOOLBAR, &toolbar_renderer);
//			manager.add_renderer(hwnd, L"Rebar", &rebar_renderer);
//			manager.add_renderer(hwnd, L"Navigation", &navigation_renderer);
//			manager.add_renderer(hwnd, L"AddressBand", &address_band_renderer);
#endif
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
	} client;
}
