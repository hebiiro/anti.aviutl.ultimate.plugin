#pragma once

namespace aut
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		AviUtl::FilterPlugin* fp = 0;

		//
		// 指定されたコンフィグファイルをフルパスにして返します。
		//
		std::wstring getConfigFileName(LPCWSTR fileName) const
		{
			WCHAR path[MAX_PATH] = {};
			::GetModuleFileNameW(fp->dll_hinst, path, std::size(path));
			::PathRemoveExtensionW(path);
			::PathAppendW(path, fileName);
			return path;
		}

		//
		// ハイブの初期化を実行します。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			this->fp = fp;

			return TRUE;
		}

		//
		// ハイブの後始末を実行します。
		//
		BOOL exit()
		{
			return TRUE;
		}
	} hive;
}
