#pragma once
#include "Skin.h"
#include "Gdi/Renderer.h"

namespace fgo::dark::gdi
{
	inline struct DialogRenderer : Renderer
	{
		int onFillRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X", dc, hive.safe_string(rc), brush);

			HINSTANCE instance = (HINSTANCE)::GetWindowLongPtr(currentState->hwnd, GWLP_HINSTANCE);
			DWORD style = ::GetWindowLong(currentState->hwnd, GWL_STYLE);
			if (instance == ::GetModuleHandle(_T("comdlg32.dll")) && style & WS_THICKFRAME)
			{
				// コンボボックス＆コンボリストボックス用。
				// WS_THICKFRAMEがない場合は色の設定コモンダイアログなので何もしない。

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_DIALOG);

				COLORREF color = painter::getBrushColor(brush);
//				MY_TRACE_HEX(color);
				if (color == ::GetSysColor(COLOR_WINDOW))
				{
					if (skin::theme::manager.onDrawThemeBackground(theme, dc, WP_DIALOG, ETS_NORMAL, rc))
						return TRUE;
				}
				else
				{
					if (skin::theme::manager.onDrawThemeBackground(theme, dc, WP_DIALOG, ETS_SELECTED, rc))
						return TRUE;
				}
			}

			return hive.orig.FillRect(dc, rc, brush);
		}

		BOOL onDrawFrame(State* currentState, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), %d, 0x%08X", dc, hive.safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		BOOL onDrawFrameControl(State* currentState, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X, 0x%08X", dc, hive.safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		BOOL onFrameRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X", dc, hive.safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		BOOL onDrawEdge(State* currentState, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X, 0x%08X", dc, hive.safe_string(rc), edge, flags);

			if (edge == EDGE_SUNKEN)
			{
				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_WINDOW);
				if (skin::theme::manager.onDrawThemeBackground(theme, dc, skin::theme::WINDOW_SUNKENEDGE, 0, rc))
				{
					if (flags & BF_ADJUST)
						::InflateRect(rc, -2, -2);

					return TRUE;
				}
			}

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		BOOL onDrawFocusRect(State* currentState, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, (%ws)", dc, hive.safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		BOOL onDrawStateW(State* currentState, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("0x%08X, 0x%08X, %d, %d, %d, %d, 0x%08X", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		BOOL onExtTextOutW(State* currentState, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("%d, %d, 0x%08X, 0x%08X, 0x%08X, %d, 0x%08X, 0x%08X, 0x%08X", x, y, options, rc, text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			if (options == ETO_OPAQUE)
			{
				HINSTANCE instance = (HINSTANCE)::GetWindowLongPtr(currentState->hwnd, GWLP_HINSTANCE);
				if (instance == ::GetModuleHandle(_T("comdlg32.dll")))
				{
					// コンボボックス＆コンボリストボックス用。

					HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_DIALOG);

					COLORREF bkColor = ::GetBkColor(dc);
//					MY_TRACE_HEX(bkColor);
					if (bkColor == RGB(0xff, 0xff, 0xff))
					{
						if (skin::theme::manager.onExtTextOut(theme, dc, WP_DIALOG, ETS_NORMAL, x, y, options, rc, text, c, dx))
							return TRUE;
					}
					else
					{
						if (skin::theme::manager.onExtTextOut(theme, dc, WP_DIALOG, ETS_SELECTED, x, y, options, rc, text, c, dx))
							return TRUE;
					}
				}
			}

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		BOOL onPatBlt(State* currentState, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	} dialogRenderer;
}
