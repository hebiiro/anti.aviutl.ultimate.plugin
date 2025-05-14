#include "pch.h"
#include "ffmpeg.hpp"
#include "app.hpp"

namespace apn::item_wave::reader
{
	int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
	{
		_tsetlocale(LC_CTYPE, _T(""));

		struct Initializer {
			Initializer() { my::tracer_to_file::init(nullptr); }
			~Initializer() { my::tracer_to_file::exit(); }
		} initializer;

		MY_TRACE_FUNC("{/}", cmd_line);

		try
		{
			app.main();

			MY_TRACE("プロセスが正常終了しました\n");

			return 0;
		}
		catch (...)
		{
			MY_TRACE("プロセスが異常終了しました\n");

			return 20250512;
		}
	}
}

int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
{
	return apn::item_wave::reader::WinMain(instance, prev_instance, cmd_line, cmd_show);
}
