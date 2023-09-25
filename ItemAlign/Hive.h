#pragma once

namespace fgo::item_align
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"ItemAlign";
		inline static const LPCWSTR DisplayName = L"アイテム整列";

		HINSTANCE instance = 0;
		HWND mainWindow = 0;
		int frame = 0;

		template <class... Args>
		int messageBox(Args... args) {
			WCHAR text[MAX_PATH] = {};
			::StringCchPrintfW(text, std::size(text), args...);
			return ::MessageBoxW(mainWindow, text, DisplayName, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
