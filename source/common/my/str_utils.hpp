#pragma once

namespace my
{
	using tstring = std::basic_string<TCHAR>;
	using tstring_view = std::basic_string_view<TCHAR>;

	//
	// std::stringをstd::wstringに変換して返します。
	// https://nprogram.hatenablog.com/entry/2018/01/03/113206
	//
	inline std::wstring cp_to_wide(const std::string &s, UINT codepage)
	{
		int in_length = (int)s.length();
		int out_length = MultiByteToWideChar(codepage, 0, s.c_str(), in_length, 0, 0); 
		std::wstring result(out_length, L'\0');
		if (out_length) MultiByteToWideChar(codepage, 0, s.c_str(), in_length, &result[0], out_length);
		return result;
	}

	//
	// std::wstringをstd::stringに変換して返します。
	// https://nprogram.hatenablog.com/entry/2018/01/03/113206
	//
	inline std::string wide_to_cp(const std::wstring &s, UINT codepage)
	{
		int in_length = (int)s.length();
		int out_length = WideCharToMultiByte(codepage, 0, s.c_str(), in_length, 0, 0, 0, 0); 
		std::string result(out_length, '\0');
		if (out_length) WideCharToMultiByte(codepage, 0, s.c_str(), in_length, &result[0], out_length, 0, 0); 
		return result;
	}

	//
	// printf()の%hs、%ws、%tsの代わりの処理です。
	//
	inline std::string hs(const std::string &s) { return s; }
	inline std::string hs(const std::wstring &s) { return wide_to_cp(s, CP_ACP); }
	inline std::wstring ws(const std::string &s) { return cp_to_wide(s, CP_ACP); }
	inline std::wstring ws(const std::wstring &s) { return s; }
#ifdef _UNICODE
	inline tstring ts(const std::string &s) { return ws(s); }
	inline tstring ts(const std::wstring &s) { return ws(s); }
#else
	inline tstring ts(const std::string &s) { return hs(s); }
	inline tstring ts(const std::wstring &s) { return hs(s); }
#endif

	//
	// str内のfromをtoに置換した文字列を返します。
	//
	inline tstring replace(const tstring& str, const tstring& from, const tstring& to)
	{
		tstring result;

		size_t current_pos = 0;
		while (true)
		{
			auto pos = str.find(from, current_pos);
			if (pos == str.npos) break;

			result += str.substr(current_pos, pos - current_pos);
			result += to;

			current_pos = pos + from.length();
		}
		result += str.substr(current_pos);

		return result;
	}

	//
	// 指定された文字列を小文字にして返します。
	//
	inline tstring to_lower(const tstring& s)
	{
		tstring result(s);
		::CharLowerBuff(result.data(), (DWORD)result.size());
		return result;
	}

	//
	// 指定された文字列を大文字にして返します。
	//
	inline tstring to_upper(const tstring& s)
	{
		tstring result(s);
		::CharUpperBuff(result.data(), (DWORD)result.size());
		return result;
	}

	//
	// 指定された文字列が同じならtrueを返します。
	//
	inline bool eq(const tstring& s1, const tstring& s2)
	{
		return s1 == s2;
	}

	//
	// s1内にs2が含まれている場合はtrueを返します。
	//
	inline auto contains(const tstring& s1, const tstring& s2)
	{
		return s1.find(s2) != s1.npos;
	}

	namespace case_insensitive
	{
		//
		// 指定された文字列が同じならtrueを返します。
		// 大文字小文字を無視して比較します。
		//
		inline bool eq(const tstring& s1, const tstring& s2)
		{
			return my::eq(to_upper(s1), to_upper(s2));
		}

		//
		// s1内にs2が含まれている場合はtrueを返します。
		// 大文字小文字を無視して比較します。
		//
		inline auto contains(const tstring& s1, const tstring& s2)
		{
			return my::contains(to_upper(s1), to_upper(s2));
		}
	}

