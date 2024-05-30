#pragma once

namespace apn::dark::gdi
{
	struct RendererNc : Renderer
	{
		virtual LRESULT on_subclass_proc(MessageState* current_state) override
		{
			switch (current_state->message)
			{
			case WM_NCPAINT:
				{
					MY_TRACE_FUNC("WM_NCPAINT, {:#010x}, {:#010x}, {:#010x}",
						current_state->hwnd, current_state->wParam, current_state->lParam);

					auto hwnd = current_state->hwnd;

					auto style = my::get_style(hwnd);
					if ((style & WS_CAPTION) == WS_CAPTION)
						return __super::on_subclass_proc(current_state);

					auto rc = my::get_window_rect(hwnd);
					auto origin = POINT { rc.left, rc.top };
					::OffsetRect(&rc, -origin.x, -origin.y);
					return draw_nc_paint(hwnd, origin, &rc);
				}
			}

			return __super::on_subclass_proc(current_state);
		}
	};
}
