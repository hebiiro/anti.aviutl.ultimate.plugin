#pragma once

namespace apn::filter_copy::controller
{
	//
	// このクラスはコントローラ層のアプリです。
	//
	inline struct app_t
	{
		//
		// 初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			if (!controller::config_io.init()) return FALSE;
			if (!controller::hook.init()) return FALSE;
			if (!view::setting_dialog.init()) return FALSE;

			if (!controller::config_io.read()) MY_TRACE("コンフィグの読み込みに失敗しました\n");

			return TRUE;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			controller::config_io.write();

			controller::hook.exit();
			controller::config_io.exit();
			view::setting_dialog.exit();

			return TRUE;
		}
	} app;
}
