#pragma once

namespace apn::ease_select_ui
{
	//
	// このクラスはホストプロセスを監視します。
	//
	struct HostChecker
	{
		UINT_PTR timer_id = 0;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// コンフィグファイルを監視するタイマーを作成します。
			timer_id = ::SetTimer(nullptr, 0, 1000,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
				{
					if (!::IsWindow(hive.host_window))
					{
						MY_TRACE("ホストプロセスが無効になりました\n");

						::PostMessage(hive.main_window, WM_CLOSE, 0, 0);
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
	} host_checker;
}
