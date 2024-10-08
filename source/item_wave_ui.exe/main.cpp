#include "pch.h"
#include "resource.h"
#include "app_interface.hpp"
#include "hive.hpp"
#include "share.hpp"
#include "reader.hpp"
#include "waiter.hpp"
#include "cache.hpp"
#include "config_update_checker.hpp"
#include "host_process_checker.hpp"
#include "ui_window.hpp"
#include "config_io.hpp"
#include "app.hpp"

namespace apn::item_wave::ui
{
	int APIENTRY WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
	{
		// デバッグトレースとCOMを初期化します。
		struct Initializer
		{
			Initializer()
			{
				_tsetlocale(LC_ALL, _T(""));
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

		// インスタンスハンドルをハイブに格納します。
		hive.instance = instance;

		if (!app->init())
		{
			MY_TRACE("アプリケーションの初期化に失敗しました\n");

			return -1;
		}

		// メッセージループを開始します。
		MSG msg = {};
		while (::GetMessage(&msg, 0, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		app->exit();

		MY_TRACE("プロセスが正常終了しました\n");

		// プロセスを終了します。
		return 0;
	}
}

int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
{
	return apn::item_wave::ui::WinMain(instance, prev_instance, cmd_line, cmd_show);
}
