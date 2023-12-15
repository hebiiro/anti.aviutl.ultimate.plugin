#include "pch.h"
#include "Story.h"

namespace fgo
{
	//
	// 初期化
	//
	BOOL func_init(AviUtl::FilterPlugin* fp)
	{
		return story.func_init(fp);
	}

	//
	// 終了
	//
	BOOL func_exit(AviUtl::FilterPlugin* fp)
	{
		return story.func_exit(fp);
	}

	//
	// 映像フィルタ関数
	//
	BOOL func_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
	{
		return story.func_video_proc(fp, fpip);
	}

	//
	// 音声フィルタ関数
	//
	BOOL func_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
	{
		return story.func_audio_proc(fp, fpip);
	}

	//
	// ウィンドウ関数
	//
	BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
	{
		return story.func_WndProc(hwnd, message, wParam, lParam, editp, fp);
	}

	BOOL func_update(AviUtl::FilterPlugin* fp, AviUtl::FilterPlugin::UpdateStatus status)
	{
		return story.func_update(fp, status);
	}

	BOOL func_save_start(AviUtl::FilterPlugin* fp, int32_t s, int32_t e, AviUtl::EditHandle* editp)
	{
		return story.func_save_start(fp, s, e, editp);
	}

	BOOL func_save_end(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
	{
		return story.func_save_end(fp, editp);
	}

	BOOL func_is_saveframe(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t saveno, int32_t frame, int32_t fps, int32_t edit_flag, int32_t inter)
	{
		return story.func_is_saveframe(fp, editp, saveno, frame, fps, edit_flag, inter);
	}

	BOOL func_project_load(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t size)
	{
		return story.func_project_load(fp, editp, data, size);
	}

	BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size)
	{
		return story.func_project_save(fp, editp, data, size);
	}

	BOOL func_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t frame, LPSTR title, int32_t max_title)
	{
		return story.func_modify_title(fp, editp, frame, title, max_title);
	}

	//
	// エントリポイント
	//
	EXTERN_C BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
	{
		return story.DllMain(instance, reason, reserved);
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
			.name = "アルティメットプラグイン",
			.func_proc = func_video_proc,
			.func_init = func_init,
			.func_exit = func_exit,
			.func_update = func_update,
			.func_WndProc = func_WndProc,
			.information = "アルティメットプラグイン r13",
			.func_save_start = func_save_start,
			.func_save_end = func_save_end,
			.func_is_saveframe = func_is_saveframe,
			.func_project_load = func_project_load,
			.func_project_save = func_project_save,
			.func_modify_title = func_modify_title,
		};

		static AviUtl::FilterPluginDLL audio_filter = {
			.flag =
				AviUtl::FilterPlugin::Flag::NoConfig |
				AviUtl::FilterPlugin::Flag::AlwaysActive |
				AviUtl::FilterPlugin::Flag::ExInformation,
			.name = "アルティメットプラグイン(音声)",
			.func_proc = func_audio_proc,
			.information = "アルティメットプラグイン(音声)",
		};

		static AviUtl::FilterPluginDLL* list[] = {
			&video_filter, &audio_filter, 0,
		};

		return list;
	}

	//
	// この世界に辿り着いたときに呼び出されます。
	//
	BOOL WINAPI hello_world(HINSTANCE instance)
	{
		MY_TRACE_FUNC("0x%08X", instance);

		return story.init(instance);
	}

	//
	// この世界から離れるときに呼び出されます。
	//
	BOOL WINAPI see_you()
	{
		MY_TRACE_FUNC("");

		return story.exit();
	}

	//
	// このプロセスで唯一のMagiオブジェクトを返します。
	//
	Magi* WINAPI get_magi()
	{
		static Magi magi;

		return &magi;
	}

	//
	// このプロセスで唯一のFateオブジェクトを返します。
	//
	Fate* WINAPI get_fate()
	{
		static Fate fate;

		return &fate;
	}
}
