#pragma once

namespace fgo::font_select
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static const LPCWSTR Name = L"FontSelect";
		inline static const LPCWSTR DisplayName = L"フォント選択";

		struct ControlID {
			inline static const UINT FontComboBox = 0x5654;
		};

		HINSTANCE instance = 0;
		HWND mainWindow = 0;
		int itemWidth = 400; // アイテムの幅。
		int itemHeight = 48; // アイテムの高さ。
		int fontHeight = 34; // フォントの高さ。
		_bstr_t previewText = L"おはヨーOl|0123456789"; // プレビュー用に表示するテキスト。
		BOOL marge = FALSE;
		BOOL useContextMenu = TRUE; // コンテキストメニューを使用するかどうか。
		BOOL useBackward = TRUE; // 戻るボタンを使用するかどうか。
		BOOL useForward = TRUE; // 進むボタンを使用するかどうか。

		template <class... Args>
		int messageBox(Args... args) {
			WCHAR text[MAX_PATH] = {};
			::StringCchPrintfW(text, std::size(text), args...);
			return ::MessageBoxW(mainWindow, text, DisplayName, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
