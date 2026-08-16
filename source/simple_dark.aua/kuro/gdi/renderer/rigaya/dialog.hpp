#pragma once

namespace apn::dark::kuro::gdi::rigaya
{
	struct dialog_renderer_t : gdi::dialog_renderer_t
	{
		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, w_param, l_param);

			switch (message)
			{
			case WM_SHOWWINDOW:
				{
					MY_TRACE_FUNC("{/hex}, WM_SHOWWINDOW, {/hex}, {/hex}", hwnd, w_param, l_param);

					if (w_param) ::SetActiveWindow(hwnd);

					break;
				}
			}

			return __super::on_subclass_proc(hwnd, message, w_param, l_param);
		}
	};
}
