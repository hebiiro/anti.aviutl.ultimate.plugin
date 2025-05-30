#pragma once

namespace sprintfmt
{
#if 0
	//
	// std::stringをstd::wstringに変換して返します。
	// https://nprogram.hatenablog.com/entry/2018/01/03/113206
	//
	inline std::wstring cp_to_wide(const std::string& s, UINT codepage)
	{
		auto in_length = (int)s.length();
		auto out_length = ::MultiByteToWideChar(codepage, DWORD(), s.c_str(), in_length, LPWSTR(), int()); 
		auto result = std::wstring(out_length, L'\0');
		if (out_length) ::MultiByteToWideChar(codepage, DWORD(), s.c_str(), in_length, result.data(), out_length);
		return result;
	}

	//
	// std::wstringをstd::stringに変換して返します。
	// https://nprogram.hatenablog.com/entry/2018/01/03/113206
	//
	inline std::string wide_to_cp(const std::wstring& s, UINT codepage)
	{
		auto in_length = (int)s.length();
		auto out_length = ::WideCharToMultiByte(codepage, DWORD(), s.c_str(), in_length, LPSTR(), int(), LPCCH(), LPBOOL()); 
		auto result = std::string(out_length, '\0');
		if (out_length) ::WideCharToMultiByte(codepage, DWORD(), s.c_str(), in_length, result.data(), out_length, LPCCH(), LPBOOL()); 
		return result;
	}

	//
	// マルチバイト文字列をワイド文字列に変換して返します。
	//
	inline std::wstring to_wide(const std::string& s)
	{
		return cp_to_wide(s, CP_ACP);
	}

	//
	// ワイド文字列をマルチバイト文字列に変換して返します。
	//
	inline std::string from_wide(const std::wstring& s)
	{
		return wide_to_cp(s, CP_ACP);
	}
#else
	//
	// マルチバイト文字列をワイド文字列に変換して返します。
	//
	inline std::wstring to_wide(const std::string& s)
	{
		auto s_begin = s.c_str();
		auto s_end = s.c_str() + s.length();
		auto mb_state = mbstate_t();
		auto result = std::wstring();
		result.reserve(s.length());

		for (auto p = s_begin; p < s_end;)
		{
			auto wc = wchar_t();
			auto char_length = (int)mbrtowc(&wc, p, MB_CUR_MAX, &mb_state);
			if (char_length < 1) break;
			result += wc;
			p += char_length;
		}

		return result;
	}

	//
	// ワイド文字列をマルチバイト文字列に変換して返します。
	//
	inline std::string from_wide(const std::wstring& s)
	{
		auto s_begin = s.c_str();
		auto s_end = s.c_str() + s.length();
		auto mb_state = mbstate_t();
		auto mc = std::make_unique<char[]>(MB_CUR_MAX);
		auto result = std::string();
		result.reserve(s.length() * 2);

		for (auto p = s_begin; p < s_end; p++)
		{
			auto char_length = size_t();
			wcrtomb_s(&char_length, mc.get(), MB_CUR_MAX, *p, &mb_state);
			if ((int)char_length < 1) break;
			result += { mc.get(), char_length };
		}

		return result;
	}
#endif
	//
	// この構造体は文字列型毎のユーティリティです。
	//
	template <typename S> struct Utils;

	//
	// この構造体はstd::string型用に特殊化されたユーティリティです。
	//
	template <> struct Utils<std::string> {
		inline static const std::string::value_type eos = '\0';
		inline static const std::string null_pointer = "(null)";
		inline static const std::string prefix = "%";
		inline static const std::string suffix_hs = "hs";
		inline static const std::string suffix_ls = "ls";
		inline static const std::string suffix_d32 = "d";
		inline static const std::string suffix_d64 = "lld";
		inline static const std::string suffix_u32 = "u";
		inline static const std::string suffix_u64 = "llu";
		inline static const std::string suffix_f = "f";
		inline static const std::string fmt_p32 = "0x%08X";
		inline static const std::string fmt_p64 = "0x%016llX";
		inline static constexpr const std::string& from_hs(const std::string& value) { return value; }
		inline static std::string from_ls(const std::wstring& value) { return from_wide(value); }
		inline static std::string::size_type strlen(const std::string::value_type* s) { return ::strlen(s); }
		inline static constexpr int sprintf(
			std::string::value_type* buffer, std::string::size_type buffer_size,
			const std::string::value_type* fmt, auto&&... args)
		{
			return ::_snprintf_s(buffer, buffer_size, _TRUNCATE, fmt, args...);
		}
	};

