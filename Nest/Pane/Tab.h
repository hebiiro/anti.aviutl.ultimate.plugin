#pragma once

namespace fgo::nest
{
	struct Tab : Tools::Window
	{
		//
		// タブコントロールのフォントをセットします。
		//
		BOOL setFont()
		{
			AviUtl::SysInfo si = {};
			magi.auin.get_sys_info(0, &si);
			if (!si.hfont) return FALSE;

			::SendMessage(*this, WM_SETFONT, (WPARAM)si.hfont, TRUE);

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("0x%08X, 0x%08X, 0x%08X, 0x%08X", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_LBUTTONDBLCLK:
				{
					MY_TRACE_FUNC("WM_LBUTTONDBLCLK, 0x%08X, 0x%08X", wParam, lParam);

					// タブコントロールはNM_DBLCLKを通知しないので手動で通知します。

					HWND parent = ::GetParent(hwnd);
					NMHDR header = {};
					header.code = NM_DBLCLK;
					header.hwndFrom = hwnd;
					header.idFrom = ::GetWindowLongPtr(hwnd, GWLP_ID);
					::SendMessage(parent, WM_NOTIFY, header.idFrom, (LPARAM)&header);

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
