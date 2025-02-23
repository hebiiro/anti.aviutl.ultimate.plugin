#pragma once

namespace apn
{
	//
	// このクラスはプラグインです。
	//
	inline struct Plugin
	{
		BOOL init(HINSTANCE instance)
		{
			MY_TRACE_FUNC("{:#010x}", instance);

			if (!hook_manager.init()) return FALSE;
			if (!config_io.init()) return FALSE;
			if (!env.init()) return FALSE;
			if (!addin_loader.init()) return FALSE;
			if (!addin_manager.fire_dll_init()) return FALSE;

			return TRUE;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");

			addin_manager.fire_dll_exit();
			addin_loader.exit();
			env.exit();
			config_io.exit();
			hook_manager.exit();

			return TRUE;
		}

		BOOL func_init(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE_FUNC("");

			magi.init(fp);
			plugin_window.init(fp);
			addin_manager.fire_init();

			return TRUE;
		}

		BOOL func_exit(AviUtl::FilterPlugin* fp)
		{
			MY_TRACE_FUNC("");

			addin_manager.fire_exit();
			plugin_window.exit();
			magi.exit();

			return TRUE;
		}

		BOOL func_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
//			MY_TRACE_FUNC("");

			return addin_manager.fire_video_proc(fp, fpip);
		}

		BOOL func_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
//			MY_TRACE_FUNC("");

			return addin_manager.fire_audio_proc(fp, fpip);
		}

		BOOL func_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
		{
			auto result = addin_manager.fire_window_proc(hwnd, message, wParam, lParam, editp, fp);

			switch (message)
			{
			case AviUtl::FilterPlugin::WindowMessage::Init:
				{
					MY_TRACE_FUNC("Init, {:#010x}, {:#010x}", wParam, lParam);

					// このウィンドウにカテゴリ名を設定します。
					::SetProp(hwnd, _T("aviutl.plugin.category_name"), (HANDLE)L"アルティメット");

					result |= addin_manager.fire_window_init(hwnd, message, wParam, lParam, editp, fp);

					break;
				}
			case AviUtl::FilterPlugin::WindowMessage::Exit:
				{
					MY_TRACE_FUNC("Exit, {:#010x}, {:#010x}", wParam, lParam);

					result |= addin_manager.fire_window_exit(hwnd, message, wParam, lParam, editp, fp);

					break;
				}
			}

			return result | plugin_window.func_wnd_proc(hwnd, message, wParam, lParam, editp, fp);
		}

		BOOL func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
		{
			return addin_manager.fire_update(fp, status);
		}

		BOOL func_save_start(AviUtl::FilterPlugin* fp, int32_t s, int32_t e, AviUtl::EditHandle* editp)
		{
			return addin_manager.fire_save_start(fp, s, e, editp);
		}

		BOOL func_save_end(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
		{
			return addin_manager.fire_save_end(fp, editp);
		}

		BOOL func_is_save_frame(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t saveno, int32_t frame, int32_t fps, int32_t edit_flag, int32_t inter)
		{
			return addin_manager.fire_is_saveframe(fp, editp, saveno, frame, fps, edit_flag, inter);
		}

		BOOL func_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size)
		{
			return addin_manager.fire_project_load(fp, editp, data, size);
		}

		BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size)
		{
			return addin_manager.fire_project_save(fp, editp, data, size);
		}

		BOOL func_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t frame, LPSTR title, int32_t max_title)
		{
			return addin_manager.fire_modify_title(fp, editp, frame, title, max_title);
		}

		BOOL DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
		{
			switch (reason)
			{
			case DLL_PROCESS_ATTACH:
				{
					MY_TRACE_FUNC("DLL_PROCESS_ATTACH");

					::DisableThreadLibraryCalls(magi.instance = hive.instance = instance);

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
	} plugin;
}
