#pragma once

namespace apn::filter_drag
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
			timer_id = ::SetTimer(0, 0, 1000,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
				{
					if (config_checker.check_update())
					{
						MY_TRACE("コンフィグファイルが更新されました\n");

						config_io.init(); // コンフィグファイルを再ロードします。
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

			::KillTimer(0, timer_id);

			return TRUE;
		}
	} config_checker;
}
