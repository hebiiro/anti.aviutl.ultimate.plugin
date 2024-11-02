#pragma once

namespace apn::ease_select_ui
{
	//
	// このクラスはコンフィグファイルの更新を監視します。
	// これにより、ユーザーがコンフィグファイルを直接編集した場合でも
	// 変更が即座に反映されるようになります。
	//
	struct ConfigChecker : my::FileUpdateChecker
	{
		UINT_PTR timer_id = 0;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ファイルアップデートチェッカー(スーパークラス)を初期化します。
			__super::init(hive.config_file_name.c_str());

			// コンフィグファイルを監視するタイマーを作成します。
			timer_id = ::SetTimer(nullptr, 0, 1000,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
				{
					if (config_checker.check_update())
					{
						MY_TRACE("コンフィグファイルが更新されました\n");

						hive.app->read_config(); // コンフィグを再読み込みします。
					}
				});

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			::KillTimer(nullptr, timer_id);

			return TRUE;
		}
	} config_checker;
}
