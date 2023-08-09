#include "pch.h"
#include "Story.h"

using namespace fgo;

//
// デバッグ用コールバック関数。デバッグメッセージを出力する。
//
void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}

//
// 初期化
//
BOOL func_init(AviUtl::FilterPlugin* fp)
{
	return story.func_init(fp);
}

//
// 終了
//
BOOL func_exit(AviUtl::FilterPlugin* fp)
{
	return story.func_exit(fp);
}

//
// フィルタ関数
//
BOOL func_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
{
	return story.func_proc(fp, fpip);
}

//
// ウィンドウ関数
//
BOOL func_WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
{
	return story.func_WndProc(hwnd, message, wParam, lParam, editp, fp);
}

//
// エントリポイント
//
BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	return story.DllMain(instance, reason, reserved);
}

//
// フィルタ構造体のポインタを渡す関数
//
AviUtl::FilterPluginDLL* WINAPI GetFilterTable()
{
	LPCSTR name = "AviUtlチューナープラグイン";
	LPCSTR information = "AviUtlチューナープラグイン r1 by 蛇色";

	static AviUtl::FilterPluginDLL filter = {
		.flag =
//			AviUtl::FilterPlugin::Flag::NoConfig | // このフラグを指定するとウィンドウが作成されなくなってしまう。
			AviUtl::FilterPlugin::Flag::AlwaysActive | // このフラグがないと「フィルタ」に ON/OFF を切り替えるための項目が追加されてしまう。
			AviUtl::FilterPlugin::Flag::DispFilter | // このフラグがないと「設定」の方にウィンドウを表示するための項目が追加されてしまう。
			AviUtl::FilterPlugin::Flag::ExInformation,
		.name = name,
//		.func_proc = func_proc,
		.func_init = func_init,
		.func_exit = func_exit,
		.func_WndProc = func_WndProc,
		.information = information,
	};

	return &filter;
}

//
// このプロセスで唯一の Fate オブジェクトを返します。
//
Fate* WINAPI get_fate()
{
	static Fate fate;

	return &fate;
}

//
// このプロセスで唯一の Universe オブジェクトを返します。
//
Universe* WINAPI get_universe()
{
	static Universe universe;

	return &universe;
}
