#pragma once

namespace fgo::text_split
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"TextSplit";
		inline static const LPCWSTR DisplayName = L"テキスト分解";

		HINSTANCE instance = 0;
		HWND mainWindow = 0;
		int frame = 0;
		int fitWidth = 0;
		BOOL absolute = FALSE;
		BOOL row = FALSE;
		BOOL fit = FALSE;
		BOOL deleteOrig = FALSE;
	} hive;
}
