#pragma once
#include "Manager.h"
#include "Element/Menu.h"
#include "Element/ScrollBar.h"
#include "Element/Static.h"
#include "Element/Button.h"
#include "Element/EditBox.h"
#include "Element/ComboBox.h"
#include "Element/ListBox.h"
#include "Element/ToolTip.h"
#include "Element/TrackBar.h"
#include "Element/Spin.h"
#include "Element/Header.h"
#include "Element/ListView.h"
#include "Element/TreeView.h"
#include "Element/ToolBar.h"
#include "Element/Tab.h"
#include "Element/CommandModule.h"
#include "Element/PreviewPane.h"
#include "Element/CommonItemsDialog.h"
#include "Element/Window.h"

namespace fgo::dark::theme
{
	//
	// このクラスはテーマレンダラの利用者です。
	//
	inline struct Client
	{
		//
		// テーマレンダラの利用を開始します。
		//
		BOOL init(HWND hwnd)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			manager.addRendererNC(hwnd, VSCLASS_MENU, &menuRenderer);
			manager.addRenderer(hwnd, VSCLASS_MENU, &menuRenderer);
			manager.addRenderer(hwnd, VSCLASS_SCROLLBAR, &scrollbarRenderer);
			manager.addRenderer(hwnd, VSCLASS_STATIC, &staticRenderer);
			manager.addRenderer(hwnd, VSCLASS_BUTTON, &buttonRenderer);
			manager.addRenderer(hwnd, VSCLASS_EDIT, &editboxRenderer);
			manager.addRenderer(hwnd, VSCLASS_LISTBOX, &listboxRenderer);
			manager.addRenderer(hwnd, VSCLASS_COMBOBOX, &comboboxRenderer);
			manager.addRenderer(hwnd, VSCLASS_TOOLTIP, &tooltipRenderer);
			manager.addRenderer(hwnd, VSCLASS_TRACKBAR, &trackbarRenderer);
			manager.addRenderer(hwnd, VSCLASS_SPIN, &spinRenderer);
			manager.addRenderer(hwnd, VSCLASS_HEADER, &headerRenderer);
			manager.addRenderer(hwnd, VSCLASS_LISTVIEW, &listviewRenderer);
			manager.addRenderer(hwnd, VSCLASS_TREEVIEW, &treeviewRenderer);
			manager.addRenderer(hwnd, VSCLASS_TOOLBAR, &toolbarRenderer);
			manager.addRenderer(hwnd, VSCLASS_TAB, &tabRenderer);
			manager.addRenderer(hwnd, VSCLASS_WINDOW, &windowRenderer);

			manager.addRendererNC(hwnd, L"Explorer::" VSCLASS_MENU, &menuRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_MENU, &menuRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_SCROLLBAR, &scrollbarRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_STATIC, &staticRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_BUTTON, &buttonRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_EDIT, &editboxRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_LISTBOX, &listboxRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_COMBOBOX, &comboboxRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_TOOLTIP, &tooltipRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_TRACKBAR, &trackbarRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_SPIN, &spinRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_HEADER, &headerRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_LISTVIEW, &listviewRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_TREEVIEW, &treeviewRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_TOOLBAR, &toolbarRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_TAB, &tabRenderer);
			manager.addRenderer(hwnd, L"Explorer::" VSCLASS_WINDOW, &windowRenderer);
#if 1
			manager.addRenderer(hwnd, L"Combobox::" VSCLASS_LISTBOX, &listboxRenderer);
			manager.addRenderer(hwnd, L"CFD::" VSCLASS_COMBOBOX, &comboboxRenderer);
			manager.addRenderer(hwnd, L"CFD::" VSCLASS_HEADER, &headerRenderer);
			manager.addRenderer(hwnd, L"CFD::" VSCLASS_LISTVIEW, &listviewRenderer);
			manager.addRenderer(hwnd, L"CFD::" VSCLASS_TREEVIEW, &treeviewRenderer);
			manager.addRendererNC(hwnd, L"ItemsView::" VSCLASS_HEADER, &headerRenderer);
			manager.addRendererNC(hwnd, L"ItemsView::" VSCLASS_LISTVIEW, &listviewRenderer);
			manager.addRenderer(hwnd, L"Placesbar::" VSCLASS_TOOLBAR, &toolbarRenderer);
			manager.addRenderer(hwnd, L"AddressComposited::" VSCLASS_COMBOBOX, &comboboxRenderer);
			manager.addRendererNC(hwnd, L"CommandModule", &commandModuleRenderer);
			manager.addRendererNC(hwnd, L"PreviewPane", &previewPaneRenderer);
//			manager.addRendererNC(hwnd, L"NavbarComposited::" L"PreviewPane", &previewPaneRenderer);
//			manager.addRendererNC(hwnd, L"InactiveNavbarComposited::" L"PreviewPane", &previewPaneRenderer);
			manager.addRendererNC(hwnd, L"CommonItemsDialog", &commonItemsDialogRenderer);
#endif
			return TRUE;
		}
	} client;
}
