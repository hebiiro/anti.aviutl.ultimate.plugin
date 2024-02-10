#pragma once

namespace fgo::font_preview
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"FontPreview";
		inline static const LPCWSTR DisplayName = L"フォントプレビュー";

		struct ControlID {
			inline static const UINT FontComboBox = 0x5654;
		};

		struct WindowMessage {
			inline static const UINT DropDown = ::RegisterWindowMessage(_T("FontPreview::DropDown"));
		};

		HINSTANCE instance = 0;
		HWND mainWindow = 0;
		int itemHeight = 48; // アイテムの高さ。
		int listboxWidth = 400; // リストボックスの幅。
		int listboxHeight = 600; // リストボックスの高さ。
		_bstr_t previewText = L"AaＡａあア亜"; // プレビュー用に表示するテキスト。
		BOOL marge = FALSE;

		template <class... Args>
		int messageBox(Args... args) {
			WCHAR text[MAX_PATH] = {};
			::StringCchPrintfW(text, std::size(text), args...);
			return ::MessageBoxW(mainWindow, text, DisplayName, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
