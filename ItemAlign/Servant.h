#pragma once
#include "MainWindow.h"

namespace fgo::item_align
{
	//
	// このクラスはタイムラインアイテムの位置を変化させます。
	//
	inline struct ItemAlign : Servant, Magi::CommandID::Plugin
	{
		MainWindow mainWindow;

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
			if (::GetModuleHandleW(L"BuildStairs.auf"))
			{
				::MessageBoxW(hwnd,
					L"ItemAlignとBuildStairs.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					Hive::DisplayName, MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!mainWindow.create()) return FALSE;

			hive.mainWindow = mainWindow;

			addMenuItem(fp, L"選択アイテムをずらす", IDC_SHIFT);
			addMenuItem(fp, L"現在位置を選択アイテムの先頭に移動", IDC_MOVE_TO_BEGIN);
			addMenuItem(fp, L"現在位置を選択アイテムの最後に移動", IDC_MOVE_TO_END);
			addMenuItem(fp, L"選択アイテムを左に詰める", IDC_MOVE_BEGIN);
			addMenuItem(fp, L"選択アイテムを右に詰める", IDC_MOVE_END);
			addMenuItem(fp, L"選択アイテムの左に伸ばす", IDC_STRETCH_BEGIN);
			addMenuItem(fp, L"選択アイテムの右に伸ばす", IDC_STRETCH_END);
			addMenuItem(fp, L"現在位置の近くにある中間点を削除", IDC_DELETE_MIDPT);

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			save();
			mainWindow.destroy();

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
					if (mainWindow)
						Tools::AviUtl::PluginWindowExtension::show(mainWindow);

					break;
				}
			}

			return FALSE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"ItemAlign.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			getPrivateProfileInt(path, L"Config", L"frame", hive.frame);
			getPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			setPrivateProfileInt(path, L"Config", L"frame", hive.frame);
			setPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}
	} servant;
}
