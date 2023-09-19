#pragma once

namespace fgo::single_out
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		//
		// このクラスは他のクラスから通知を受け取ります。
		//
		struct Commander {
			virtual BOOL onExportImage(BOOL hasAlpha, BOOL itemOnly) = 0;
		} *commander = 0;

		inline static const LPCWSTR Name = L"SingleOut";
		inline static const LPCWSTR DisplayName = L"画像のエクスポート";

		HINSTANCE instance = 0;
		HWND mainWindow = 0;
		int quality = 90;
	} hive;
}
