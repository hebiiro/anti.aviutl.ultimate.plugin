#pragma once
#include "Gdi/Manager.h"

namespace fgo::dark::hook
{
	//
	// このクラスは::FillRect()をフックします。
	//
	inline struct
	{
		inline static int WINAPI hook(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			DWORD from = *((DWORD*)&dc - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, (%ws), 0x%08X", from, dc, hive.safe_string(rc), brush);
			return gdi::manager.onFillRect(dc, rc, brush);
		}
		inline static decltype(&hook) orig = ::FillRect;
	} FillRect;

	//
	// このクラスは::DrawFrame()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, LPRECT rc, UINT width, UINT type)
		{
			DWORD from = *((DWORD*)&dc - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, (%ws), %d, 0x%08X", from, dc, hive.safe_string(rc), width, type);
			return gdi::manager.onDrawFrame(dc, rc, width, type);
		}
		inline static decltype(&hook) orig = 0;
	} DrawFrame;

	//
	// このクラスは::DrawFrameControl()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, LPRECT rc, UINT type, UINT state)
		{
			DWORD from = *((DWORD*)&dc - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, (%ws), 0x%08X, 0x%08X", from, dc, hive.safe_string(rc), type, state);
			return gdi::manager.onDrawFrameControl(dc, rc, type, state);
		}
		inline static decltype(&hook) orig = ::DrawFrameControl;
	} DrawFrameControl;

	//
	// このクラスは::FrameRect()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			DWORD from = *((DWORD*)&dc - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, (%ws), 0x%08X", from, dc, hive.safe_string(rc), brush);
			return gdi::manager.onFrameRect(dc, rc, brush);
		}
		inline static decltype(&hook) orig = ::FrameRect;
	} FrameRect;

	//
	// このクラスは::DrawEdge()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, LPRECT rc, UINT edge, UINT flags)
		{
			DWORD from = *((DWORD*)&dc - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, (%ws), 0x%08X, 0x%08X", from, dc, hive.safe_string(rc), edge, flags);
			return gdi::manager.onDrawEdge(dc, rc, edge, flags);
		}
		inline static decltype(&hook) orig = ::DrawEdge;
	} DrawEdge;

	//
	// このクラスは::DrawFocusRect()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, LPCRECT rc)
		{
			DWORD from = *((DWORD*)&dc - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, (%ws)", from, dc, hive.safe_string(rc));
			return gdi::manager.onDrawFocusRect(dc, rc);
		}
		inline static decltype(&hook) orig = ::DrawFocusRect;
	} DrawFocusRect;

	//
	// このクラスは::DrawStateW()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
		{
			DWORD from = *((DWORD*)&dc - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, 0x%08X, %d, %d, %d, %d, 0x%08X", from, dc, fore, x, y, cx, cy, flags);
			return gdi::manager.onDrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}
		inline static decltype(&hook) orig = ::DrawStateW;
	} DrawStateW;

	//
	// このクラスは::ExtTextOutW()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			DWORD from = *((DWORD*)&dc - 1);

			if (hive.lockExtTextOut)
			{
//				MY_TRACE(_T("再帰呼び出し時はフックしません\n"));

				return orig(dc, x, y, options, rc, text, c, dx);
			}

			if (options & ETO_GLYPH_INDEX)
				MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, 0x%08X, (%ws), ETO_GLYPH_INDEX", from, dc, x, y, options, hive.safe_string(rc));
			else
				MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, 0x%08X, (%ws), %ws", from, dc, x, y, options, hive.safe_string(rc), hive.safe_string(text, c));

			Hive::ExtTextOutLocker locker;

			return gdi::manager.onExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}
		inline static decltype(&hook) orig = ::ExtTextOutW;
	} ExtTextOutW;

	//
	// このクラスは::PatBlt()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, int x, int y, int w, int h, DWORD rop)
		{
			DWORD from = *((DWORD*)&dc - 1);
			MY_TRACE_EX("0x%08X, 0x%08X, %d, %d, %d, %d, 0x%08X", from, dc, x, y, w, h, rop);
			return gdi::manager.onPatBlt(dc, x, y, w, h, rop);
		}
		inline static decltype(&hook) orig = ::PatBlt;
	} PatBlt;
}
