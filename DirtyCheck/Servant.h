#pragma once
#include "auls_confirmclose.h"

namespace fgo::dirty_check
{
	//
	// このクラスはプロジェクトファイルが変更されているときに終了を確認します。
	//
	inline struct DirtyCheck : Servant
	{
		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"DirtyCheck";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"終了確認";
		}

		BOOL on_init() override
		{
			if (::GetModuleHandleW(L"auls_confirmclose.auf"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"DirtyCheckとauls_confirmclose.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			return func_init(magi.fp);
		}

		BOOL on_exit() override
		{
			return func_exit(magi.fp);
		}

		BOOL on_window_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			return func_wndproc(hwnd, message, wParam, lParam, editp, fp);
		}

		BOOL on_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* data, int32_t* size) override
		{
			return func_project_save(fp, editp, data, size);
		}

		BOOL on_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int32_t frame, LPSTR title, int32_t max_title) override
		{
			return func_modify_title(fp, editp, frame, title, max_title);
		}
	} servant;
}
