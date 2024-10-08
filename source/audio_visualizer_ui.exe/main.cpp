#include "pch.h"
#include "app_interface.hpp"
#include "hive.hpp"
#include "utils.hpp"
#include "host_process_checker.hpp"
#include "config_file_checker.hpp"
#include "browser.hpp"
#include "visual_editor.hpp"
#include "visual.hpp"
#include "visual_manager.hpp"
#include "main_window.hpp"
#include "config_io.hpp"
#include "app.hpp"

//
// エントリポイントです。
//
int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmdLine, int show_cmd)
{
	using namespace apn::audio_visualizer::ui;

	_tsetlocale(LC_CTYPE, _T(""));

	my::tracer_to_file::init(hive.instance = instance);

	MY_TRACE_FUNC("");

	::CoInitialize(nullptr);

	if (app->init())
		app->run();
	app->exit();

	::CoUninitialize();

	my::tracer_to_file::exit();

	return 0;
}