	//
	// エラーメッセージを返します。
	//
	inline auto get_error_message(HRESULT hr)
	{
		LPTSTR text = nullptr;
		::FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			hr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&text,
			0,
			nullptr);
		if (!text) return tstring {};
		tstring result(text);
		::LocalFree(text);
		return result;
	}

	//
	// テンポラリフォルダ名を返します。
	//
	inline auto get_temp_path()
	{
		const auto buffer_length = ::GetTempPath(0, nullptr);
		tstring buffer(buffer_length, _T('\0'));
		auto length = ::GetTempPath(buffer_length, buffer.data());
		buffer.resize(length);
		return buffer;
	}

	//
	// カレントディレクトリ名を返します。
	//
	inline auto get_current_directory()
	{
		const auto buffer_length = ::GetCurrentDirectory(0, nullptr);
		tstring buffer(buffer_length, _T('\0'));
		auto length = ::GetCurrentDirectory(buffer_length, buffer.data());
		buffer.resize(length);
		return buffer;
	}

	//
	// モジュールファイルのパスを返します。
	//
	inline std::filesystem::path get_module_file_name(HINSTANCE instance, size_t buffer_length = MAX_PATH)
	{
		auto buffer = std::make_unique<TCHAR[]>(buffer_length);
		::GetModuleFileName(instance, buffer.get(), (DWORD)buffer_length);
		return buffer.get();
	}

	//
	// 指定されたプロセスのモジュールファイルのパスを返します。
	//
	inline std::filesystem::path get_module_file_name_ex(HANDLE process, HINSTANCE instance, size_t buffer_length = MAX_PATH)
	{
		auto buffer = std::make_unique<TCHAR[]>(buffer_length);
		::GetModuleFileNameEx(process, instance, buffer.get(), (DWORD)buffer_length);
		return buffer.get();
	}

	//
	// クラス名を返します。
	//
	inline auto get_class_name(HWND hwnd, size_t buffer_length = MAX_PATH)
	{
		tstring buffer(buffer_length, _T('\0'));
		const auto length = ::GetClassName(hwnd, buffer.data(), (int)buffer.length());
		buffer.resize(length);
		return buffer;
	}

	//
	// ウィンドウテキストを返します。
	//
	inline auto get_window_text(HWND hwnd)
	{
		const auto buffer_length = ::GetWindowTextLength(hwnd) + 1;
		if (buffer_length <= 0) return tstring {};
		tstring buffer(buffer_length, _T('\0'));
		::GetWindowText(hwnd, buffer.data(), (int)buffer.length());
		buffer.resize(buffer_length - 1);
		return buffer;
	}

	//
	// ダイアログコントロールのテキストを返します。
	//
	inline auto get_dlg_item_text(HWND hwnd, UINT id, size_t buffer_length = MAX_PATH)
	{
		tstring buffer;
		buffer.resize(buffer_length);
		size_t size = ::GetDlgItemText(hwnd, id, buffer.data(), (int)buffer.length());
		buffer.resize(size);
		return buffer;
	}

	//
	// メニューアイテムのテキストを返します。
	//
	inline auto get_menu_item_text(HMENU menu, UINT item_id, UINT flags)
	{
		const auto buffer_length = ::GetMenuString(menu, item_id, nullptr, 0, flags) + 1;
		tstring buffer(buffer_length, _T('\0'));
		::GetMenuString(menu, item_id, buffer.data(), (int)buffer.length(), flags);
		buffer.resize(buffer_length - 1);
		return buffer;
	}

	//
	// コンボボックス内の指定されたアイテムのテキストを返します。
	//
	inline tstring get_text_from_combobox(HWND combobox, int index = -1)
	{
		if (index == -1)
			index = (int)::SendMessage(combobox, CB_GETCURSEL, 0, 0);

		// 指定されたアイテムのテキストの長さを取得します。
		auto text_length = (int)::SendMessage(combobox, CB_GETLBTEXTLEN, index, 0);
		if (text_length <= 0) return {};

		// 指定されたアイテムのテキストを取得します。
		tstring text(text_length, _T('\0'));
		::SendMessage(combobox, CB_GETLBTEXT, index, (LPARAM)text.data());
		return text;
	}
}
