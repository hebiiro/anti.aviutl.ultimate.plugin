#pragma once

namespace apn::text_collect
{
	//
	// ファイル選択ダイアログを表示して読み込み用のパスを取得します。
	//
	inline std::wstring get_open_file_name(
		HWND owner,
		LPCWSTR title,
		LPCWSTR filter,
		LPCWSTR default_file_name,
		DWORD flags = OFN_FILEMUSTEXIST)
	{
		std::wstring file_name = default_file_name;

		// バッファを確保します。
		file_name.resize(MAX_PATH, L'\0');

		// ファイル選択ダイアログ用の構造体を設定します。
		OPENFILENAMEW ofn = { sizeof(ofn) };
		ofn.hwndOwner = owner;
		ofn.Flags = flags;
		ofn.lpstrTitle = title;
		ofn.lpstrFile = file_name.data();
		ofn.nMaxFile = (DWORD)file_name.size();
		ofn.lpstrFilter = filter;

		// ファイル選択ダイアログを表示します。
		if (!::GetOpenFileNameW(&ofn)) return {};

		// 受け取った文字列の長さをセットします。
		file_name.resize(wcslen(file_name.c_str()));

		return file_name;
	}

	//
	// ファイル選択ダイアログを表示して書き込み用のパスを取得します。
	//
	inline std::wstring get_save_file_name(
		HWND owner,
		LPCWSTR title,
		LPCWSTR filter,
		LPCWSTR default_file_name,
		LPCWSTR default_extension,
		DWORD flags = OFN_OVERWRITEPROMPT)
	{
		std::wstring file_name = default_file_name;

		// バッファを確保します。
		file_name.resize(MAX_PATH, L'\0');

		// ファイル選択ダイアログ用の構造体を設定します。
		OPENFILENAMEW ofn = { sizeof(ofn) };
		ofn.hwndOwner = owner;
		ofn.Flags = flags;
		ofn.lpstrTitle = title;
		ofn.lpstrFile = file_name.data();
		ofn.nMaxFile = (DWORD)file_name.size();
		ofn.lpstrFilter = filter;
		ofn.lpstrDefExt = default_extension;

		// ファイル選択ダイアログを表示します。
		if (!::GetSaveFileNameW(&ofn)) return {};

		// 受け取った文字列の長さをセットします。
		file_name.resize(wcslen(file_name.c_str()));

		return file_name;
	}

	//
	// 指定されたファイルを実行します。
	//
	inline BOOL shell_execute(LPCTSTR verb, LPCTSTR file, LPCTSTR parameters)
	{
		SHELLEXECUTEINFO sei = { sizeof(sei) };
		sei.lpVerb = verb;
		sei.nShow = SW_SHOWNORMAL;
		sei.lpFile = file;
		sei.lpParameters = parameters;
		return ::ShellExecuteEx(&sei);
	}
}
