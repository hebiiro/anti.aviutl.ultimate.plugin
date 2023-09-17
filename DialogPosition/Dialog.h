#pragma once

namespace fgo::dialog_position
{
	//
	// このクラスはフックされたダイアログです。
	//
	struct Dialog : Tools::Window
	{
		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, 0x%08X, 0x%08X", wParam, lParam);

					// デフォルト処理を先に実行します。
					LRESULT lr = __super::onWndProc(hwnd, message, wParam, lParam);

					// モニタ情報を取得します。
					HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
					MONITORINFO mi = { sizeof(mi) };
					::GetMonitorInfo(monitor, &mi);

					// ウィンドウ矩形を取得します。
					RECT rc; ::GetWindowRect(hwnd, &rc);

					// ウィンドウ矩形がモニタの範囲外ならオフセットを算出します。
					POINT offset = {};
					if (rc.right > mi.rcWork.right) offset.x = mi.rcWork.right - rc.right;
					if (rc.bottom > mi.rcWork.bottom) offset.y = mi.rcWork.bottom - rc.bottom;
					if (rc.left < mi.rcWork.left) offset.x = mi.rcWork.left - rc.left;
					if (rc.top < mi.rcWork.top) offset.y = mi.rcWork.top - rc.top;

					if (offset.x || offset.y)
					{
						// モニタ内に収まるようにウィンドウ矩形を調整します。

						::OffsetRect(&rc, offset.x, offset.y);

						int x = rc.left;
						int y = rc.top;
						int w = rc.right - rc.left;
						int h = rc.bottom - rc.top;

						::SetWindowPos(hwnd, 0, x, y, w, h, SWP_NOZORDER);
					}

					return lr;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
