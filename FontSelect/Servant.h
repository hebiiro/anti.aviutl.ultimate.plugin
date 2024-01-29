#pragma once
#include "Hive.h"
#include "Config.h"
#include "Paint.h"
#include "SettingDialog.h"
#include "MainWindow.h"

namespace fgo::font_select
{
	//
	// このクラスはフォントを選択するためのカスタムメニューを表示します。
	//
	inline struct FontSelect : Servant, Magi::CommandID::Plugin
	{
		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return Hive::Name;
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return Hive::DisplayName;
		}

		//
		// この仮想関数は、DLLの初期化のタイミングで呼ばれます。
		//
		BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			hive.instance = instance;

			return TRUE;
		}

		//
		// この仮想関数は、DLLの後始末のタイミングで呼ばれます。
		//
		BOOL on_dll_exit(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			return TRUE;
		}

		//
		// AviUtlにメニューアイテムを追加します。
		//
		BOOL addMenuItem(AviUtl::FilterPlugin* fp, LPCWSTR text, UINT id)
		{
			char itemText[MAX_PATH] = {};
			::StringCchPrintfA(itemText, std::size(itemText), "[%ws]%ws", Hive::DisplayName, text);

			return fp->exfunc->add_menu_item(fp, itemText, mainWindow.mainDialog, id, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
		}

		//
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (!paint.init()) return FALSE;
			if (!settingDialog.init()) return FALSE;
			if (!mainWindow.init()) return FALSE;
			if (!config.init()) return FALSE;

			hive.mainWindow = mainWindow;

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			config.exit();
			mainWindow.exit();
			settingDialog.exit();
			paint.exit();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウのコマンドのタイミングで呼ばれます。
		//
		BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
			{
			case ID_ADDIN:
				{
					if (::IsWindow(mainWindow))
						mainWindow.show();

					break;
				}
			}

			return FALSE;
		}
	} servant;
}
