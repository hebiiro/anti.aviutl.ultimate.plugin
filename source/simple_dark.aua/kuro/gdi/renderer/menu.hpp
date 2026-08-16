#pragma once

namespace apn::dark::kuro::gdi
{
	struct menu_renderer_t : renderer_nc_t
	{
		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, w_param, l_param);

			switch (message)
			{
			case WM_SHOWWINDOW:
				{
					MY_TRACE("WM_SHOWWINDOW\n");

					break;
				}
			}

			return __super::on_subclass_proc(hwnd, message, w_param, l_param);
		}

		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, dc, control, brush);

			return get_dialog_brush(hwnd, message, dc, control, brush);
		}

		virtual BOOL on_rectangle(message_state_t* current_state, HDC dc, int left, int top, int right, int bottom) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}, {/}, {/}, {/})", dc, left, top, right, bottom);

			return hive.orig.Rectangle(dc, left, top, right, bottom);
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(message_state_t* current_state, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/}, {/hex}", dc, safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(message_state_t* current_state, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(message_state_t* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), edge, flags);

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(message_state_t* current_state, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, ({/})", dc, safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(message_state_t* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(message_state_t* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
