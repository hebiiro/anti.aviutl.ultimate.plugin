#pragma once

#if defined(_DEBUG) && !defined(MY_TRACE_ENABLED)
#define MY_TRACE_ENABLED
#endif

#define MY_TRACE_EMPTY				((void)0)

#ifdef MY_TRACE_ENABLED

#define MY_TRACE(f, ...)			my::Tracer::output_text(_T(__FILE__), __LINE__, my::format(_T(f), __VA_ARGS__).c_str())
#define MY_TRACE_FUNC(f, ...)		my::Tracer::output_func(_T(__FILE__), __LINE__, _T(__FUNCTION__), _T(__FUNCSIG__), my::format(_T(f), __VA_ARGS__).c_str())
#define MY_TRACE_BINARY(buf, c)		my::Tracer::output_binary(_T(__FILE__), __LINE__, buf, c)

#define MY_TRACE_STR(xxx)			MY_TRACE(#xxx _T(" = {/}\n"), xxx)
#define MY_TRACE_INT(xxx)			MY_TRACE(#xxx _T(" = {/}\n"), xxx)
#define MY_TRACE_INT64(xxx)			MY_TRACE(#xxx _T(" = {/}\n"), xxx)
#define MY_TRACE_HEX(xxx)			MY_TRACE(#xxx _T(" = {/hex}\n"), xxx)
#define MY_TRACE_REAL(xxx)			MY_TRACE(#xxx _T(" = {/}\n"), xxx)
#define MY_TRACE_RECT(xxx)			MY_TRACE(#xxx _T(" = {/}, {/}, {/}, {/}\n"), (xxx).left, (xxx).top, (xxx).right, (xxx).bottom)
#define MY_TRACE_POINT(xxx)			MY_TRACE(#xxx _T(" = {/}, {/}\n"), (xxx).x, (xxx).y)
#define MY_TRACE_SIZE(xxx)			MY_TRACE(#xxx _T(" = {/}, {/}\n"), (xxx).cx, (xxx).cy)

