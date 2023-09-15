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
		static std::wstring getAddinDirectory(HINSTANCE instance)
		{
			WCHAR path[MAX_PATH] = {};
			::GetModuleFileNameW(instance, path, std::size(path));
			::PathRemoveExtensionW(path);
			::StringCbCatW(path, sizeof(path), L"Addin\\");
			return path;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		static std::wstring getConfigFileName(HINSTANCE instance)
		{
			WCHAR path[MAX_PATH] = {};
			::GetModuleFileNameW(instance, path, std::size(path));
			::PathRemoveExtensionW(path);
			::StringCbCatW(path, sizeof(path), L"Config\\AddinOnOff.ini");
			return path;
		}

		//
		// コンフィグファイルを参照し、指定されたファイル名のアドインが有効ならTRUEを返します。
		//
		static BOOL isAddinEnabled(LPCWSTR configFileName, LPCWSTR fileName)
		{
			WCHAR addinName[MAX_PATH] = {};
			::StringCbCopyW(addinName, sizeof(addinName), fileName);
			::PathRemoveExtensionW(addinName);

			BOOL enable = TRUE;
			getPrivateProfileInt(configFileName, L"AddinOnOff", addinName, enable);
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
			return fate.add_servant(module, get_servant());
		}

		//
		// アドインディレクトリ内のauaファイルをアドインとして読み込みます。
		//
		BOOL loadAddins(HINSTANCE instance)
		{
			std::wstring directory = getAddinDirectory(instance);
			std::wstring searchPath = directory + L"*.aua";

			WIN32_FIND_DATAW fd;
			HANDLE handle = ::FindFirstFileW(searchPath.c_str(), &fd);

			if (handle == INVALID_HANDLE_VALUE)
				return FALSE;

			std::wstring configFileName = getConfigFileName(instance);

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
						std::wstring fileName = directory + fd.cFileName;

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
		BOOL init(HINSTANCE instance)
		{
			if (!loadAddins(instance)) return FALSE;
			if (!fate.fire_dll_init()) return FALSE;

			return TRUE;
		}

		//
		// 後始末を実行します。
		//
		BOOL exit()
		{
			if (!fate.fire_dll_exit()) return FALSE;

			return TRUE;
		}
	} master;
}
