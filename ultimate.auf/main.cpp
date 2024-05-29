#include "pch.h"
#include "hive.hpp"
#include "hook.hpp"
#include "env.hpp"
#include "addin_loader.hpp"
#include "plugin_window.hpp"
#include "config_io.hpp"
#include "plugin.hpp"

namespace apn
{
	static const auto plugin_version = "r26"s;
	static const auto video_filter_name = "アルティメットプラグイン"s;
	static const auto video_filter_information = video_filter_name + " " + plugin_version;
	static const auto audio_filter_name = video_filter_name + "(音声)";
	static const auto audio_filter_information = audio_filter_name + " " + plugin_version;

	//
	// 初期化
	//
	BOOL func_init(AviUtl::FilterPlugin* fp)
	{
		return plugin.func_init(fp);
	}

	//
	// 終了
	//
	BOOL func_exit(AviUtl::FilterPlugin* fp)
	{
		return plugin.func_exit(fp);
	}

	//
	// 映像フィルタ関数
	//
	BOOL func_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
	{
		return plugin.func_video_proc(fp, fpip);
	}

	//
	// 音声フィルタ関数
	//
	BOOL func_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
	{
		return plugin.func_audio_proc(fp, fpip);
	}

	//
	// ウィンドウ関数
	//
	BOOL func_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
	{
		return plugin.func_wnd_proc(hwnd, message, wParam, lParam, editp, fp);
	}

	BOOL func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
	{
		return plugin.func_update(fp, status);
	}

	BOOL func_save_start(AviUtl::FilterPlugin* fp, int32_t s, int32_t e, AviUtl::EditHandle* editp)
	{
		return plugin.func_save_start(fp, s, e, editp);
	}

	BOOL func_save_end(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
	{
		return plugin.func_save_end(fp, editp);
	}

	BOOL func_is_save_frame(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t saveno, int32_t frame, int32_t fps, int32_t edit_flag, int32_t inter)
	{
		return plugin.func_is_save_frame(fp, editp, saveno, frame, fps, edit_flag, inter);
	}

	BOOL func_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size)
	{
		return plugin.func_project_load(fp, editp, data, size);
	}

	BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size)
	{
		return plugin.func_project_save(fp, editp, data, size);
	}

	BOOL func_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t frame, LPSTR title, int32_t max_title)
	{
		return plugin.func_modify_title(fp, editp, frame, title, max_title);
	}

	//
	// エントリポイント
	//
	EXTERN_C BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
	{
		return plugin.DllMain(instance, reason, reserved);
	}

	AviUtl::FilterPluginDLL** WINAPI GetFilterTableList()
	{
		MY_TRACE_FUNC("");

		static AviUtl::FilterPluginDLL video_filter = {
			.flag =
				AviUtl::FilterPlugin::Flag::AlwaysActive |
				AviUtl::FilterPlugin::Flag::WindowThickFrame |
				AviUtl::FilterPlugin::Flag::WindowSize |
				AviUtl::FilterPlugin::Flag::ExInformation,
			.x = 400,
			.y = 400,
			.name = video_filter_name.c_str(),
			.func_proc = func_video_proc,
			.func_init = func_init,
			.func_exit = func_exit,
			.func_update = func_update,
			.func_WndProc = func_wnd_proc,
			.information = video_filter_information.c_str(),
			.func_save_start = func_save_start,
			.func_save_end = func_save_end,
			.func_is_saveframe = func_is_save_frame,
			.func_project_load = func_project_load,
			.func_project_save = func_project_save,
			.func_modify_title = func_modify_title,
		};

		static AviUtl::FilterPluginDLL audio_filter = {
			.flag =
				AviUtl::FilterPlugin::Flag::AudioFilter |
				AviUtl::FilterPlugin::Flag::NoConfig |
				AviUtl::FilterPlugin::Flag::AlwaysActive |
				AviUtl::FilterPlugin::Flag::ExInformation,
			.name = audio_filter_name.c_str(),
			.func_proc = func_audio_proc,
			.information = audio_filter_information.c_str(),
		};

		static AviUtl::FilterPluginDLL* list[] = {
			&video_filter, &audio_filter, nullptr,
		};

		return list;
	}

	//
	// コアの初期化を実行します。
	//
	BOOL WINAPI core_init(HINSTANCE instance)
	{
		MY_TRACE_FUNC("{:#010x}", instance);

		return plugin.init(instance);
	}

	//
	// コアの後始末を実行します。
	//
	BOOL WINAPI core_exit()
	{
		MY_TRACE_FUNC("");

		return plugin.exit();
	}

	//
	// このプロセスで唯一のMagiオブジェクトを返します。
	//
	Magi* WINAPI core_get_magi()
	{
		static Magi magi;

		return &magi;
	}

	//
	// このプロセスで唯一のAddinManagerオブジェクトを返します。
	//
	AddinManager* WINAPI core_get_addin_manager()
	{
		static AddinManager addin_manager;

		return &addin_manager;
	}
}
