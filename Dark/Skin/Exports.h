#pragma once

namespace fgo::dark::skin
{
	struct ColorSet
	{
		COLORREF fillColor;
		COLORREF edgeColor;
		COLORREF textBkColor;
		COLORREF textForeColor;
		COLORREF textBackColor;

		struct Type
		{
			static const int fillColor = 0;
			static const int edgeColor = 1;
			static const int textBkColor = 2;
			static const int textForeColor = 3;
			static const int textBackColor = 4;
		};
	};

	struct Exports
	{
		ColorSet* (WINAPI* dark_getNamedColor)(LPCWSTR name) = 0;

		//
		// ダークモードプラグインのエクスポート関数を取得します。
		// 最初に1回だけ呼び出してください。
		//
		BOOL init()
		{
			// Dark.auaを取得します。
			HMODULE dark = ::GetModuleHandleA("Dark.aua");

			// Dark.auaが取得できなかった場合はDarkenWindow.aulを取得します。
			if (!dark) dark = ::GetModuleHandleA("DarkenWindow.aul");

			// どちらも取得できなかった場合は失敗します。
			if (!dark) return FALSE;

			// dark_getNamedColorを取得します。
			dark_getNamedColor = (decltype(dark_getNamedColor))::GetProcAddress(dark, "dark_getNamedColor");
			if (!dark_getNamedColor) return FALSE; // エクスポート関数を取得できなかった場合は失敗します。

			return TRUE;
		}

		//
		// ダークモードが有効ならTRUEを返します。
		//
		BOOL isEnabled()
		{
			return !!dark_getNamedColor;
		}

		//
		// 指定された名前のカラーセットを返します。
		// 指定された名前のカラーセットが存在しない場合は0を返します。
		//
		ColorSet* getNamedColor(LPCWSTR name)
		{
			if (!dark_getNamedColor) return 0;

			return dark_getNamedColor(name);
		}
	} exports;
}
