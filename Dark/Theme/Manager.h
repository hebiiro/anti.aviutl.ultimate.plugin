#pragma once
#include "Renderer.h"

namespace fgo::dark::theme
{
	//
	// このクラスはテーマレンダラを管理します。
	//
	inline struct Manager
	{
		//
		// テーマレンダラのコレクションです。
		//
		std::unordered_map<HTHEME, Renderer*> collection;

		Renderer* getRenderer(HTHEME theme) const
		{
			// レンダラの使用が抑制されている場合は0を返します。
			if (hive.lockRenderer) return 0;

			auto it = collection.find(theme);
			if (it == collection.end()) return 0;
			return it->second;
		}

		//
		// テーマレンダラをコレクションに追加します。
		//
		void addRenderer(HTHEME theme, Renderer* renderer)
		{
			collection[theme] = renderer;
		}

		//
		// テーマレンダラをコレクションに追加します。
		//
		void addRenderer(HWND hwnd, LPCWSTR vsclass, Renderer* renderer)
		{
			HTHEME theme = ::OpenThemeData(hwnd, vsclass);
			MY_TRACE(_T("%ws == 0x%08X\n"), vsclass, theme);
			addRenderer(theme, renderer);
		}

		//
		// テーマレンダラをコレクションに追加します。
		//
		void addRendererNC(HWND hwnd, LPCWSTR vsclass, Renderer* renderer)
		{
			UINT dpi = ::GetDpiForWindow(hwnd);
			HTHEME theme = ::OpenThemeDataForDpi(hwnd, vsclass, dpi);
//			HTHEME theme = ::OpenThemeDataEx(hwnd, vsclass, OTD_NONCLIENT);
			MY_TRACE(_T("%ws == 0x%08X\n"), vsclass, theme);
			addRenderer(theme, renderer);
		}
	} manager;
}
