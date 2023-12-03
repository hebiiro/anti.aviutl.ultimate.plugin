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
			MY_TRACE_FUNC("0x%08X", instance);

			mystic_code.init();
			master.init(instance);

			return TRUE;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			master.exit();
			mystic_code.exit();

			return TRUE;
		}

		BOOL func_init(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE_FUNC("");

			magi.init(fp);
			spiritron.init(fp);
			fate.fire_init();

			return TRUE;
		}

		BOOL func_exit(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE_FUNC("");

			fate.fire_exit();
			spiritron.exit();
			magi.exit();

			return TRUE;
		}

		BOOL func_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
//			MY_TRACE_FUNC("");

			return fate.fire_video_proc(fp, fpip);
		}

		BOOL func_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
//			MY_TRACE_FUNC("");

			return fate.fire_audio_proc(fp, fpip);
		}

		BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			BOOL result = fate.fire_window_proc(hwnd, message, wParam, lParam, editp, fp);

			switch (message)
			{
			case AviUtl::FilterPlugin::WindowMessage::Init:
				{
					MY_TRACE_FUNC("Init, 0x%08X, 0x%08X", wParam, lParam);

					result |= fate.fire_window_init(hwnd, message, wParam, lParam, editp, fp);

					break;
				}
			case AviUtl::FilterPlugin::WindowMessage::Exit:
				{
					MY_TRACE_FUNC("Exit, 0x%08X, 0x%08X", wParam, lParam);

					result |= fate.fire_window_exit(hwnd, message, wParam, lParam, editp, fp);

					break;
				}
			}

			return result | spiritron.func_WndProc(hwnd, message, wParam, lParam, editp, fp);
		}

		BOOL func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
		{
			return fate.fire_update(fp, status);
		}

		BOOL func_save_start(AviUtl::FilterPlugin* fp, int32_t s, int32_t e, AviUtl::EditHandle* editp)
		{
			return fate.fire_save_start(fp, s, e, editp);
		}

		BOOL func_save_end(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
		{
			return fate.fire_save_end(fp, editp);
		}

		BOOL func_is_saveframe(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t saveno, int32_t frame, int32_t fps, int32_t edit_flag, int32_t inter)
		{
			return fate.fire_is_saveframe(fp, editp, saveno, frame, fps, edit_flag, inter);
		}

		BOOL func_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size)
		{
			return fate.fire_project_load(fp, editp, data, size);
		}

		BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size)
		{
			return fate.fire_project_save(fp, editp, data, size);
		}

		BOOL func_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, LPSTR title, int32_t max_title)
		{
			return fate.fire_modify_title(fp, editp, title, max_title);
		}

		BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
		{
			switch (reason)
			{
			case DLL_PROCESS_ATTACH:
				{
					MY_TRACE_FUNC("DLL_PROCESS_ATTACH");

					::DisableThreadLibraryCalls(instance);

					break;
				}
			case DLL_PROCESS_DETACH:
				{
					MY_TRACE_FUNC("DLL_PROCESS_DETACH");

					break;
				}
			}

			return TRUE;
		}
	} story;
}
