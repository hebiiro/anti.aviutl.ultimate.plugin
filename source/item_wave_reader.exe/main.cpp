#include "pch.h"
#include "hive.hpp"
#include "input_plugin.hpp"
#include "worker.hpp"
#include "app.hpp"

namespace apn::item_wave::reader
{
	int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPTSTR cmdLine, int cmdShow)
	{
		struct Initializer
		{
			Initializer()
			{
				_tsetlocale(LC_CTYPE, _T(""));
				my::tracer_to_file::init(nullptr);
				::OleInitialize(nullptr);
			}

			~Initializer()
			{
				my::tracer_to_file::exit();
				::OleUninitialize();
			}

		} initializer;

		MY_TRACE_FUNC("");

		app.init(instance);
		app.receive();
		app.send();
		app.exit();

		MY_TRACE("プロセスが正常終了しました\n");

		return 0;
	}
}

int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
{
	return apn::item_wave::reader::WinMain(instance, prev_instance, cmd_line, cmd_show);
}
