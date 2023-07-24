#pragma once
#include "Master.h"

namespace fgo
{
	//
	// このクラスはこのプログラムの全体的な流れです。
	//
	inline struct Story
	{
		BOOL func_init(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE(_T("Story::func_init()\n"));

			fate.init(fp);
			master.init();
			grand_order.init();

			return TRUE;
		}

		BOOL func_exit(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE(_T("Story::func_exit()\n"));

			grand_order.exit();
			master.exit();
			fate.exit();

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

					break;
				}
			case AviUtl::FilterPlugin::WindowMessage::Exit:
				{
					MY_TRACE(_T("Story::func_WndProc(Exit, 0x%08X, 0x%08X)\n"), wParam, lParam);

					break;
				}
			case AviUtl::FilterPlugin::WindowMessage::Command:
				{
					MY_TRACE(_T("Story::func_WndProc(Command, 0x%08X, 0x%08X)\n"), wParam, lParam);

					if (wParam == 0 && lParam == 0) return TRUE;

					break;
				}
			}

			return FALSE;
		}

		BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
		{
			switch (reason)
			{
			case DLL_PROCESS_ATTACH:
				{
					MY_TRACE(_T("DLL_PROCESS_ATTACH\n"));

					// 参照カウンタを増やしてこの DLL がアンロードされないようにします。
					WCHAR moduleFileName[MAX_PATH] = {};
					::GetModuleFileNameW(instance, moduleFileName, MAX_PATH);
					::LoadLibraryW(moduleFileName);

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
