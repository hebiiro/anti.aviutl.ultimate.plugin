#pragma once
#include "SubProcess.h"

namespace fgo::nest
{
	//
	// このクラスはコンソール用のシャトルです。
	//
	struct Console : SubProcess
	{
		HWND findWindow() override
		{
			return ::GetConsoleWindow();
		}
	}; inline std::shared_ptr<Console> console;
}
