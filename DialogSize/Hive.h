#pragma once

namespace fgo::dialog_size
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCTSTR AppName = _T("ダイアログサイズ調整");

		HINSTANCE instance = 0;
	} hive;
}
