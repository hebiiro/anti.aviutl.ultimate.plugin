#pragma once
#include "Skin.h"
#include "Theme/Renderer.h"

namespace fgo::dark::theme
{
	inline struct SpinRenderer : Renderer
	{
		HRESULT onDrawThemeParentBackground(HWND hwnd, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, (%ws)", hwnd, dc, hive.safe_string(rc));

			return hive.orig.DrawThemeParentBackground(hwnd, dc, rc);
		}

		HRESULT onDrawThemeBackground(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, LPCRECT rcClip) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, %d, %d, (%ws), (%ws)", theme, dc, partId, stateId, hive.safe_string(rc), hive.safe_string(rcClip));

			{
				if (skin::theme::manager.onDrawThemeBackground(theme, dc, partId, stateId, rc))
					return S_OK;
			}

			return hive.orig.DrawThemeBackground(theme, dc, partId, stateId, rc, rcClip);
		}

		HRESULT onDrawThemeBackgroundEx(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, const DTBGOPTS* options) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, %d, %d, (%ws), 0x%08X", theme, dc, partId, stateId, hive.safe_string(rc), options);

			return hive.orig.DrawThemeBackgroundEx(theme, dc, partId, stateId, rc, options);
		}

		HRESULT onDrawThemeText(HTHEME theme, HDC dc, int partId, int stateId, LPCWSTR text, int c, DWORD textFlags, DWORD textFlags2, LPCRECT rc) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, %d, %d, %ws, 0x%08X, 0x%08X, (%ws)", theme, dc, partId, stateId, hive.safe_string(text, c), textFlags, textFlags2, hive.safe_string(rc));

			return hive.orig.DrawThemeText(theme, dc, partId, stateId, text, c, textFlags, textFlags2, rc);
		}

		HRESULT onDrawThemeTextEx(HTHEME theme, HDC dc, int partId, int stateId, LPCWSTR text, int c, DWORD textFlags, LPRECT rc, const DTTOPTS* options) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, %d, %d, %ws, 0x%08X, (%ws), 0x%08X", theme, dc, partId, stateId, hive.safe_string(text, c), textFlags, hive.safe_string(rc), options);

			return hive.orig.DrawThemeTextEx(theme, dc, partId, stateId, text, c, textFlags, rc, options);
		}

		HRESULT onDrawThemeIcon(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT rc, HIMAGELIST imageList, int imageIndex) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, %d, %d, (%ws), 0x%08X, %d", theme, dc, partId, stateId, hive.safe_string(rc), imageList, imageIndex);

			return hive.orig.DrawThemeIcon(theme, dc, partId, stateId, rc, imageList, imageIndex);
		}

		HRESULT onDrawThemeEdge(HTHEME theme, HDC dc, int partId, int stateId, LPCRECT destRect, UINT edge, UINT flags, LPRECT contentRect) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, %d, %d, (%ws), 0x%08X, 0x%08X", theme, dc, partId, stateId, hive.safe_string(destRect), edge, flags);

			return hive.orig.DrawThemeEdge(theme, dc, partId, stateId, destRect, edge, flags, contentRect);
		}
	} spinRenderer;
}
