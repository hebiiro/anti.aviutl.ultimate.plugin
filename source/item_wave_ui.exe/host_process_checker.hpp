#pragma once

namespace apn::item_wave::ui
{
	//
	// このクラスはホストプロセスを監視します。
	//
	inline struct HostProcessChecker
	{
		//
		// ホストプロセス監視用のタイマーIDです。
		//
		UINT_PTR timer_id = 0;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ホストプロセスを監視するタイマーを作成します。
			timer_id = ::SetTimer(nullptr, 0, 1000,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
				{
					if (!::IsWindow(hive.host_window))
					{
						MY_TRACE("ホストプロセスが無効になりました\n");

						::PostQuitMessage(0);
					}
				});

			return !!timer_id;
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
	} host_process_checker;
}
