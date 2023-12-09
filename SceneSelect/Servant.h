#pragma once
#include "MainWindow.h"

namespace fgo::scene_select
{
	//
	// このクラスはタイムラインアイテムの位置を変化させます。
	//
	inline struct SceneSelect : Servant, Magi::CommandID::Plugin
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
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (::GetModuleHandleW(L"SelectScene.auf"))
			{
				::MessageBoxW(hwnd,
					L"SceneSelectとSelectScene.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					hive.DisplayName, MB_OK | MB_ICONWARNING);
			}

			if (!mainWindow.init()) return FALSE;

			hive.mainWindow = mainWindow;

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
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
