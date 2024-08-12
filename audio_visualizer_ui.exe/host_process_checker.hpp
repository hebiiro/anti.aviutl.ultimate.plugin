#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはホストプロセスを監視します。
	// ホストプロセスが無効になった場合はこのプロセスを終了します。
	// これにより、ホストプロセスが強制終了したとしても、
	// このプロセスの残骸が残らないようになります。
	//
	inline struct HostProcessChecker
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// ホストプロセスを監視するスレッドタイマーを作成します。
			::SetTimer(hive.main_window, (UINT_PTR)this, 1000,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
				{
					if (!::IsWindow(hive.host_window))
					{
						MY_TRACE("ホストプロセスが無効になりました\n");

						::PostQuitMessage(0);
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

			::KillTimer(hive.main_window, (UINT_PTR)this);

			return TRUE;
		}
	} host_process_checker;
}