	//
	// この構造体はstd::wstring型用に特殊化されたユーティリティです。
	//
	template <> struct Utils<std::wstring> {
		inline static const std::wstring::value_type eos = L'\0';
		inline static const std::wstring null_pointer = L"(null)";
		inline static const std::wstring prefix = L"%";
		inline static const std::wstring suffix_hs = L"hs";
		inline static const std::wstring suffix_ls = L"ls";
		inline static const std::wstring suffix_d32 = L"d";
		inline static const std::wstring suffix_d64 = L"lld";
		inline static const std::wstring suffix_u32 = L"u";
		inline static const std::wstring suffix_u64 = L"llu";
		inline static const std::wstring suffix_f = L"f";
		inline static const std::wstring fmt_p32 = L"0x%08X";
		inline static const std::wstring fmt_p64 = L"0x%016llX";
		inline static std::wstring from_hs(const std::string& value) { return to_wide(value); }
		inline static constexpr const std::wstring& from_ls(const std::wstring& value) { return value; }
		inline static std::wstring::size_type strlen(const std::wstring::value_type* s) { return ::wcslen(s); }
		inline static constexpr int sprintf(
			std::wstring::value_type* buffer, std::wstring::size_type buffer_size,
			const std::wstring::value_type* fmt, auto&&... args)
		{
			return ::_snwprintf_s(buffer, buffer_size, _TRUNCATE, fmt, args...);
		}
	};

	//
	// C言語のsprintf()を使用して文字列をフォーマット化して返します。
	//
	template <size_t c_max_size = 64, typename S, typename... Args>
	_NODISCARD constexpr auto sprintf_S(const S& fmt, Args&&... args) -> S
	{
		using utils = Utils<S>;

		auto s = S(c_max_size, utils::eos);
		utils::sprintf(s.data(), s.size(), fmt.c_str(), args...);
		s.resize(utils::strlen(s.data()));
		return s;
	}

	//
	// std::string型用のsprintf()です。
	//
	template <size_t c_max_size = 64, typename... Args>
	_NODISCARD constexpr auto sprintf(const std::string& fmt, Args&&... args) -> std::string
	{
		return sprintf_S<c_max_size>(fmt, args...);
	}

	//
	// std::wstring型用のsprintf()です。
	//
	template <size_t c_max_size = 64, typename... Args>
	_NODISCARD constexpr auto sprintf(const std::wstring& fmt, Args&&... args) -> std::wstring
	{
		return sprintf_S<c_max_size>(fmt, args...);
	}

	//
	// T型の値をS型の文字列に変換して返します。
	//
	template <typename S, typename T>
	_NODISCARD constexpr auto fmt_value_to_string_S(const S& fmt, const T& value, auto... args) -> S
	{
		using utils = Utils<S>;

		// サブフォーマットが指定されている場合は
		// 指定されている書式を使用します。
		if (fmt.length())
		{
			if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::wstring>)
			{
				return sprintf<2048>(utils::prefix + fmt, args..., value.c_str());
			}
			else if constexpr(std::is_convertible_v<T, const char*> || std::is_convertible_v<T, const wchar_t*>)
			{
				return sprintf<2048>(utils::prefix + fmt, args..., value);
			}
			else
			{
				return sprintf(utils::prefix + fmt, args..., value);
			}
		}
		// サブフォーマットが指定されていない場合は
		// 型に合わせて書式を設定します。
		else
		{
			if constexpr (std::is_convertible_v<T, const char*>)
			{
				if constexpr (std::is_pointer_v<T>)
					return value ? utils::from_hs(value) : utils::null_pointer;
				else
					return utils::from_hs(value);
			}
			else if constexpr (std::is_convertible_v<T, const wchar_t*>)
			{
				if constexpr (std::is_pointer_v<T>)
					return value ? utils::from_ls(value) : utils::null_pointer;
				else
					return utils::from_ls(value);
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				return utils::from_hs(value);
			}
			else if constexpr (std::is_same_v<T, std::wstring>)
			{
				return utils::from_ls(value);
			}
			else if constexpr(std::is_integral_v<T>)
			{
				if constexpr(std::is_signed_v<T>)
				{
					if constexpr(sizeof(T) <= 4)
						return sprintf(utils::prefix + fmt + utils::suffix_d32, args..., value);
					else
						return sprintf(utils::prefix + fmt + utils::suffix_d64, args..., value);
				}
				else
				{
					if constexpr(sizeof(T) <= 4)
						return sprintf(utils::prefix + fmt + utils::suffix_u32, args..., value);
					else
						return sprintf(utils::prefix + fmt + utils::suffix_u64, args..., value);
				}
			}
			else if constexpr(std::is_floating_point_v<T>)
			{
				return sprintf(utils::prefix + fmt + utils::suffix_f, args..., value);
			}
			else if constexpr(std::is_pointer_v<T> || std::is_enum_v<T>)
			{
				if constexpr(sizeof(T) <= 4)
					return sprintf(utils::fmt_p32, args..., value);
				else
					return sprintf(utils::fmt_p64, args..., value);
			}
			else
			{
				static_assert(std::false_type::value, "この型に対応するfmt_value_to_string()が見つかりませんでした");
			}
		}
	}

	//
	// 指定された値をstd::stringに変換して返します。
	//
	template <typename T>
	_NODISCARD constexpr auto fmt_value_to_string(const std::string& fmt, const T& value, auto... args) -> std::string
	{
		return fmt_value_to_string_S<std::string, T>(fmt, value, args...);
	}

	//
	// 指定された値をstd::stringに変換して返します。
	//
	template <typename T>
	_NODISCARD constexpr auto fmt_value_to_string(const std::wstring& fmt, const T& value, auto... args) -> std::wstring
	{
		return fmt_value_to_string_S<std::wstring, T>(fmt, value, args...);
	}
