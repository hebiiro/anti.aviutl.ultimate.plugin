#pragma once

namespace fgo
{
	//
	// このクラスはマスターです。
	//
	inline struct Master
	{
		//
		// アドインフォルダのフルパスを返します。
		//
		std::wstring getAddinDirectory() const
		{
			WCHAR path[MAX_PATH] = {};
			::GetModuleFileNameW(sheba.fp->dll_hinst, path, std::size(path));
			::PathRemoveExtensionW(path);
			::StringCbCatW(path, sizeof(path), L"Addin");
			return path;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		std::wstring getConfigFileName() const
		{
			return sheba.getConfigFileName(L"Last.ini");
		}

		//
		// コンフィグファイルを参照し、指定されたファイル名のアドインが有効なら TRUE を返します。
		//
		static BOOL isAddinEnabled(LPCWSTR configFileName, LPCWSTR fileName)
		{
			BOOL enable = TRUE;
			getPrivateProfileInt(configFileName, L"Enable", fileName, enable);
			return enable;
		}

		//
		// 指定されたファイル名のアドインを読み込み、サーヴァントを取得します。
		//
		BOOL loadAddin(LPCWSTR fileName)
		{
			HMODULE module = ::LoadLibraryExW(fileName, 0, LOAD_WITH_ALTERED_SEARCH_PATH);
			if (!module) return FALSE;
			auto get_servant = (Servant* (WINAPI*)())::GetProcAddress(module, "get_servant");
			if (!get_servant) return FALSE;
			return fate.add_servant((*get_servant)());
		}

		//
		// アドインディレクトリ内の aua ファイルをアドインとして読み込みます。
		//
		BOOL loadAddins()
		{
			std::wstring directory = getAddinDirectory();
			std::wstring searchPath = directory + L"\\*.aua";

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
					// ファイルの場合はアドインファイルとして読み込みます。

					if (isAddinEnabled(configFileName.c_str(), fd.cFileName))
					{
						std::wstring fileName = directory + L"\\" + fd.cFileName;

						loadAddin(fileName.c_str());
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
			if (!loadAddins()) return FALSE;

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
