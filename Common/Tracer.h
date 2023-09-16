#pragma once
#include "StringPrint.h"

#ifdef _DEBUG
#define MY_TRACE(format, ...)		Tools::Tracer::output_format(_T(__FILE__), __LINE__, format, __VA_ARGS__)
#define MY_TRACE_BINARY(buf, c)		Tools::Tracer::output_binary(_T(__FILE__), __LINE__, buf, c)
#define MY_TRACE_FUNC(format, ...)	Tools::Tracer::output_func(_T(__FILE__), __LINE__, _T(__FUNCSIG__), _T(format), __VA_ARGS__)
#else
#define MY_TRACE(format, ...)		((void)0)
#define MY_TRACE_BINARY(buf, c)		((void)0)
#define MY_TRACE_FUNC(format, ...)	((void)0)
#endif

#define MY_TRACE_STR(xxx)	MY_TRACE(_T(#xxx) _T(" = %hs\n"), xxx)
#define MY_TRACE_WSTR(xxx)	MY_TRACE(_T(#xxx) _T(" = %ws\n"), xxx)
#define MY_TRACE_TSTR(xxx)	MY_TRACE(_T(#xxx) _T(" = %s\n"), xxx)
#define MY_TRACE_INT(xxx)	MY_TRACE(_T(#xxx) _T(" = %d\n"), xxx)
#define MY_TRACE_INT64(xxx)	MY_TRACE(_T(#xxx) _T(" = %lld\n"), xxx)
#define MY_TRACE_HEX(xxx)	MY_TRACE(_T(#xxx) _T(" = 0x%08X\n"), xxx)
#define MY_TRACE_REAL(xxx)	MY_TRACE(_T(#xxx) _T(" = %f\n"), xxx)
#define MY_TRACE_RECT(xxx)	MY_TRACE(_T(#xxx) _T(" = %d, %d, %d, %d\n"), (xxx).left, (xxx).top, (xxx).right, (xxx).bottom)
#define MY_TRACE_POINT(xxx)	MY_TRACE(_T(#xxx) _T(" = %d, %d\n"), (xxx).x, (xxx).y)
#define MY_TRACE_SIZE(xxx)	MY_TRACE(_T(#xxx) _T(" = %d, %d\n"), (xxx).cx, (xxx).cy)

#ifdef _DEBUG

#define MY_TRACE_RECT2(xxx) \
do \
{ \
	int x = (xxx).left; \
	int y = (xxx).top; \
	int w = (xxx).right - (xxx).left; \
	int h = (xxx).bottom - (xxx).top; \
	MY_TRACE(_T(#xxx) _T(" = %d, %d, %d, %d\n"), x, y, w, h); \
} \
while (0)

#define MY_TRACE_HWND(hwnd) \
do \
{ \
	HWND hwndCache = hwnd; \
	TCHAR windowText[MAX_PATH] = {}; \
	::GetWindowText(hwndCache, windowText, MAX_PATH); \
	TCHAR className[MAX_PATH] = {}; \
	::GetClassName(hwndCache, className, MAX_PATH); \
	MY_TRACE(_T(#hwnd) _T(" = 0x%08X = \"%s\" (%s)\n"), hwndCache, windowText, className); \
} \
while (0)

#else

#define MY_TRACE_RECT2		MY_TRACE
#define MY_TRACE_HWND		MY_TRACE

#endif

#define MY_TRACE_COM_ERROR(hr) MY_TRACE(_T("0x%08X = %s\n"), hr, _com_error(hr).ErrorMessage())

namespace Tools {
	struct Tracer {
		using string = std::basic_string<TCHAR>;

		inline static constexpr int maxSize = 2048;
		inline static struct Logger {
			virtual void output(LPCTSTR raw, LPCTSTR text) {
				::OutputDebugString(text);
			}
		} default_logger;
		inline static Logger* logger = &default_logger;

		//
		// 文字列を修飾して出力します。
		//
		static void output_text(LPCTSTR file, int line, LPCTSTR raw) {
			// ロガーが存在しない場合は何もしません。
			if (!logger) return;

			if (file) {
				// rawを修飾してからロガーに出力させます。
				logger->output(raw, FormatString(maxSize, _T("%s(%d) : %s"), file, line, raw).c_str());
			} else {
				// rawをロガーに出力させます。
				logger->output(raw, raw);
			}
		}

		//
		// 書式化された文字列を出力します。
		//
		template <class... Args>
		static void output_format(LPCTSTR file, int line, LPCTSTR format, Args... args) {
			FormatString text(maxSize, format, args...);
			output_text(file, line, text.c_str());
		}

		static TCHAR ch(BYTE a) {
			if (a > 0x09)
				return _T('A') + a - 0x0A;
			else
				return _T('0') + a;
		}

		//
		// バイナリ文字列を出力します。
		//
		static void output_binary(LPCTSTR file, int line, const void* buf, int c) {
			const BYTE* bytes = (const BYTE*)buf;

			if (!bytes) {
				output_text(file, line, _T("(null)\n"));
				return;
			}

			constexpr int cch = 16;
			TCHAR text[cch * 3 + 2] = {};

			for (int i = 0; i < c;) {
				int j;
				for (j = 0; i < c && j < cch * 3; i++) {
					text[j++] = ch(bytes[i] >> 4 & 0x0F);
					text[j++] = ch(bytes[i] >> 0 & 0x0F);
					text[j++] = _T(' ');
				}
				text[j++] = _T('\n');
				text[j++] = _T('\0');

				output_text(file, line, text);
			}
		}

		//
		// 関数文字列を出力します。
		//
		template <class... Args>
		static void output_func(LPCTSTR file, int line, LPCTSTR func, LPCTSTR format, Args... args) {
			//
			// formatとargsからfunc_argsを構築します。
			//
			FormatString<TCHAR> func_args(maxSize, format, args...);

			//
			// funcからfunc_nameを構築します。
			//
			string func_name = func;
			size_t end = func_name.find_first_of(_T('('));
			if (end != string::npos) func_name.resize(end);
			size_t template_begin = func_name.find_first_of(_T('<'));
			size_t begin = func_name.find_last_of(_T(' '), template_begin);
			if (begin != string::npos) func_name.erase(0, begin + 1);

			//
			// func_nameとfunc_argsを書式化して出力します。
			//
			output_format(file, line, _T("%s(%s)\n"), func_name, func_args);
		}
	};
}
