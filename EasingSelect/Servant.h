#pragma once
#include "HostWindow.h"

namespace fgo::easing_select
{
	//
	// このクラスは画像からイージング番号を選択できるようにします。
	//
	inline struct EasingSelect : Servant
	{
		std::shared_ptr<HostWindow> hostWindow;

		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"EasingSelect";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"イージング選択";
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			if (::GetModuleHandleW(L"SelectEasing.auf"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"EasingSelectとSelectEasing.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			// ホストウィンドウの作成に失敗する可能性もあるので、
			// 最初はメインウィンドウをAviUtlウィンドウにしておきます。
			hive.mainWindow = magi.auin.GetAviUtlWindow();

			try
			{
				// ホストウィンドウを作成します。
				hostWindow = std::make_shared<HostWindow>();

				// ホストウィンドウをメインウィンドウに設定します。
				hive.mainWindow = *hostWindow;
			}
			catch (LPCTSTR e)
			{
				::MessageBox(hive.mainWindow, e, hive.AppName, MB_OK);
			}

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (hostWindow)
			{
				// ホストウィンドウを削除します。
				hostWindow = 0;
			}

			return FALSE;
		}
	} servant;
}
