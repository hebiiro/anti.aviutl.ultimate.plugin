#pragma once

namespace fgo::encode_guard
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCTSTR AppName = _T("エンコードガード");

		HINSTANCE instance = 0;
		HWND mainWindow = 0;
		BOOL checkRange = TRUE;
		BOOL checkLastFrame = TRUE;
		BOOL checkFrameRate = TRUE;
	} hive;
}
