#pragma once

namespace apn::dialog_position
{
	//
	// このクラスはダイアログをフックします。
	//
	struct Dialog : my::Window
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
					MY_TRACE_FUNC("WM_INITDIALOG, {:#010x}, {:#010x}", wParam, lParam);

					// デフォルト処理を先に実行します。
					auto result = __super::on_wnd_proc(hwnd, message, wParam, lParam);

					// モニタ矩形を取得します。
					auto monitor = my::get_monitor_rect(hwnd);

					// ウィンドウ矩形を取得します。
					auto rc = my::get_window_rect(hwnd);

					// ウィンドウ矩形がモニタの範囲外ならオフセットを算出します。
					auto offset = POINT {};
					if (rc.right > monitor.right) offset.x = monitor.right - rc.right;
					if (rc.bottom > monitor.bottom) offset.y = monitor.bottom - rc.bottom;
					if (rc.left < monitor.left) offset.x = monitor.left - rc.left;
					if (rc.top < monitor.top) offset.y = monitor.top - rc.top;

					if (offset.x || offset.y)
					{
						// モニタ内に収まるようにウィンドウ矩形を調整します。

						::OffsetRect(&rc, offset.x, offset.y);

						auto x = rc.left;
						auto y = rc.top;
						auto w = my::get_width(rc);
						auto h = my::get_height(rc);

						::SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER);
					}

					return result;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
