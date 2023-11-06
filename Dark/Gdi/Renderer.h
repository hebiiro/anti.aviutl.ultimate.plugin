#pragma once
#include "Hive.h"

namespace fgo::dark::gdi
{
	struct State
	{
		HWND hwnd = 0;
		UINT message = 0;
		WPARAM wParam = 0;
		LPARAM lParam = 0;
	};

	struct Reflector
	{
		virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) = 0;
	};

	struct Renderer
	{
		inline static constexpr LPCTSTR PropName = _T("fgo::dark::gdi::Renderer");

		inline static void attach(HWND hwnd, Renderer* renderer)
		{
			::SetProp(hwnd, PropName, renderer);
		}

		inline static void detach(HWND hwnd)
		{
			::RemoveProp(hwnd, PropName);
		}

		inline static Renderer* fromHandle(HWND hwnd)
		{
			// レンダラの使用が抑制されている場合は0を返します。
			if (hive.lockRenderer) return 0;

			return (Renderer*)::GetProp(hwnd, PropName);
		}

		virtual LRESULT onSubclassProc(Reflector* reflector, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return reflector->onWndProc(hwnd, message, wParam, lParam);
		}

		virtual LRESULT onCustomDraw(Reflector* reflector, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			return reflector->onWndProc(hwnd, message, wParam, lParam);
		}

		virtual int onFillRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush)
		{
			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL onDrawFrame(State* currentState, HDC dc, LPRECT rc, UINT width, UINT type)
		{
			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL onDrawFrameControl(State* currentState, HDC dc, LPRECT rc, UINT type, UINT state)
		{
			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL onFrameRect(State* currentState, HDC dc, LPCRECT rc, HBRUSH brush)
		{
			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL onDrawEdge(State* currentState, HDC dc, LPRECT rc, UINT edge, UINT flags)
		{
			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL onDrawFocusRect(State* currentState, HDC dc, LPCRECT rc)
		{
			return hive.orig.DrawFocusRect(dc, rc);
		}

		virtual BOOL onDrawStateW(State* currentState, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
		{
			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL onExtTextOutW(State* currentState, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL onPatBlt(State* currentState, HDC dc, int x, int y, int w, int h, DWORD rop)
		{
			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
