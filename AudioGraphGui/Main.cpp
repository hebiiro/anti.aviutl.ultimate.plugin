#include "pch.h"
#include "App.h"

//
// エントリポイントです。
//
int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prevInstance, LPTSTR cmdLine, int cmdShow)
{
	using namespace fgo::audio_graph_gui;

	return app.WinMain(instance, prevInstance, cmdLine, cmdShow);
}
