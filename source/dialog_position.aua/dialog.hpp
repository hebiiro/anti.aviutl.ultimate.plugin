#pragma once

namespace apn::dialog_position
{
	//
	// このクラスはダイアログをフックします。
	//
	struct Dialog : my::WindowInterceptor
	{
		//
		// ウィンドウプロシージャです。
		//
		LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, {/hex}, {/hex}", wParam, lParam);

					// デフォルト処理を先に実行します。
					auto result = __super::on_wnd_proc(hwnd, message, wParam, lParam);

					// モニタ矩形を取得します。
					auto monitor = my::get_monitor_rect(hwnd);

					// ウィンドウ矩形を取得します。
					auto rc = my::get_window_rect(hwnd);

					// ウィンドウ矩形がモニタの範囲外ならオフセットを算出します。
					auto offset = POINT {};
					if (rc.left < monitor.left) offset.x = monitor.left - rc.left;
					if (rc.top < monitor.top) offset.y = monitor.top - rc.top;
					if (my::get_width(rc) <= my::get_width(monitor))
						if (rc.right > monitor.right) offset.x = monitor.right - rc.right;
					if (my::get_height(rc) <= my::get_height(monitor))
						if (rc.bottom > monitor.bottom) offset.y = monitor.bottom - rc.bottom;

					// オフセットが有効の場合は
					if (offset.x || offset.y)
					{
						// モニタ内に収まるようにウィンドウ矩形を調整します。
						::OffsetRect(&rc, offset.x, offset.y);
						my::set_window_pos(hwnd, nullptr, &rc, SWP_NOZORDER);
					}

					return result;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
