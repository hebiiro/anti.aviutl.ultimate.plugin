#pragma once
#include "Hook.h"
#include "MainWindow.h"

namespace fgo::encode_guard
{
	//
	// このクラスは間違った設定でのエンコードを防ぎます。
	//
	inline struct EncodeGuard : Servant, Magi::CommandID::Plugin
	{
		MainWindow mainWindow;

		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"EncodeGuard";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"エンコードガード";
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
			if (::GetModuleHandleW(L"ConfigChecker.auf"))
			{
				::MessageBoxW(hwnd,
					L"EncodeGuardとConfigChecker.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!hook.init()) return FALSE;
			if (!mainWindow.create()) return FALSE;

			hive.mainWindow = mainWindow;

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			save();
			hook.exit();
			mainWindow.destroy();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウのコマンドのタイミングで呼ばれます。
		//
		BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
			{
			case ID_ADDIN:
				{
					if (mainWindow)
						Tools::AviUtl::PluginWindowExtension::show(mainWindow);

					break;
				}
			}

			return FALSE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"EncodeGuard.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			getPrivateProfileInt(path, L"Config", L"checkRange", hive.checkRange);
			getPrivateProfileInt(path, L"Config", L"checkLastFrame", hive.checkLastFrame);
			getPrivateProfileInt(path, L"Config", L"checkFrameRate", hive.checkFrameRate);
			getPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			setPrivateProfileInt(path, L"Config", L"checkRange", hive.checkRange);
			setPrivateProfileInt(path, L"Config", L"checkLastFrame", hive.checkLastFrame);
			setPrivateProfileInt(path, L"Config", L"checkFrameRate", hive.checkFrameRate);
			setPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}
	} servant;
}
