#pragma once
#include "Hive.h"
#include "MainWindow.h"
#include "ConfigLoader.h"
#include "ConfigSaver.h"
#include "Hook/Api.h"
#include "Hook/CallWndProcRet.h"
#include "Hook/GetMessage.h"
#include "Hook/Module.h"

namespace fgo::nest
{
	//
	// このクラスはサーヴァントの派生クラスです。
	//
	inline struct Nest : Servant
	{
		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"Nest";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"ネスト";
		}

		//
		// この仮想関数は、DLLの初期化のタイミングで呼ばれます。
		//
		BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			hive.instance = instance;

			// コンテナのウィンドウクラスを登録します。
			Container::registerWndClass();

			// メインウィンドウを作成します。
			if (mainWindow->create(std::make_shared<ConfigLoader>(), std::make_shared<ConfigSaver>()))
			{
				hive.mainWindow = *mainWindow;
				MY_TRACE_HEX(hive.mainWindow);
			}

			hook::api.init();
			hook::call_wnd_proc_ret.init();
			hook::get_message.init();

			return TRUE;
		}

		//
		// この仮想関数は、DLLの後始末のタイミングで呼ばれます。
		//
		BOOL on_dll_exit(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("");

			hook::get_message.exit();
			hook::call_wnd_proc_ret.exit();
			hook::api.exit();

			return TRUE;
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			if (::GetModuleHandleW(L"UniteWindow.aul"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"NestとUniteWindow.aulが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			if (::GetModuleHandleW(L"SplitWindow.aul"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"NestとSplitWindow.aulが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!init()) return FALSE;
			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			if (!save()) return FALSE;
			if (!exit()) return FALSE;
			return TRUE;
		}

		addr_t on_get_address(LPCWSTR name) override
		{
			if (wcscmp(name, L"shuttle") == 0) return (addr_t)&exports.shuttle;

			return 0;
		}

		struct Exports {
			struct Shuttle : Share::Nest::Exports::Shuttle {
				//
				// エクスポート関数です。
				// 指定されたターゲットを保持するシャトル名を変更します。
				//
				BOOL rename(HWND target, LPCWSTR name) override
				{
					MY_TRACE_FUNC("0x%08X, %ws", target, name);

					auto shuttle = fgo::nest::Shuttle::getPointer(target);
					if (!shuttle) return FALSE;
					return shuttleManager.rename(shuttle, name);
				}

				//
				// エクスポート関数です。
				// 指定されたシャトル名が使用可能な場合はTRUEを返します。
				//
				BOOL is_empty_name(LPCWSTR name) override
				{
					MY_TRACE_FUNC("%ws", name);

					return !shuttleManager.get(name);
				}
			} shuttle;
		} exports;

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"Nest.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			hive.psdtoolkit.load(path, L"PSDToolKit");
			hive.bouyomisan.load(path, L"Bouyomisan");
			hive.console.load(path, L"Console");

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
		// コンフィグファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			hive.psdtoolkit.save(path, L"PSDToolKit");
			hive.bouyomisan.save(path, L"Bouyomisan");
			hive.console.save(path, L"Console");

			return TRUE;
		}

		//
		// 初期化処理です。
		//
		BOOL init()
		{
			mainWindow->createSubProcesses(hive.mainWindow);

			// モジュール別フックを初期化します。
			hook::module.init();

			return TRUE;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			return TRUE;
		}
	} servant;
}
