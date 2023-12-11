#pragma once
#include "Hook.h"

namespace fgo::dialog_size
{
	//
	// このクラスはダイアログのサイズを調整します。
	//
	inline struct DialogSize : Servant
	{
		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"DialogSize";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"ダイアログサイズ調整";
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
			if (::GetModuleHandleW(L"BigDialog.aul"))
			{
				::MessageBoxW(hwnd,
					L"DialogSizeとBigDialog.aulが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			hook.init();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			hook.exit();

			return FALSE;
		}
	} servant;
}
