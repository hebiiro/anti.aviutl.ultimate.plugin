#pragma once

namespace fgo::last_frame
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCTSTR AppName = _T("最終フレーム自動調整");

		HINSTANCE instance = 0;
		BOOL enable = TRUE;
	} hive;
}
