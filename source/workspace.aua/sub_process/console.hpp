#pragma once

namespace apn::workspace
{
	//
	// このクラスはコンソール用のシャトルです。
	//
	struct Console : SubProcess
	{
		//
		// コンソールを初期化します。
		//
		virtual BOOL on_init()
		{
			MY_TRACE_FUNC("");

			if (!__super::on_init()) return FALSE;

			// 最小化されているので元に戻します。
			::ShowWindowAsync(window, SW_RESTORE);

			// WM_SIZEをポストしてresize()を実行します。
			::PostMessage(*this, WM_SIZE, 0, 0);

			return TRUE;
		}

		//
		// コンソールを見つけます。
		//
		virtual BOOL on_find_window()
		{
			MY_TRACE_FUNC("");

			window = ::GetConsoleWindow();

			return window != nullptr;
		}
	}; inline auto console = std::make_shared<Console>();
}
