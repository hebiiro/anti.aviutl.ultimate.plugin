#pragma once

namespace fgo::namecage
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"Namecage";
		inline static const LPCWSTR DisplayName = L"アイテム名常時表示";

		HINSTANCE instance = 0;
		int offset = -6;
	} hive;
}
