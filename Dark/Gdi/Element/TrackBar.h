#pragma once
#include "Skin.h"
#include "Gdi/Renderer.h"

namespace fgo::dark::gdi
{
	inline struct TrackBarRenderer : Renderer
	{
		LRESULT onSubclassProc(Reflector* reflector, State* currentState) override
		{
			switch (currentState->message)
			{
			case WM_ERASEBKGND:
				{
					// チラツキを防止するために
					// デフォルトの塗りつぶし処理をスキップします。
					return TRUE;
				}
			case WM_LBUTTONDOWN:
				{
					LRESULT result = __super::onSubclassProc(reflector, currentState);

					// コモンコントロールをバージョン6にすると、
					// WM_LBUTTONDOWNでSB_THUMBTRACKが送られてこなくなります。
					// なので、SB_THUMBTRACKを手動で送ります。
					HWND hwnd = currentState->hwnd;
					int pos = (int)::SendMessage(hwnd, TBM_GETPOS, 0, 0);
					::SendMessage(::GetParent(hwnd), WM_HSCROLL, MAKEWPARAM(SB_THUMBTRACK, pos), (LPARAM)hwnd);

					return result;
				}
			}

			return __super::onSubclassProc(reflector, currentState);
		}

		int onFillRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("0x%08X, (%ws), 0x%08X", dc, hive.safe_string(rc), brush);

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
//			MY_TRACE_FUNC("%d, %d, 0x%08X, 0x%08X, 0x%08X, %d, 0x%08X, 0x%08X, 0x%08X", x, y, options, rc, text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			{
				COLORREF bkColor = ::GetBkColor(dc);
				if (bkColor == ::GetSysColor(COLOR_HIGHLIGHT))
				{
					HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_TRACKBAR);
					auto state = skin::theme::manager.getState(theme, TKP_TRACK, 0);
					if (state && state->fillColor != CLR_NONE)
						::SetBkColor(dc, state->fillColor);
				}
			}

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		BOOL onPatBlt(State* currentState, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("0x%08X, %d, %d, %d, %d, 0x%08X", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	} trackbarRenderer;
}
