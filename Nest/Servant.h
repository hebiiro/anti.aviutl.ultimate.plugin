#pragma once
#include "Hive.h"
#include "MainWindow.h"

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
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
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
			getPrivateProfileInt(path, L"Config", L"dockPSDToolKit", hive.dockPSDToolKit);
			getPrivateProfileInt(path, L"Config", L"dockBouyomisan", hive.dockBouyomisan);

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
			setPrivateProfileInt(path, L"Config", L"dockPSDToolKit", hive.dockPSDToolKit);
			setPrivateProfileInt(path, L"Config", L"dockBouyomisan", hive.dockBouyomisan);

			return TRUE;
		}

		//
		// 初期化処理です。
		//
		BOOL init()
		{
			mainWindow->createSubProcesses(hive.mainWindow);

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
