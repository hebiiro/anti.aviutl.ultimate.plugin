#pragma once

namespace my
{
	//
	// この構造体は文字列型毎のユーティリティです。
	//
	template <typename S> struct FormatUtils;

	//
	// この構造体はstd::string型用に特殊化されたユーティリティです。
	//
	template <> struct FormatUtils<std::string> {
		using S = std::string;
		inline static const S hex = "hex";
		inline static const S hex8 = "hex8";
		inline static const S hex16 = "hex16";
		inline static const S hex32 = "hex32";
		inline static const S hex64 = "hex64";
		inline static const S fmt_hex8 = "0x%02X";
		inline static const S fmt_hex16 = "0x%04X";
		inline static const S fmt_hex32 = "0x%08X";
		inline static const S fmt_hex64 = "0x%016llX";
	};

	//
	// この構造体はstd::wstring型用に特殊化されたユーティリティです。
	//
	template <> struct FormatUtils<std::wstring> {
		using S = std::wstring;
		inline static const S hex = L"hex";
		inline static const S hex8 = L"hex8";
		inline static const S hex16 = L"hex16";
		inline static const S hex32 = L"hex32";
		inline static const S hex64 = L"hex64";
		inline static const S fmt_hex8 = L"0x%02X";
		inline static const S fmt_hex16 = L"0x%04X";
		inline static const S fmt_hex32 = L"0x%08X";
		inline static const S fmt_hex64 = L"0x%016llX";
	};
/*
	//
	// この構造体は16進数で書式化するために使用されます。
	//
	template <typename T>
	struct hex { const T& value; };

	//
	// hex<T>をstd::stringに変換します。
	//
	template <typename T>
	_NODISCARD constexpr auto fmt_value_to_string(const std::string& fmt, const hex<T>& value, auto... args) -> std::string
	{
		return sprintfmt::::fmt_value_to_string(fmt, (void*)value.value, args...);
	}

	//
	// hex<T>型をstd::wstringに変換します。
	//
	template <typename T>
	_NODISCARD constexpr auto fmt_value_to_string(const std::wstring& fmt, const hex<T>& value, auto... args) -> std::wstring
	{
		return sprintfmt::::fmt_value_to_string(fmt, (void*)value.value, args...);
	}
*/
	//
	// このクラスはサブフォーマッタ生成関数オブジェクトです。
	//
	template <typename S>
	struct MakeSubFormatter {
		template <typename T, typename... Args>
		constexpr auto operator()(const T& value, Args&&... args) const {
			return [&](const S& fmt)
			{
				using utils = FormatUtils<S>;
				using namespace sprintfmt;

				if (fmt == utils::hex) return sprintf(sizeof(value) <= 4 ? utils::fmt_hex32 : utils::fmt_hex64, value);
				else if (fmt == utils::hex8) return sprintf(utils::fmt_hex8, value);
				else if (fmt == utils::hex16) return sprintf(utils::fmt_hex16, value);

				return fmt_value_to_string(fmt, value, args...);
			};
		}
	};

	//
	// std::string型用のカスタムフォーマッタを作成して返します。
	// 少ない文字数でコーディングできるようにするための処理なので、必須の定義ではありません。
	//
	template <typename T, typename... Args>
	_NODISCARD constexpr auto sfs(const T& value, Args&&... args)
	{
		return MakeSubFormatter<std::string>()(value, args...);
	}

	//
	// std::wstring型用のカスタムフォーマッタを作成して返します。
	// 少ない文字数でコーディングできるようにするための処理なので、必須の定義ではありません。
	template <typename T, typename... Args>
	_NODISCARD constexpr auto sfw(const T& value, Args&&... args)
	{
		return MakeSubFormatter<std::wstring>()(value, args...);
	}

	//
	// カスタムフォーマッタを使用して
	// フォーマット化された文字列を返します。
	//
	template <typename... Args>
	_NODISCARD constexpr auto format(const std::string& fmt, Args&&... args) -> std::string
	{
		return sprintfmt::Formatter<std::string, MakeSubFormatter<std::string>>::format(fmt, args...);
	}

	//
	// カスタムフォーマッタを使用して
	// フォーマット化されたワイド文字列を返します。
	//
	template <typename... Args>
	_NODISCARD constexpr auto format(const std::wstring& fmt, Args&&... args) -> std::wstring
	{
		return sprintfmt::Formatter<std::wstring, MakeSubFormatter<std::wstring>>::format(fmt, args...);
	}
}
