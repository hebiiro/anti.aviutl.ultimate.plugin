#pragma once
#include "Master.h"
#include "MysticCode.h"
#include "Spiritron.h"

namespace fgo
{
	//
	// このクラスはこのプログラムの全体的な流れです。
	//
	inline struct Story
	{
		BOOL init(HINSTANCE instance)
		{
			MY_TRACE(_T("Story::init(0x%08X)\n"), instance);

			mystic_code.init();
			master.init(instance);

			return TRUE;
		}

		BOOL exit()
		{
			MY_TRACE(_T("Story::exit()\n"));

			master.exit();
			mystic_code.exit();

			return TRUE;
		}

		BOOL func_init(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE(_T("Story::func_init()\n"));

			magi.init(fp);
			spiritron.init(fp);
			fate.fire_init();

			return TRUE;
		}

		BOOL func_exit(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE(_T("Story::func_exit()\n"));

			fate.fire_exit();
			spiritron.exit();
			magi.exit();

			return TRUE;
		}

		BOOL func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			MY_TRACE(_T("Story::func_proc()\n"));

			return FALSE;
		}

		BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			switch (message)
			{
			case AviUtl::FilterPlugin::WindowMessage::Init:
				{
					MY_TRACE(_T("Story::func_WndProc(Init, 0x%08X, 0x%08X)\n"), wParam, lParam);

					return fate.fire_window_init(hwnd, message, wParam, lParam, editp, fp);
				}
			case AviUtl::FilterPlugin::WindowMessage::Exit:
				{
					MY_TRACE(_T("Story::func_WndProc(Exit, 0x%08X, 0x%08X)\n"), wParam, lParam);

					return fate.fire_window_exit(hwnd, message, wParam, lParam, editp, fp);
				}
			}

			return spiritron.func_WndProc(hwnd, message, wParam, lParam, editp, fp);
		}

		BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
		{
			switch (reason)
			{
			case DLL_PROCESS_ATTACH:
				{
					MY_TRACE(_T("DLL_PROCESS_ATTACH\n"));

					::DisableThreadLibraryCalls(instance);

					break;
				}
			case DLL_PROCESS_DETACH:
				{
					MY_TRACE(_T("DLL_PROCESS_DETACH\n"));

					break;
				}
			}

			return TRUE;
		}
	} story;
}
