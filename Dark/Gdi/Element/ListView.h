#pragma once
#include "Skin.h"
#include "Gdi/Renderer.h"

namespace fgo::dark::gdi
{
	inline struct ListViewRenderer : Renderer
	{
		int onFillRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X", dc, hive.safe_string(rc), brush);

			{
				// 背景色を描画します。

				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_LISTVIEW);
				COLORREF color = painter::getBrushColor(brush);

				if (color == ::GetSysColor(COLOR_HIGHLIGHT))
				{
					if (skin::theme::manager.onDrawThemeBackground(theme, dc, LVP_LISTITEM, LISS_HOT, rc))
						return TRUE;
				}
				else if (color == ::GetSysColor(COLOR_3DFACE))
				{
					if (skin::theme::manager.onDrawThemeBackground(theme, dc, LVP_LISTITEM, LISS_SELECTEDNOTFOCUS, rc))
						return TRUE;
				}
				else
				{
					if (skin::theme::manager.onDrawThemeBackground(theme, dc, LVP_LISTITEM, LISS_NORMAL, rc))
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
#if 1
//			if (!(options & ETO_IGNORELANGUAGE))
			{
				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_LISTVIEW);

				if (options == ETO_OPAQUE)
				{
					// セパレータのカラーを指定します。

					auto state = skin::theme::manager.getState(theme, LVP_LISTITEM, 0);

					if (state && state->fillColor != CLR_NONE)
					{
						::SetBkColor(dc, state->fillColor);

						return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
					}
				}

				int stateId = LISS_NORMAL;

				COLORREF bkColor = ::GetBkColor(dc);
				if (bkColor == ::GetSysColor(COLOR_HIGHLIGHT) || bkColor == CLR_NONE)
					stateId = LISS_HOT;

				if (skin::theme::manager.onExtTextOut(theme, dc, LVP_LISTITEM, stateId, x, y, options, rc, text, c, dx))
					return TRUE;
			}
#endif
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		BOOL onPatBlt(State* currentState, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	} listviewRenderer;
}
