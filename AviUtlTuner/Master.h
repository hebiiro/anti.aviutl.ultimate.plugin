#pragma once

namespace fgo
{
	//
	// このクラスはマスターです。
	//
	inline struct Master
	{
		//
		// プラグインフォルダのフルパスを返します。
		//
		std::wstring getPluginDirectory() const
		{
			WCHAR path[MAX_PATH] = {};
			::GetModuleFileNameW(fate.fp->dll_hinst, path, std::size(path));
			::PathRemoveExtensionW(path);
			::StringCbCatW(path, sizeof(path), L"Plugin");
			return path;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		std::wstring getConfigFileName() const
		{
			return fate.getConfigFileName(L"AviUtlTuner.ini");
		}

		//
		// コンフィグファイルを参照し、指定されたファイル名のプラグインが有効なら TRUE を返します。
		//
		static BOOL isPluginEnabled(LPCWSTR configFileName, LPCWSTR fileName)
		{
			BOOL enable = TRUE;
			getPrivateProfileInt(configFileName, L"Enable", fileName, enable);
			return enable;
		}

		//
		// 指定されたファイル名のプラグインを読み込み、サーヴァントを取得します。
		//
		BOOL loadPlugin(LPCWSTR fileName)
		{
			HMODULE module = ::LoadLibraryExW(fileName, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
			auto get_servant = (Servant* (WINAPI*)())::GetProcAddress(module, "get_servant");
			if (!get_servant) return FALSE;
			return grand_order.add_servant((*get_servant)());
		}

		//
		// プラグインディレクトリ内の aut ファイルをプラグインとして読み込みます。
		//
		BOOL loadPlugins()
		{
			std::wstring directory = getPluginDirectory();
			std::wstring searchPath = directory + L"\\*.aut";

			WIN32_FIND_DATAW fd;
			HANDLE handle = ::FindFirstFileW(searchPath.c_str(), &fd);

			if (handle == INVALID_HANDLE_VALUE)
				return FALSE;

			std::wstring configFileName = getConfigFileName();

			do
			{
				if (fd.cFileName[0] == '.') continue;

				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					// ディレクトリの場合は何もしません。
				}
				else
				{
					// ファイルの場合はプラグインファイルとして読み込みます。

					if (isPluginEnabled(configFileName.c_str(), fd.cFileName))
					{
						std::wstring fileName = directory + L"\\" + fd.cFileName;

						loadPlugin(fileName.c_str());
					}
				}
			}
			while (::FindNextFileW(handle, &fd));

			::FindClose(handle);

			return TRUE;
		}

		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			if (!loadPlugins()) return FALSE;

			return TRUE;
		}

		//
		// 後始末を実行します。
		//
		BOOL exit()
		{
			return TRUE;
		}
	} master;
}
