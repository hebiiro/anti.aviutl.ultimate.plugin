#pragma once

namespace my
{
	//
	// メモリアドレスの型です。
	//
	using addr_t = uintptr_t;

	using tstring = std::basic_string<TCHAR>;
	using tstring_view = std::basic_string_view<TCHAR>;

	template <typename T>
	struct case_insensitive_char_traits
	{
		using base_traits = std::char_traits<T>;

		using char_type = T;
		using int_type = int;
		using off_type = std::streamoff;
		using pos_type = std::streampos;
		using state_type = std::mbstate_t;
		using comparison_category = std::weak_ordering;

		static constexpr void assign(char_type& c1, const char_type& c2) noexcept
		{
			base_traits::assign(c1, c2);
		}

		static constexpr bool eq(char_type c1, char_type c2) noexcept
		{
			return base_traits::eq(std::tolower(c1), std::tolower(c2));
		}

		static constexpr bool lt(char_type c1, char_type c2) noexcept
		{
			return base_traits::lt(std::tolower(c1), std::tolower(c2));
		}

		static constexpr int compare(const char_type* s1, const char_type* s2, size_t n)
		{
			auto order = std::lexicographical_compare_three_way(s1, s1 + n, s2, s2 + n,
				[](char_type c1, char_type c2) -> std::weak_ordering { return std::tolower(c1) <=> std::tolower(c2); });
			return order == std::weak_ordering::equivalent ? 0 : order == std::weak_ordering::greater ? 1 : -1;
		}

		static constexpr size_t length(const char_type* s)
		{
			return base_traits::length(s);
		}

		static constexpr const char_type* find(const char_type* s, size_t n, const char_type& a)
		{
			// https://x.com/sigma_axis/status/1784444143969947864
			// >> 見つからなかった場合の戻り値が std::find_if() の場合は 第2引数，std::char_traits<>::find() の場合はnullptrです．
			auto ret = std::find_if(s, s + n, [a](char_type c) { return std::tolower(c) == a; });
			return ret == s + n ? nullptr : ret;

//			return std::find_if(s, s + n, [a](char_type c) { return std::tolower(c) == a; });
		}

		static constexpr char_type* move(char_type* s1, const char_type* s2, size_t n)
		{
			return base_traits::move(s1, s2, n);
		}

		static constexpr char_type* copy(char_type* s1, const char_type* s2, size_t n)
		{
			return base_traits::copy(s1, s2, n);
		}

		static constexpr char_type* assign(char_type* s, size_t n, char_type a)
		{
			return base_traits::assign(s, n, a);
		}
	};

	using case_insensitive_string = std::basic_string<char, case_insensitive_char_traits<char>>;
	using case_insensitive_wstring = std::basic_string<wchar_t, case_insensitive_char_traits<wchar_t>>;
	using case_insensitive_tstring = std::basic_string<TCHAR, case_insensitive_char_traits<TCHAR>>;

	template <typename T, typename F>
	inline T transform(const T& _s, F func)
	{
		T s(_s);
		std::transform(s.begin(), s.end(), s.begin(), func);
		return s;
	}

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

	template <typename T>
	struct FormatArg {
		T value;
		FormatArg(T value) : value(value) {}
		T& operator()() { return value; }
	};

	template <typename T>
	struct FormatArg<T*> {
		unsigned value;
		FormatArg(T* value) : value((unsigned)value) {}
		unsigned& operator()() { return value; }
	};

	template <typename T>
	struct FormatArg<const T*> {
		unsigned value;
		FormatArg(const T* value) : value((unsigned)value) {}
		unsigned& operator()() { return value; }
	};

	template <>
	struct FormatArg<char*> {
		tstring s;
		FormatArg(char* s) : s(s ? ts(s) : _T("null")) {}
		tstring& operator()() { return s; }
	};

