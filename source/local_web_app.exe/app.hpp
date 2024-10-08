#pragma once

namespace apn::local_web_app
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// コンストラクタです。
		//
		App() { app = this; }

		//
		// 初期化処理を実行します。
		//
		virtual BOOL init() override
		{
			MY_TRACE_FUNC("");

			if (!config_io.init()) return FALSE;
			if (!main_window.init()) return FALSE;

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL exit() override
		{
			MY_TRACE_FUNC("");

			main_window.exit();
			config_io.exit();

			return TRUE;
		}
	} app_impl;
}
