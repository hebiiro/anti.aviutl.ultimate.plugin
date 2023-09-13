#pragma once

namespace fgo::filer
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCTSTR AppName = _T("ファイラ");

		HINSTANCE instance = 0; // このDLLのインスタンスハンドルです。
		HWND mainWindow = 0;
		HWND activeWindow = 0;
		BOOL useCommonDialog = TRUE;
		Share::Nest::Exports nest;
	} hive;
}
