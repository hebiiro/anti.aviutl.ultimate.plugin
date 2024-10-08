#pragma once

namespace apn::item_wave::ui
{
	inline struct ConfigUpdateChecker
		: my::FileUpdateChecker2
		, my::FileUpdateChecker2::Handler
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			__super::init(hive.config_file_name.c_str(), hive.ui_window, this);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			__super::exit();

			return TRUE;
		}

		//
		// この仮想関数はコンフィグファイルが変更されたときに呼び出されます。
		//
		virtual void on_file_update(my::FileUpdateChecker2* checker) override
		{
			MY_TRACE("コンフィグファイルが更新されたのでコンフィグを再読み込みします\n");

			app->read_config();
		}
	} config_update_checker;
}
