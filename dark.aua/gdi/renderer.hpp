#pragma once

namespace apn::dark::gdi
{
	//
	// このクラスはメッセージの状態です。
	//
	struct MessageState
	{
		WNDPROC wnd_proc = 0;
		HWND hwnd = 0;
		UINT message = 0;
		WPARAM wParam = 0;
		LPARAM lParam = 0;
	};

	struct Renderer
	{
		//
		// レンダラーのコレクションです。
		//
		thread_local inline static std::unordered_map<HWND, std::shared_ptr<Renderer>> collection;

		//
		// レンダラーとウィンドウを関連付けます。
		//
		inline static void attach(HWND hwnd, const std::shared_ptr<Renderer>& renderer)
		{
			collection[hwnd] = renderer;
		}

		//
		// レンダラーとウィンドウの関連付けを解除します。
		//
		inline static void detach(HWND hwnd)
		{
			collection.erase(hwnd);
		}

		//
		// 指定されたウィンドウに関連付けられているレンダラを返します。
		//
		inline static std::shared_ptr<Renderer> from_handle(HWND hwnd)
		{
			// レンダラの使用が抑制されている場合はnullptrを返します。
			if (hive.renderer_locked) return nullptr;

			auto it = collection.find(hwnd);
			if (it == collection.end()) return nullptr;
			return it->second;
		}

		inline static LRESULT CALLBACK orig_wnd_proc(MessageState* current_state)
		{
			return hive.orig.CallWindowProcInternal(current_state->wnd_proc, current_state->hwnd, current_state->message, current_state->wParam, current_state->lParam);
		}

		virtual LRESULT draw_nc_paint(HWND hwnd, HDC dc, const POINT& origin, LPRECT rc)
		{
			if (auto theme = skin::theme::manager.get_theme(VSCLASS_WINDOW))
			{
				auto style = my::get_style(hwnd);
				auto ex_style = my::get_ex_style(hwnd);

				if (ex_style & WS_EX_WINDOWEDGE)
				{
					auto part_id = WP_WINDOW_EDGE;
					auto state_id = 0;
					python.call_draw_figure(hwnd, theme, dc, part_id, state_id, rc);
					::InflateRect(rc, -2, -2);
				}
				else if (style & WS_BORDER)
				{
					auto part_id = WP_BORDER;
					auto state_id = 0;
					python.call_draw_figure(hwnd, theme, dc, part_id, state_id, rc);
					::InflateRect(rc, -1, -1);
				}

				if (ex_style & WS_EX_STATICEDGE)
				{
					auto part_id = WP_STATIC_EDGE;
					auto state_id = 0;
					python.call_draw_figure(hwnd, theme, dc, part_id, state_id, rc);
					::InflateRect(rc, -1, -1);
				}

				if (ex_style & WS_EX_CLIENTEDGE)
				{
					auto part_id = WP_CLIENT_EDGE;
					auto state_id = 0;
					python.call_draw_figure(hwnd, theme, dc, part_id, state_id, rc);
					::InflateRect(rc, -2, -2);
				}

				if (style & WS_HSCROLL && style & WS_VSCROLL)
				{
					auto corner_rc = *rc;
					corner_rc.top = corner_rc.bottom - ::GetSystemMetrics(SM_CYHSCROLL);
					corner_rc.left = corner_rc.right - ::GetSystemMetrics(SM_CXVSCROLL);

					auto part_id = WP_WINDOW_FACE;
					auto state_id = 0;
					python.call_draw_figure(hwnd, theme, dc, part_id, state_id, &corner_rc);
				}
			}

			return 0;
		}

		virtual LRESULT on_subclass_proc(MessageState* current_state)
		{
			return orig_wnd_proc(current_state);
		}

		virtual LRESULT on_custom_draw(MessageState* current_state)
		{
			return orig_wnd_proc(current_state);
		}

		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush)
		{
			return brush;
		}

		virtual BOOL on_rectangle(MessageState* current_state, HDC dc, int left, int top, int right, int bottom)
		{
			return hive.orig.Rectangle(dc, left, top, right, bottom);
		}

		virtual BOOL on_fill_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush)
		{
			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(MessageState* current_state, HDC dc, LPRECT rc, UINT width, UINT type)
		{
			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(MessageState* current_state, HDC dc, LPRECT rc, UINT type, UINT state)
		{
			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush)
		{
			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(MessageState* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags)
		{
			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(MessageState* current_state, HDC dc, LPCRECT rc)
		{
			return hive.orig.DrawFocusRect(dc, rc);
		}

		virtual BOOL on_draw_state_w(MessageState* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags)
		{
			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(MessageState* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx)
		{
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(MessageState* current_state, HDC dc, int x, int y, int w, int h, DWORD rop)
		{
			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