#ifdef _FILESYSTEM_
	//
	// std::filesystem::pathをstd::stringに変換します。
	//
	_NODISCARD constexpr auto fmt_value_to_string(const std::string& fmt, const std::filesystem::path& value, auto... args) -> std::string
	{
		return fmt_value_to_string(fmt, value.string(), args...);
	}

	//
	// std::filesystem::pathをstd::wstringに変換します。
	//
	_NODISCARD constexpr auto fmt_value_to_string(const std::wstring& fmt, const std::filesystem::path& value, auto... args) -> std::wstring
	{
		return fmt_value_to_string(fmt, value.wstring(), args...);
	}
#endif
	//
	// この構造体はヘルパーです。
	//
	template <typename S> struct Helper;

	//
	// この構造体はstd::string型に特殊化されたヘルパーです。
	//
	template <> struct Helper<std::string>
	{
		//
		// フォーマット用のキーワードです。
		//
		inline static constexpr struct {
			inline static const std::string begin = "{/";
			inline static const std::string end = "}";
			inline static const std::string escape = "/";
			inline static const std::string separator = "$";
			inline static const std::string invalid_arg_index = "invalid_arg_index";
		} keyword;

		//
		// 指定された文字列の出現位置を返します。
		//
		inline static const std::string::value_type* find_str(
			const std::string::value_type* str_begin,
			const std::string::value_type* str_end,
			const std::string& pattern)
		{
			// 文字列が無効の場合は失敗します。
			if (str_begin >= str_end) return nullptr;

			// パターンが無効の場合は失敗します。
			if (pattern.empty()) return nullptr;

			// 文字列の長さを取得します。
			auto str_length = (size_t)(str_end - str_begin);

			// 文字列がパターンより短い場合はnullptrを返します。
			if (str_length < pattern.length()) return nullptr;

			// mbrlen()が使用する変数です。
			auto mb_state = mbstate_t {};

			// 文字列の長さからパターンの長さを除いた数だけループします。
			for (auto p = str_begin; p <= str_end - pattern.length();)
			{
				// 文字の長さを取得します。
				auto char_length = (int)mbrlen(p, MB_CUR_MAX, &mb_state);

				// 文字の長さが取得できなかった場合はループを終了します。
				if (char_length < 1) break;

				// 文字列内の現在位置がパターンと一致した場合は
				if (strncmp(p, pattern.c_str(), pattern.length()) == 0)
					return p; // 一致した位置を返します。

				// 文字の長さの分だけ現在位置を進めます。
				p += char_length;
			}

			// パターンが見つからなかったのでnullptrを返します。
			return nullptr;
		}

		//
		// 指定された文字列を整数に変換して返します。
		//
		inline static int to_int(const std::string& s)
		{
			try { return std::stoi(s); } catch (...) { return 0; }
		}
	};

	//
	// この構造体はstd::wstring型に特殊化されたヘルパーです。
	//
	template <> struct Helper<std::wstring>
	{
		//
		// フォーマット用のキーワードです。
		//
		inline static constexpr struct {
			inline static const std::wstring begin = L"{/";
			inline static const std::wstring end = L"}";
			inline static const std::wstring escape = L"/";
			inline static const std::wstring separator = L"$";
			inline static const std::wstring invalid_arg_index = L"invalid_arg_index";
		} keyword;

		//
		// 指定された文字列の出現位置を返します。
		//
		inline static const std::wstring::value_type* find_str(
			const std::wstring::value_type* str_begin,
			const std::wstring::value_type* str_end,
			const std::wstring& pattern)
		{
			// 文字列が無効の場合は失敗します。
			if (str_begin >= str_end) return nullptr;

			// パターンが無効の場合は失敗します。
			if (pattern.empty()) return nullptr;

			// 文字列の長さを取得します。
			auto str_length = (size_t)(str_end - str_begin);

			// 文字列がパターンより短い場合はnullptrを返します。
			if (str_length < pattern.length()) return nullptr;

			// 文字列の長さからパターンの長さを除いた数だけループします。
			for (auto p = str_begin; p <= str_end - pattern.length(); p++)
			{
				// 文字列内の現在位置がパターンと一致した場合は
				if (wcsncmp(p, pattern.c_str(), pattern.length()) == 0)
					return p; // 一致した位置を返します。
			}

			// パターンが見つからなかったのでnullptrを返します。
			return nullptr;
		}

		//
		// 指定された文字列を整数に変換して返します。
		//
		inline static int to_int(const std::wstring& s)
		{
			try { return std::stoi(s); } catch (...) { return 0; }
		}
	};

	//
	// この構造体はサブフォーマッタ生成関数オブジェクトです。
	//
	template <typename S>
	struct MakeSubFormatter {
		template <typename T, typename... Args>
		constexpr auto operator()(const T& value, Args&&... args) const {
			return [&](const S& fmt) {
				return fmt_value_to_string(fmt, value, args...);
			};
		}
	};

	//
	// この構造体はフォーマッタです。
	//
	template <typename S, typename M>
	struct Formatter
	{
		//
		// フォーマット用のヘルパーです。
		//
		inline static const Helper<S> utils;

		//
		// 新規作成したサブフォーマッタを使用して文字列化を実行します。
		//
		template <typename T>
		inline static constexpr auto use_sub_formatter(const T& value, const S& fmt)
		{
			return use_sub_formatter(M()(value), fmt);
		}

		//
		// 既存のサブフォーマッタを使用して文字列化を実行します。
		//
		template <typename T>
		inline static constexpr auto use_sub_formatter(T&& sub_formatter, const S& fmt)
		requires requires { std::forward<T>(sub_formatter)(fmt); } // 引数がサブフォーマッタ(ラムダ)の場合は
		{
			return sub_formatter(fmt);
		}

		//
		// インデックスで指定された引数が存在しなかった場合に呼び出されます。
		//
		inline static constexpr auto sub_format(size_t index, const S& sub_fmt) -> S
		{
			return utils.keyword.invalid_arg_index;
		}

		//
		// インデックスで指定された引数をサブフォーマット化した文字列を返します。
		//
		template <typename Head, typename... Tail>
		inline static constexpr auto sub_format(size_t index, const S& sub_fmt, Head&& head, Tail&&... tail) -> S
		{
			// インデックスが無効の場合は
			if (index < 1)
			{
				// 引数が存在しなかった場合の処理を実行します。
				return sub_format(index, sub_fmt);
			}
			// インデックスが一致する場合は
			else if (index == 1)
			{
				// 引数をサブフォーマッタを使用して文字列化して返します。
				return use_sub_formatter(std::forward<Head>(head), sub_fmt);
			}
			// インデックスが一致しない場合は
			else
			{
				// インデックスをデクリメントし、次の引数を処理します。
				return sub_format(--index, sub_fmt, std::forward<Tail>(tail)...);
			}
		}

		//
		// フォーマット化された文字列を返します。
		//
		template <typename... Args>
		inline static constexpr auto format(const S& fmt, Args&&... args) -> S
		{
			//
			// この構造体はパターンを検索してその位置を保持します。
			//
			struct Finder {
				const S::value_type* head;
				const S::value_type* tail;
				Finder(const S::value_type* begin, const S::value_type* end, const S& pattern)
					: head(utils.find_str(begin, end, pattern))
					, tail(head ? head + pattern.length() : nullptr) {}
			};

			// フォーマットの開始位置です。
			auto fmt_begin = fmt.c_str();

			// フォーマットの終了位置です。
			auto fmt_end = fmt.c_str() + fmt.length();

			// 最終的に返す結果を格納する変数です。
			auto result = S {};

			// 現在の使用対象の引数のインデックスです。
			auto current_arg_index = size_t { 1 };

			// 現在の検索対象位置です。
			auto current_pos = fmt_begin;

			while (*current_pos)
			{
				// 開始位置を取得します。
				auto begin = Finder { current_pos, fmt_end, utils.keyword.begin };

				// 開始位置が取得できなかった場合はループを終了します。
				if (!begin.head)
					break;

				// 開始位置末尾が不正の場合はループを終了します。
				if (begin.tail >= fmt_end)
					break;

				// 現在の検索位置から開始位置までを結果に追加します。
				result += { current_pos, begin.head };

				// "{/...}"
				// エスケープされていない場合は
				if (begin.tail[0] != utils.keyword.escape[0])
				{
					// 終了位置を取得します。
					// 開始位置の末尾から検索します。
					auto end = Finder { begin.tail, fmt_end, utils.keyword.end };

					// 終了位置が取得できなかった場合はループを終了します。
					if (!end.head)
						break;

					// 文字列化に使用する引数のインデックスです。
					auto arg_index = current_arg_index;

					// 文字列化に使用するサブフォーマットです。
					auto sub_fmt = S {};

					// セパレータの位置を取得します。
					auto separator = Finder { begin.tail, end.head, utils.keyword.separator };

					// "{/arg_index$sub_fmt}"
					// セパレータが存在する場合は
					if (separator.head)
					{
						// 前半文字列から引数インデックスを取得します。
						arg_index = utils.to_int({ begin.tail, separator.head });

						// 後半文字列からサブフォーマットを取得します。
						sub_fmt = { separator.tail, end.head };
					}
					// "{/sub_fmt}"
					// セパレータが存在しない場合は
					else
					{
						// サブフォーマットを取得します。
						sub_fmt = { begin.tail, end.head };
					}

					// インデックスの位置にある引数を文字列化して結果に追加します。
					result += sub_format(arg_index, sub_fmt, args...);

					// 次の引数を使用対象にします。
					current_arg_index++;

					// 現在の検索対象位置を終了位置末尾に変更します。
					current_pos = end.tail;
				}
				// "{//..."
				// エスケープされている場合は
				else
				{
					// "{/"を結果に追加します。
					result += utils.keyword.begin;

					// 現在の検索対象位置を開始位置末尾に変更します。
					current_pos = begin.tail + 1;
				}
			}

			// 現在の検索位置から最後までを結果に追加します。
			result += { current_pos, fmt_end };

			return result;
		}
	};

	//
	// std::string型用のサブフォーマッタを作成して返します。
	//
	template <typename T, typename... Args>
	_NODISCARD constexpr auto sfs(const T& value, Args&&... args)
	{
		return MakeSubFormatter<std::string>()(value, args...);
	}

	//
	// std::wstring型用のサブフォーマッタを作成して返します。
	//
	template <typename T, typename... Args>
	_NODISCARD constexpr auto sfw(const T& value, Args&&... args)
	{
		return MakeSubFormatter<std::wstring>()(value, args...);
	}

	//
	// デフォルトのフォーマッタを使用して
	// フォーマット化された文字列を返します。
	//
	template <typename... Args>
	_NODISCARD constexpr auto format(const std::string& fmt, Args&&... args) -> std::string
	{
		return sprintfmt::Formatter<std::string, MakeSubFormatter<std::string>>::format(fmt, args...);
	}

	//
	// デフォルトのフォーマッタを使用して
	// フォーマット化されたワイド文字列を返します。
	//
	template <typename... Args>
	_NODISCARD constexpr auto format(const std::wstring& fmt, Args&&... args) -> std::wstring
	{
		return sprintfmt::Formatter<std::wstring, MakeSubFormatter<std::wstring>>::format(fmt, args...);
	}
}
