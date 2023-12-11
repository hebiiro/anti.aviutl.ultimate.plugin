#pragma once

namespace fgo::easing_select
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCTSTR AppName = _T("イージング選択");

		HINSTANCE instance = 0; // このDLLのインスタンスハンドルです。
		HWND mainWindow = 0;
	} hive;
}
