#pragma once

namespace apn::workspace
{
	//
	// このクラスはコンソール用のシャトルです。
	//
	struct Console : SubProcess
	{
		virtual HWND find_window() override
		{
			return ::GetConsoleWindow();
		}
	}; inline std::shared_ptr<Console> console;
}
