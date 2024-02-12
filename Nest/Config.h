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
			hive.psdtoolkit.load(path, L"PSDToolKit");
			hive.bouyomisan.load(path, L"Bouyomisan");
			hive.console.load(path, L"Console");

			return TRUE;
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			setPrivateProfileBool(path, L"Config", L"useMaximumPlay", hive.useMaximumPlay);
			setPrivateProfileShortcutKey(path, L"ShortcutKey.showCaption", hive.shortcutKey.showCaption);
			hive.psdtoolkit.save(path, L"PSDToolKit");
			hive.bouyomisan.save(path, L"Bouyomisan");
			hive.console.save(path, L"Console");

			return TRUE;
		}
	} config;
}
