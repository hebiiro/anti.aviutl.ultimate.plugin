#pragma once
#include "DoubleBuffer.h"
#include "Skin.h"
#include "Gdi/Renderer.h"

namespace fgo::dark::gdi
{
	inline struct TabRenderer : Renderer
	{
		LRESULT onSubclassProc(Reflector* reflector, State* currentState) override
		{
			HWND hwnd = currentState->hwnd;
			UINT message = currentState->message;
			WPARAM wParam = currentState->wParam;
			LPARAM lParam = currentState->lParam;

//			MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X, 0x%08X", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_NCCREATE:
				{
					MY_TRACE_FUNC("WM_NCCREATE, 0x%08X, 0x%08X, 0x%08X", hwnd, wParam, lParam);

					// ダブルバッファリングを有効にします。
					double_buffer.create(hwnd);

					break;
				}
			case WM_NCDESTROY:
				{
					MY_TRACE_FUNC("WM_NCDESTROY, 0x%08X, 0x%08X, 0x%08X", hwnd, wParam, lParam);

					// ダブルバッファリングを無効にします。
					double_buffer.destroy(hwnd);

					break;
				}
			}

			return __super::onSubclassProc(reflector, currentState);
		}

		int onFillRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
//			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X", dc, hive.safe_string(rc), brush);

			{
				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_TAB);
				if (skin::theme::manager.onDrawThemeBackground(theme, dc, 0, 0, rc))
					return TRUE;
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

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		BOOL onPatBlt(State* currentState, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	} tabRenderer;
}
