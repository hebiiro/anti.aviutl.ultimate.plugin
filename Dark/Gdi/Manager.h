#pragma once
#include "Gdi/Renderer.h"

namespace fgo::dark::gdi
{
	//
	// このクラスはGDIレンダリングを管理します。
	//
	inline struct Manager
	{
		State currentState = {};

		State getCurrentState()
		{
			return currentState;
		}

		void setCurrentState(const State& state)
		{
			currentState = state;
		}

		void setCurrentState(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			currentState.hwnd = hwnd;
			currentState.message = message;
			currentState.wParam = wParam;
			currentState.lParam = lParam;
		}

		int onFillRect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onFillRect(&currentState, dc, rc, brush);
			return hive.orig.FillRect(dc, rc, brush);
		}

		BOOL onDrawFrame(HDC dc, LPRECT rc, UINT width, UINT type)
		{
			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onDrawFrame(&currentState, dc, rc, width, type);
			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		BOOL onDrawFrameControl(HDC dc, LPRECT rc, UINT type, UINT state)
		{
			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onDrawFrameControl(&currentState, dc, rc, type, state);
			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		BOOL onFrameRect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onFrameRect(&currentState, dc, rc, brush);
			return hive.orig.FrameRect(dc, rc, brush);
		}

		BOOL onDrawEdge(HDC dc, LPRECT rc, UINT edge, UINT flags)
		{
			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onDrawEdge(&currentState, dc, rc, edge, flags);
			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		BOOL onDrawFocusRect(HDC dc, LPCRECT rc)
		{
			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onDrawFocusRect(&currentState, dc, rc);
			return hive.orig.DrawFocusRect(dc, rc);
		}

		BOOL onDrawStateW(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
		{
			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onDrawStateW(&currentState, dc, fore, cb, lData, wData, x, y, cx, cy, flags);
			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		BOOL onExtTextOutW(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			MY_TRACE_HWND(currentState.hwnd);

			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onExtTextOutW(&currentState, dc, x, y, options, rc, text, c, dx);
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		BOOL onPatBlt(HDC dc, int x, int y, int w, int h, DWORD rop)
		{
			auto renderer = Renderer::fromHandle(currentState.hwnd);
			if (renderer) return renderer->onPatBlt(&currentState, dc, x, y, w, h, rop);
			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	} thread_local manager;
}
