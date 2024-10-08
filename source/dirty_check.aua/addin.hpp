#pragma once

namespace apn::dirty_check
{
	//
	// このクラスはプロジェクトファイルが変更されているときに終了を確認します。
	//
	inline struct DirtyCheck : Addin
	{
		//
		// この仮想関数は、このアドインの識別名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_name() override
		{
			return hive.c_name;
		}

		//
		// この仮想関数は、このアドインの表示名が必要なときに呼ばれます。
		//
		virtual LPCWSTR get_addin_display_name() override
		{
			return hive.c_display_name;
		}

		virtual BOOL on_init() override
		{
			return func_init(magi.fp);
		}

		virtual BOOL on_exit() override
		{
			return func_exit(magi.fp);
		}

		virtual BOOL on_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			return func_wndproc(hwnd, message, wParam, lParam, editp, fp);
		}

		virtual BOOL on_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size) override
		{
			return func_project_save(fp, editp, data, size);
		}

		virtual BOOL on_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t frame, LPSTR title, int32_t max_title) override
		{
			return func_modify_title(fp, editp, frame, title, max_title);
		}
	} addin;
}
