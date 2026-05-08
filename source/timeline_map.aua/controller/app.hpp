#pragma once

namespace apn::timeline_map::controller
{
	//
	// このクラスはコントローラ層のアプリケーションです。
	//
	inline struct app_t
	{

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// コンフィグ入出力を初期化します。
			if (!config_io.init())
			{
				MY_TRACE("コンフィグ入出力の初期化に失敗しました\n");
			}

			// フックを初期化します。
			if (!hook::exedit_window.init())
			{
				view::overview.message_box(L"フックの初期化に失敗しました\n");

				return FALSE;
			}

			if (!view::overview.init())
			{
				view::overview.message_box(L"全体図の初期化に失敗しました\n");

				return FALSE;
			}

			// コンフィグを読み込みます。
			config_io.read();

			return FALSE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// コンフィグを書き込みます。
			config_io.write();

			// 全体図を終了します。
			view::overview.exit();

			// フックを終了します。
			hook::exedit_window.exit();

			// コンフィグ入出力を終了します。
			config_io.exit();

			return FALSE;
		}
	} app;
}