#define MY_TRACE_RECT2(xxx) \
do \
{ \
	int x = (xxx).left; \
	int y = (xxx).top; \
	int w = (xxx).right - (xxx).left; \
	int h = (xxx).bottom - (xxx).top; \
	MY_TRACE(#xxx _T(" = {/}, {/}, {/}, {/}\n"), x, y, w, h); \
} \
while (false)

#define MY_TRACE_HWND(xxx) \
do \
{ \
	HWND ___hwnd = xxx; \
	auto ___window_text = my::get_window_text(___hwnd); \
	auto ___class_name = my::get_class_name(___hwnd); \
	MY_TRACE(#xxx _T(" = {/hex} = \"{/}\" ({/})\n"), ___hwnd, ___window_text, ___class_name); \
} \
while (false)

#define MY_TRACE_COM_ERROR(hr) MY_TRACE("{/hex} = {/}\n", hr, _com_error(hr).ErrorMessage())

#else

#define MY_TRACE(f, ...)			MY_TRACE_EMPTY
#define MY_TRACE_FUNC(f, ...)		MY_TRACE_EMPTY
#define MY_TRACE_BINARY(buf, c)		MY_TRACE_EMPTY

#define MY_TRACE_STR(xxx)			MY_TRACE_EMPTY
#define MY_TRACE_INT(xxx)			MY_TRACE_EMPTY
#define MY_TRACE_INT64(xxx)			MY_TRACE_EMPTY
#define MY_TRACE_HEX(xxx)			MY_TRACE_EMPTY
#define MY_TRACE_REAL(xxx)			MY_TRACE_EMPTY
#define MY_TRACE_RECT(xxx)			MY_TRACE_EMPTY
#define MY_TRACE_POINT(xxx)			MY_TRACE_EMPTY
#define MY_TRACE_SIZE(xxx)			MY_TRACE_EMPTY

#define MY_TRACE_RECT2(xxx)			MY_TRACE_EMPTY
#define MY_TRACE_HWND(xxx)			MY_TRACE_EMPTY

#define MY_TRACE_COM_ERROR(xxx)		MY_TRACE_EMPTY

#endif

namespace my
{
	//
	// このクラスはデバッグトレースを担当します。
	//
	struct Tracer
	{
		//
		// 文字列クラスです。
		//
		using string = std::basic_string<TCHAR>;

		//
		// 最大文字数です。
		//
		inline static constexpr int c_max_size = 2048;

		//
		// 文字列を出力するデフォルトのクラスです。
		//
		inline static struct Logger
		{
			virtual void output(LPCTSTR raw, LPCTSTR text)
			{
				::OutputDebugString(text);
			}
		} default_logger;

		//
		// 文字列を出力するクラスです。
		//
		inline static Logger* logger = &default_logger;

		//
		// origに含まれるold_stringをnew_stringに置換して返します。
		//
		inline static string replace(const string& orig, LPCTSTR old_string, LPCTSTR new_string)
		{
			string result;
			size_t old_string_length = _tcslen(old_string);
			size_t begin_pos = 0;

			while (true)
			{
				// old_stringを検索します。
				auto end_pos = orig.find(old_string, begin_pos);

				if (end_pos != string::npos)
				{
					// old_stringが見つかった場合の処理です。
					// 
					// begin_posからend_posまでを結果に追加します。
					result.append(orig, begin_pos, end_pos - begin_pos);

					// さらにnew_stringを結果に追加します。
					result.append(new_string);

					// 検索開始位置を進めます。
					begin_pos = end_pos + old_string_length;
				}
				else
				{
					// old_stringが見つからなかった場合の処理です。
					// 
					// begin_posから最後までを結果に追加します。
					result.append(orig, begin_pos);

					// ループを終了します。
					break;
				}
			}

			return result;
		}

		//
		// __FUNCSIG__を整形して返します。
		//
		inline static string get_func_name(LPCTSTR func, LPCTSTR func_sig)
		{
#if 1
			string func_name = func;

			// "unnamed-type-"を削除します。
			func_name = replace(func_name, _T("unnamed-type-"), _T(""));

			return func_name;
#else
			string func_name = func_sig;

			// "unnamed-type-"を削除します。
			func_name = replace(func_name, _T("unnamed-type-"), _T(""));

			// 最初の'('を検索します。
			auto end = func_name.find_first_of(_T('('));

			// 見つかった場合はリサイズして'('以降を削除します。
			if (end != string::npos) func_name.resize(end);

			// 最初の'<'を検索します。
			auto template_begin = func_name.find_first_of(_T('<'));

			// 最初の'<'より前にある最後の' 'を検索します。
			auto begin = func_name.find_last_of(_T(' '), template_begin);

			// 見つかった場合は先頭から' 'までを削除します。
			if (begin != string::npos) func_name.erase(0, begin + 1);

			return func_name;
#endif
		}

		//
		// 文字列を修飾して出力します。
		//
		inline static void output_text(LPCTSTR file, int line, const string& raw)
		{
			// ロガーが存在しない場合は何もしません。
			if (!logger) return;

			if (file)
			{
				// rawを修飾してからロガーに出力させます。
				logger->output(raw.c_str(), my::format(_T("{/}({/}) : {/}"), file, line, raw).c_str());
			}
			else
			{
				// rawをロガーに出力させます。
				logger->output(raw.c_str(), raw.c_str());
			}
		}

		//
		// 関数形式の文字列を出力します。
		//
		inline static void output_func(LPCTSTR file, int line, LPCTSTR func, LPCTSTR func_sig, LPCTSTR func_args)
		{
			// funcからfunc_nameを構築します。
			auto func_name = get_func_name(func, func_sig);

			// func_nameとfunc_argsを書式化して出力します。
			output_text(file, line, my::format(_T("{/}({/})\n"), func_name, func_args));
		}

		//
		// バイトを16進数文字にして返します。
		//
		inline static TCHAR ch(BYTE a)
		{
			if (a > 0x09)
				return _T('A') + a - 0x0A;
			else
				return _T('0') + a;
		}

		//
		// バイナリ形式の文字列を出力します。
		//
		inline static void output_binary(LPCTSTR file, int line, const void* buf, int c)
		{
			auto bytes = (const BYTE*)buf;

			if (!bytes)
			{
				output_text(file, line, _T("(null)\n"));
				return;
			}

			const int cch = 16;
			TCHAR text[cch * 3 + 2] = {};

			for (int i = 0; i < c;)
			{
				int j;
				for (j = 0; i < c && j < cch * 3; i++)
				{
					text[j++] = ch(bytes[i] >> 4 & 0x0F);
					text[j++] = ch(bytes[i] >> 0 & 0x0F);
					text[j++] = _T(' ');
				}
				text[j++] = _T('\n');
				text[j++] = _T('\0');

				output_text(file, line, text);
			}
		}
	};
}
