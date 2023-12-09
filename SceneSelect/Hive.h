#pragma once

namespace fgo::scene_select
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"SceneSelect";
		inline static const LPCWSTR DisplayName = L"シーン選択";

		HINSTANCE instance = 0;
		HWND mainWindow = 0;

		template <class... Args>
		int messageBox(Args... args) {
			WCHAR text[MAX_PATH] = {};
			::StringCchPrintfW(text, std::size(text), args...);
			return ::MessageBoxW(mainWindow, text, DisplayName, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
