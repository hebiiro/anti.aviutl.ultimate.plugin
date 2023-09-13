// https://learn.microsoft.com/ja-jp/archive/msdn-magazine/2015/march/windows-with-c-using-printf-with-modern-c
#pragma once

namespace Tools {
	template <class... Args>
	HRESULT StringPrintfEx(char* buffer, size_t bufferCount,
		char** end, size_t* remaining, DWORD flags,
		const char* format, Args... args) noexcept
	{
		return ::StringCchPrintfExA(buffer, bufferCount, end, remaining, flags, format, args...);
	}

	template <class... Args>
	HRESULT StringPrintfEx(wchar_t* buffer, size_t bufferCount,
		wchar_t** end, size_t* remaining, DWORD flags,
		const wchar_t* format, Args... args) noexcept
	{
		return ::StringCchPrintfExW(buffer, bufferCount, end, remaining, flags, format, args...);
	}

	template <class T>
	T Argument(T value) noexcept
	{
		return value;
	}

	template <class T>
	const T* Argument(const std::basic_string<T>& value) noexcept
	{
		return value.c_str();
	}

	template <class T, class ... Args>
	int Format(T* buffer, size_t bufferCount, const T* format, Args... args) noexcept
	{
		T* end = 0;
		HRESULT hr = StringPrintfEx(buffer, bufferCount, &end, 0, 0, format, Argument(args)...);
		if (end) return end - buffer;
		else return -1;
	}

	template <class T, class... Args>
	void Format(std::basic_string<T>& buffer, const T* format, Args... args)
	{
		size_t size = Format(&buffer[0], buffer.size() + 1, format, args...);
		if (size != -1)
			buffer.resize(size);
	}

	template <class T>
	struct FormatString : std::basic_string<T>
	{
		template <class... Args>
		FormatString(size_t size, const T* format, Args... args)
			: std::basic_string<T>(size, T())
		{
			Format(*this, format, args...);
		}
	};

	template <class T>
	const T* Argument(const FormatString<T>& value) noexcept
	{
		return value.c_str();
	}

	inline BSTR Argument(const _bstr_t& value) noexcept
	{
		return value;
	}
}
