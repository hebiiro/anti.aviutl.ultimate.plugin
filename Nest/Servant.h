#pragma once
#include "Hive.h"
#include "Config.h"
#include "MainWindow.h"
#include "ConfigLoader.h"
#include "ConfigSaver.h"
#include "Hook/Api.h"
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
			return Hive::Name;
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return Hive::DisplayName;
		}

		//
		// この仮想関数は、DLLの初期化のタイミングで呼ばれます。
		//
		BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			hive.instance = instance;

			// コンフィグ(ini)を読み込みます。
			config.init();

			// コンテナのウィンドウクラスを登録します。
			Container::registerWndClass();

			// メインウィンドウを作成します。
			if (mainWindow->create(std::make_shared<ConfigLoader>(), std::make_shared<ConfigSaver>()))
			{
				hive.mainWindow = *mainWindow;
				MY_TRACE_HEX(hive.mainWindow);
			}

			hook::api.init();
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
			hook::api.exit();

			config.exit();

			return TRUE;
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			MY_TRACE_FUNC("");

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

			// サブプロセス用のドッキングウィンドウを作成します。
			mainWindow->createSubProcesses(hive.mainWindow);

			// モジュール別フックを初期化します。
			hook::module.init();

			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			// シャトルマネージャのデストラクタではタイミングが遅すぎるので、
			// ここでシャトルをすべて削除します。
			shuttleManager.shuttles.clear();

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
	} servant;
}
