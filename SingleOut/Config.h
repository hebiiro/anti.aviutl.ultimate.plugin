#pragma once
#include "Hive.h"
#include "MainWindow.h"

namespace fgo::single_out
{
	//
	// このクラスはコンフィグを管理します。
	//
	inline struct Config
	{
		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(hive.Name) + L".ini";
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return save(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			MY_TRACE_FUNC("%ws", path);

			getPrivateProfileInt(path, L"Config", L"quality", hive.quality);
			getPrivateProfileLabel(path, L"Config", L"mode", hive.mode, Hive::Mode::label);
			getPrivateProfileBSTR(path, L"Config", L"fileName", hive.fileName);
			getPrivateProfileInt(path, L"Config", L"index", hive.index);
			getPrivateProfileInt(path, L"Config", L"zero", hive.zero);
			getPrivateProfileWindow(path, L"MainWindow", mainWindow);

			mainWindow.mainDialog.updateControls();

			return TRUE;
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			MY_TRACE_FUNC("%ws", path);

			setPrivateProfileInt(path, L"Config", L"quality", hive.quality);
			setPrivateProfileLabel(path, L"Config", L"mode", hive.mode, Hive::Mode::label);
			setPrivateProfileBSTR(path, L"Config", L"fileName", hive.fileName);
			setPrivateProfileInt(path, L"Config", L"index", hive.index);
			setPrivateProfileInt(path, L"Config", L"zero", hive.zero);
			setPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}
	} config;
}
