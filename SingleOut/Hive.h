#pragma once

namespace fgo::single_out
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"SingleOut";
		inline static const LPCWSTR DisplayName = L"画像のエクスポート";

		HINSTANCE instance = 0;
		HWND mainWindow = 0;

		int quality = 90;
	} hive;
}
