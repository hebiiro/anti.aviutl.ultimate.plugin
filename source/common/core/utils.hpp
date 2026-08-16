#pragma once

//
// https://qiita.com/hamamu/items/4d081751b69aa3bb3557
//
template<class T> size_t HashCombine(const size_t seed,const T &v)
{
	return seed^(std::hash<T>()(v)+0x9e3779b9+(seed<<6)+(seed>>2));
}

//
// pair用
//
template<class T,class S> struct std::hash<std::pair<T,S>>
{
	size_t operator()(const std::pair<T,S> &keyval) const noexcept
	{
		return HashCombine(std::hash<T>()(keyval.first), keyval.second);
	}
};

namespace apn
{
	//
	// よく使うファイルパスを管理します。
	//
	struct base_path_t
	{
		//
		// 基準となるパスです。
		//
		std::filesystem::path path;

		//
		// 基準となるフォルダのパスです。
		//
		std::filesystem::path dir;

		//
		// アセットフォルダのパスです。
		//
		std::filesystem::path assets_dir;

		//
		// コンフィグフォルダのパスです。
		//
		std::filesystem::path config_dir;

		//
		// コンストラクタです。
		//
		base_path_t(HINSTANCE instance)
		{
			// 基準となるパスを取得します。
			path = my::get_module_file_name(instance);

			// 基準となるフォルダのパスを取得します。
			dir = path, dir.replace_extension();

			// アセットフォルダのパスを取得します。
			assets_dir = dir  / L"assets";

			// コンフィグフォルダのパスを取得します。
			config_dir = dir  / L"config";
		}
	};

	//
	// 文字列をトリムします。
	// https://www.paveway.info/entry/2019/09/29/cpp_trim
	//
	inline static std::wstring trim(const std::wstring& src)
	{
		const std::wstring trim_characters = L"\xfeff\r\n \t";

		// trimするとき削除する文字以外の最初の文字の位置を取得します。
		size_t start = src.find_first_not_of(trim_characters);
		if (start == std::wstring::npos) return L"";

		// trimするとき削除する文字以外の最後の文字の位置を取得します。
		size_t end = src.find_last_not_of(trim_characters);
		if (end == std::wstring::npos) return L"";

		// trimするとき削除する文字以外の文字列を切り出します。
		return src.substr(start, end - start + 1);
	}

	//
	// 文字列を分割して配列にして返します。
	//
	inline std::vector<std::wstring> split(const std::wstring& str, wchar_t delimiter)
	{
		std::vector<std::wstring> vec;
		std::wstringstream ss(str);
		std::wstring buffer;
		while (std::getline(ss, buffer, delimiter)) vec.emplace_back(buffer);
		return vec;
	}

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
}
