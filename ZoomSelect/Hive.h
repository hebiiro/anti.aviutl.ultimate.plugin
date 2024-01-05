#pragma once

namespace fgo::zoom_select
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		struct Label { int value; LPCWSTR label; };

		inline static const LPCWSTR Name = L"ZoomSelect";
		inline static const LPCWSTR DisplayName = L"ズーム選択";

		struct Zoom {
			inline static const int Min = 0;
			inline static const int Max = 26;
			inline static const int MaxSize = 6;
		};

		HINSTANCE instance = 0;
		HWND mainWindow = 0;
		int zoom[Zoom::MaxSize] = { 0, 5, 10, 15, 20, 26 }; // ズーム値の配列。

		template <class... Args>
		int messageBox(Args... args) {
			WCHAR text[MAX_PATH] = {};
			::StringCchPrintfW(text, std::size(text), args...);
			return ::MessageBoxW(mainWindow, text, DisplayName, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
