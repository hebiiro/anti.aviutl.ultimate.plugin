#pragma once
#include "Theme/Manager.h"

namespace fgo::dark::hook
{
	//
	// このクラスは::DrawThemeParentBackground()をフックします。
	//
	inline struct
	{
		inline static HRESULT WINAPI hook(HWND hwnd, HDC dc, LPCRECT rc)
		{
			DWORD from = *((DWORD*)&hwnd - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, (%ws)", from, hwnd, hive.safe_string(rc));

			return orig(hwnd, dc, rc);
		}
		inline static decltype(&hook) orig = ::DrawThemeParentBackground;
	} DrawThemeParentBackground;

	//
	// このクラスは::DrawThemeBackground()をフックします。
	//
	inline struct
	{
		inline static HRESULT WINAPI hook(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, LPCRECT rcClip)
		{
			auto renderer = theme::manager.getRenderer(theme);
			if (renderer)
				return renderer->onDrawThemeBackground(theme, dc, partId, stateId, rc, rcClip);

			DWORD from = *((DWORD*)&theme - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, (%ws), (%ws)", from, theme, partId, stateId, hive.safe_string(rc), hive.safe_string(rcClip));

			return orig(theme, dc, partId, stateId, rc, rcClip);
		}
		inline static decltype(&hook) orig = ::DrawThemeBackground;
	} DrawThemeBackground;

	//
	// このクラスは::DrawThemeBackgroundEx()をフックします。
	//
	inline struct
	{
		inline static HRESULT WINAPI hook(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, const DTBGOPTS* options)
		{
			auto renderer = theme::manager.getRenderer(theme);
			if (renderer)
				return renderer->onDrawThemeBackgroundEx(theme, dc, partId, stateId, rc, options);

			DWORD from = *((DWORD*)&theme - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, (%ws), 0x%08X", from, theme, partId, stateId, hive.safe_string(rc), options);

			return orig(theme, dc, partId, stateId, rc, options);
		}
		inline static decltype(&hook) orig = ::DrawThemeBackgroundEx;
	} DrawThemeBackgroundEx;

	//
	// このクラスは::DrawThemeText()をフックします。
	//
	inline struct
	{
		inline static HRESULT WINAPI hook(HTHEME theme, HDC dc, int partId, int stateId, LPCWSTR text, int c, DWORD textFlags, DWORD textFlags2, LPCRECT rc)
		{
			auto renderer = theme::manager.getRenderer(theme);
			if (renderer)
				return renderer->onDrawThemeText(theme, dc, partId, stateId, text, c, textFlags, textFlags2, rc);

			DWORD from = *((DWORD*)&theme - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, %ws, 0x%08X, 0x%08X, (%ws)", from, theme, partId, stateId, hive.safe_string(text, c), textFlags, textFlags2, hive.safe_string(rc));

			return orig(theme, dc, partId, stateId, text, c, textFlags, textFlags2, rc);
		}
		inline static decltype(&hook) orig = ::DrawThemeText;
	} DrawThemeText;

	//
	// このクラスは::DrawThemeTextEx()をフックします。
	//
	inline struct
	{
		inline static HRESULT WINAPI hook(HTHEME theme, HDC dc, int partId, int stateId, LPCWSTR text, int c, DWORD textFlags, LPRECT rc, const DTTOPTS* options)
		{
			auto renderer = theme::manager.getRenderer(theme);
			if (renderer)
				return renderer->onDrawThemeTextEx(theme, dc, partId, stateId, text, c, textFlags, rc, options);

			DWORD from = *((DWORD*)&theme - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, %ws, 0x%08X, (%ws), 0x%08X", from, theme, partId, stateId, hive.safe_string(text, c), textFlags, hive.safe_string(rc), options);

			return orig(theme, dc, partId, stateId, text, c, textFlags, rc, options);
		}
		inline static decltype(&hook) orig = ::DrawThemeTextEx;
	} DrawThemeTextEx;

	//
	// このクラスは::DrawThemeIcon()をフックします。
	//
	inline struct
	{
		inline static HRESULT WINAPI hook(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, HIMAGELIST imageList, int imageIndex)
		{
			auto renderer = theme::manager.getRenderer(theme);
			if (renderer)
				return renderer->onDrawThemeIcon(theme, dc, partId, stateId, rc, imageList, imageIndex);

			DWORD from = *((DWORD*)&theme - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, (%ws), 0x%08X, %d", from, theme, partId, stateId, hive.safe_string(rc), imageList, imageIndex);

			return orig(theme, dc, partId, stateId, rc, imageList, imageIndex);
		}
		inline static decltype(&hook) orig = ::DrawThemeIcon;
	} DrawThemeIcon;

	//
	// このクラスは::DrawThemeEdge()をフックします。
	//
	inline struct
	{
		inline static HRESULT WINAPI hook(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT destRect, UINT edge, UINT flags, LPRECT contentRect)
		{
			auto renderer = theme::manager.getRenderer(theme);
			if (renderer)
				return renderer->onDrawThemeEdge(theme, dc, partId, stateId, destRect, edge, flags, contentRect);

			DWORD from = *((DWORD*)&theme - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, (%ws), 0x%08X, 0x%08X", from, theme, partId, stateId, hive.safe_string(destRect), edge, flags);

			return orig(theme, dc, partId, stateId, destRect, edge, flags, contentRect);
		}
		inline static decltype(&hook) orig = ::DrawThemeEdge;
	} DrawThemeEdge;

	//
	// このクラスは::OpenThemeData()をフックします。
	//
	inline struct
	{
		inline static HTHEME WINAPI hook(HWND hwnd, LPCWSTR classList)
		{
			HTHEME theme = orig(hwnd, classList);
			MY_TRACE_EX("0x%08X, %ws => 0x%08X", hwnd, classList, theme);
			return theme;
		}
		inline static decltype(&hook) orig = ::OpenThemeData;
	} OpenThemeData;

	//
	// このクラスは::OpenThemeDataForDpi()をフックします。
	//
	inline struct
	{
		inline static HTHEME WINAPI hook(HWND hwnd, LPCWSTR classList, UINT dpi)
		{
			HTHEME theme = orig(hwnd, classList, dpi);
			MY_TRACE_EX("0x%08X, %ws, %d => 0x%08X", hwnd, classList, dpi, theme);
			return theme;
		}
		inline static decltype(&hook) orig = ::OpenThemeDataForDpi;
	} OpenThemeDataForDpi;

	//
	// このクラスは::OpenThemeDataEx()をフックします。
	//
	inline struct
	{
		inline static HTHEME WINAPI hook(HWND hwnd, LPCWSTR classList, DWORD flags)
		{
			HTHEME theme = orig(hwnd, classList, flags);
			MY_TRACE_EX("0x%08X, %ws, 0x%08X => 0x%08X", hwnd, classList, flags, theme);
			return theme;
		}
		inline static decltype(&hook) orig = ::OpenThemeDataEx;
	} OpenThemeDataEx;

	//
	// このクラスは::SetWindowTheme()をフックします。
	//
	inline struct
	{
		inline static HRESULT WINAPI hook(HWND hwnd, LPCWSTR subAppName, LPCWSTR subIdList)
		{
			MY_TRACE_EX("0x%08X, %ws, %ws", hwnd, subAppName, subIdList);
			return orig(hwnd, subAppName, subIdList);
		}
		inline static decltype(&hook) orig = ::SetWindowTheme;
	} SetWindowTheme;
}
