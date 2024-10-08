#pragma once

namespace apn::dark::gdi
{
	//
	// このクラスはGDIレンダリングを管理します。
	//
	inline struct Manager
	{
		MessageState current_state = {};

		MessageState get_current_state()
		{
			return current_state;
		}

		void set_current_state(const MessageState& state)
		{
			current_state = state;
		}

		void set_current_state(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			current_state.hwnd = hwnd;
			current_state.message = message;
			current_state.wParam = wParam;
			current_state.lParam = lParam;
		}

		BOOL on_rectangle(HDC dc, int left, int top, int right, int bottom)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_rectangle(&current_state, dc, left, top, right, bottom);
			return hive.orig.Rectangle(dc, left, top, right, bottom);
		}

		BOOL on_fill_rect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_fill_rect(&current_state, dc, rc, brush);
			return hive.orig.FillRect(dc, rc, brush);
		}

		BOOL on_draw_frame(HDC dc, LPRECT rc, UINT width, UINT type)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_draw_frame(&current_state, dc, rc, width, type);
			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		BOOL on_draw_frame_control(HDC dc, LPRECT rc, UINT type, UINT state)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_draw_frame_control(&current_state, dc, rc, type, state);
			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		BOOL on_frame_rect(HDC dc, LPCRECT rc, HBRUSH brush)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_frame_rect(&current_state, dc, rc, brush);
			return hive.orig.FrameRect(dc, rc, brush);
		}

		BOOL on_draw_edge(HDC dc, LPRECT rc, UINT edge, UINT flags)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_draw_edge(&current_state, dc, rc, edge, flags);
			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		BOOL on_draw_focus_rect(HDC dc, LPCRECT rc)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_draw_focus_rect(&current_state, dc, rc);
			return hive.orig.DrawFocusRect(dc, rc);
		}

		BOOL on_draw_state_w(HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_draw_state_w(&current_state, dc, fore, cb, lData, wData, x, y, cx, cy, flags);
			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		BOOL on_ext_text_out_w(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			MY_TRACE_HWND(current_state.hwnd);

			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_ext_text_out_w(&current_state, dc, x, y, options, rc, text, c, dx);
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		BOOL on_pat_blt(HDC dc, int x, int y, int w, int h, DWORD rop)
		{
			auto renderer = Renderer::from_handle(current_state.hwnd);
			if (renderer) return renderer->on_pat_blt(&current_state, dc, x, y, w, h, rop);
			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	} thread_local manager;
}
