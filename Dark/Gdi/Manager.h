#pragma once
#include "Gdi/Renderer.h"

namespace fgo::dark::gdi
{
	//
	// このクラスはGDIレンダリングを管理します。
	//
	inline struct Manager
	{
		State m_currentState = {};

		State getCurrentState()
		{
			return m_currentState;
		}

		void setCurrentState(const State& state)
		{
			m_currentState = state;
		}

		void setCurrentState(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			m_currentState.hwnd = hwnd;
			m_currentState.message = message;
			m_currentState.wParam = wParam;
			m_currentState.lParam = lParam;
		}

		int onFillRect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onFillRect(&m_currentState, dc, rc, brush);
			return hive.orig.FillRect(dc, rc, brush);
		}

		BOOL onDrawFrame(HDC dc, LPRECT rc, UINT width, UINT type)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onDrawFrame(&m_currentState, dc, rc, width, type);
			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		BOOL onDrawFrameControl(HDC dc, LPRECT rc, UINT type, UINT state)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onDrawFrameControl(&m_currentState, dc, rc, type, state);
			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		BOOL onFrameRect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onFrameRect(&m_currentState, dc, rc, brush);
			return hive.orig.FrameRect(dc, rc, brush);
		}

		BOOL onDrawEdge(HDC dc, LPRECT rc, UINT edge, UINT flags)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onDrawEdge(&m_currentState, dc, rc, edge, flags);
			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		BOOL onDrawFocusRect(HDC dc, LPCRECT rc)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onDrawFocusRect(&m_currentState, dc, rc);
			return hive.orig.DrawFocusRect(dc, rc);
		}

		BOOL onDrawStateW(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onDrawStateW(&m_currentState, dc, fore, cb, lData, wData, x, y, cx, cy, flags);
			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		BOOL onExtTextOutW(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onExtTextOutW(&m_currentState, dc, x, y, options, rc, text, c, dx);
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		BOOL onPatBlt(HDC dc, int x, int y, int w, int h, DWORD rop)
		{
			auto renderer = Renderer::fromHandle(m_currentState.hwnd);
			if (renderer) return renderer->onPatBlt(&m_currentState, dc, x, y, w, h, rop);
			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	} thread_local manager;
}
