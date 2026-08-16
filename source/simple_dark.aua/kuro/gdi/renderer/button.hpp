#pragma once

namespace apn::dark::kuro::gdi
{
	struct button_renderer_t : renderer_nc_t
	{
		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, dc, control, brush);

			// ボタンの背景の色を変更します。
			// ボタンがダイアログに配置されることを前提に処理しています。
			return get_dialog_brush(hwnd, message, dc, control, brush);
		}
	};
}
