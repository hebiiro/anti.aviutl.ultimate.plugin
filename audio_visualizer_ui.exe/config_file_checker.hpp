#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはコンフィグファイルを監視します。
	// これにより、ユーザーがコンフィグファイルを直接編集した場合でも、
	// 変更が即時に反映されるようになります。
	//
	inline struct ConfigFileChecker
		: my::FileUpdateChecker2
		, my::FileUpdateChecker2::Handler
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			__super::init(hive.config_file_name.c_str(), hive.main_window, this);

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

		virtual void on_file_update(my::FileUpdateChecker2* checker) override
		{
			MY_TRACE("コンフィグファイルが更新されました\n");

			app->read_config();
		}
	} config_file_checker;
}
