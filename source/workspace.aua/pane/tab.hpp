﻿#pragma once

namespace apn::workspace
{
	struct Tab : my::Window
	{
		//
		// タブコントロールのフォントをセットします。
		//
		BOOL set_font()
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
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_LBUTTONDBLCLK:
				{
					MY_TRACE_FUNC("WM_LBUTTONDBLCLK, {:#010x}, {:#010x}", wParam, lParam);

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

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
