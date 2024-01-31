#pragma once
#include "Hive.h"
#include "Config.h"
#include "MainWindow.h"

namespace fgo::single_out
{
	//
	// このクラスはフレームまたはアイテムの画像をエクスポートします。
	//
	inline struct SingleOut : Servant, Magi::CommandID::Plugin
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
			if (::GetModuleHandleW(L"SaveImage.auf"))
			{
				::MessageBoxW(hwnd,
					L"SingleOutとSaveImage.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					Hive::DisplayName, MB_OK | MB_ICONWARNING);
			}

			if (!mainWindow.init()) return FALSE;
			if (!config.init()) return FALSE;

			hive.mainWindow = mainWindow;

			addMenuItem(fp, L"現在フレームを保存", IDC_EXPORT_FRAME_RGB);
			addMenuItem(fp, L"現在フレームをアルファ付きで保存", IDC_EXPORT_FRAME_RGBA);
			addMenuItem(fp, L"選択アイテムを保存", IDC_EXPORT_ITEM_RGB);
			addMenuItem(fp, L"選択アイテムをアルファ付きで保存", IDC_EXPORT_ITEM_RGBA);

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			config.exit();
			mainWindow.exit();

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
