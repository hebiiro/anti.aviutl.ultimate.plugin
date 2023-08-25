#pragma once

namespace fgo::filer
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		HINSTANCE instance = 0; // この DLL のインスタンスハンドルです。
	} hive;
}
