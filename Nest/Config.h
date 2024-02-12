#pragma once
#include "Hive.h"

namespace fgo::nest
{
	//
	// このクラスはコンフィグ(ini)を管理します。
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
			getPrivateProfileBool(path, L"Config", L"useMaximumPlay", hive.useMaximumPlay);
			getPrivateProfileShortcutKey(path, L"ShortcutKey.showCaption", hive.shortcutKey.showCaption);
			getPrivateProfileSubProcess(path, L"PSDToolKit", hive.psdtoolkit);
			getPrivateProfileSubProcess(path, L"Bouyomisan", hive.bouyomisan);
			getPrivateProfileSubProcess(path, L"Console", hive.console);

			return TRUE;
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			setPrivateProfileBool(path, L"Config", L"useMaximumPlay", hive.useMaximumPlay);
			setPrivateProfileShortcutKey(path, L"ShortcutKey.showCaption", hive.shortcutKey.showCaption);
			setPrivateProfileSubProcess(path, L"PSDToolKit", hive.psdtoolkit);
			setPrivateProfileSubProcess(path, L"Bouyomisan", hive.bouyomisan);
			setPrivateProfileSubProcess(path, L"Console", hive.console);

			return TRUE;
		}

		//
		// 指定されたファイルからサブプロセスの設定を読み込みます。
		//
		BOOL getPrivateProfileSubProcess(LPCWSTR path, LPCWSTR appName, Hive::SubProcess& subProcess)
		{
			getPrivateProfileBool(path, appName, L"dock", subProcess.dock);
			getPrivateProfileString(path, appName, L"run", subProcess.run);

			if (_tcslen(subProcess.run))
			{
				TCHAR directory[MAX_PATH] = {};
				::StringCchCopy(directory, std::size(directory), subProcess.run);
				::PathRemoveFileSpec(directory);
				::ShellExecute(hive.mainWindow, 0, subProcess.run, 0, directory, SW_SHOW);
			}

			return TRUE;
		}

		//
		// 指定されたファイルにサブプロセスの設定を保存します。
		//
		BOOL setPrivateProfileSubProcess(LPCWSTR path, LPCWSTR appName, const Hive::SubProcess& subProcess)
		{
			setPrivateProfileBool(path, appName, L"dock", subProcess.dock);
			setPrivateProfileString(path, appName, L"run", subProcess.run);

			return TRUE;
		}

	} config;
}