	template <>
	struct FormatArg<const char*> {
		tstring s;
		FormatArg(const char* s) : s(s ? ts(s) : _T("null")) {}
		tstring& operator()() { return s; }
	};

	template <>
	struct FormatArg<wchar_t*> {
		tstring s;
		FormatArg(wchar_t* s) : s(s ? ts(s) : _T("null")) {}
		tstring& operator()() { return s; }
	};

	template <>
	struct FormatArg<const wchar_t*> {
		tstring s;
		FormatArg(const wchar_t* s) : s(s ? ts(s) : _T("null")) {}
		tstring& operator()() { return s; }
	};

	template <>
	struct FormatArg<std::string> {
		tstring s;
		FormatArg(const std::string& s) : s(ts(s)) {}
		tstring& operator()() { return s; }
	};

	template <>
	struct FormatArg<std::wstring> {
		tstring s;
		FormatArg(const std::wstring& s) : s(ts(s)) {}
		tstring& operator()() { return s; }
	};

	template <>
	struct FormatArg<std::filesystem::path> {
		tstring s;
		FormatArg(const std::filesystem::path& path) : s(ts(path)) {}
		tstring& operator()() { return s; }
	};

    //
	// std::format()にはハンドルや異なる文字コードセットの文字列を渡せない問題があるので、自分でカスタマイズしています。
	//
	template <typename... Args>
	inline tstring format(tstring_view fmt, const Args&... args)
	{
        return vformat(fmt, std::make_wformat_args(FormatArg(args)()...));
	}

	//
	// str内のfromをtoに置換した文字列を返します。
	//
	inline std::wstring replace(const std::wstring& str, const std::wstring& from, const std::wstring& to)
	{
		std::wstring result;

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
	// s内にxが含まれているかを判定します。
	//
	inline auto contains(const tstring& s, const tstring& x)
	{
		return s.find(x) != s.npos;
	}

	//
	// s内にxが含まれているかを判定します。
	//
	inline auto contains_i(const case_insensitive_tstring& s, const case_insensitive_tstring& x)
	{
		return s.find(x) != s.npos;
	}

	//
	// テンポラリフォルダ名を返します。
	//
	inline auto get_temp_path()
	{
		const auto buffer_length = ::GetTempPath(0, 0);
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
		const auto buffer_length = ::GetCurrentDirectory(0, 0);
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
		::GetModuleFileName(instance, buffer.get(), buffer_length);
		return buffer.get();
	}

	//
	// クラス名を返します。
	//
	template <typename T = case_insensitive_tstring>
	inline auto get_class_name(HWND hwnd, size_t buffer_length = MAX_PATH)
	{
		T buffer;
		buffer.resize(buffer_length);
		const auto length = ::GetClassName(hwnd, buffer.data(), buffer.length());
		buffer.resize(length);
		return buffer;
	}

	//
	// ウィンドウテキストを返します。
	//
	inline auto get_window_text(HWND hwnd)
	{
		const auto buffer_length = ::GetWindowTextLength(hwnd) + 1;
		tstring buffer(buffer_length, _T('\0'));
		::GetWindowText(hwnd, buffer.data(), buffer.length());
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
		size_t size = ::GetDlgItemText(hwnd, id, buffer.data(), buffer.length());
		buffer.resize(size);
		return buffer;
	}

	//
	// メニューアイテムのテキストを返します。
	//
	inline auto get_menu_item_text(HMENU menu, UINT item_id, UINT flags)
	{
		const auto buffer_length = ::GetMenuString(menu, item_id, 0, 0, flags) + 1;
		tstring buffer(buffer_length, _T('\0'));
		::GetMenuString(menu, item_id, buffer.data(), buffer.length(), flags);
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
		if (text_length <= 0) return tstring();

		// 指定されたアイテムのテキストを取得します。
		tstring text(text_length, _T('\0'));
		::SendMessage(combobox, CB_GETLBTEXT, index, (LPARAM)text.data());
		return text;
	}
}
