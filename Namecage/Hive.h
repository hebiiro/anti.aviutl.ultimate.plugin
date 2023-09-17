#pragma once

namespace fgo::namecage
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCTSTR AppName = _T("ネームケージ");

		HINSTANCE instance = 0;
	} hive;
}
