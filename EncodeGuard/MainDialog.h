#pragma once
#include "Resource.h"
#include "Hive.h"

namespace fgo::encode_guard
{
	//
	// このクラスはメインダイアログです。
	//
	struct MainDialog : Tools::Dialog
	{
		//
		// ウィンドウを作成します。
		//
		BOOL create(HWND parent)
		{
			return __super::create(hive.instance, MAKEINTRESOURCE(IDD_MAIN_DIALOG), parent);
		}

		//
		// コントロールの位置を更新します。
		//
		void onSize()
		{
			MY_TRACE_FUNC("");

			int base = getBaseSize();
			MY_TRACE_INT(base);

			HWND checkbox[] = {
				::GetDlgItem(*this, IDC_CHECK_RANGE),
				::GetDlgItem(*this, IDC_CHECK_LAST_FRAME),
				::GetDlgItem(*this, IDC_CHECK_FRAME_RATE),
			};

			RECT rc; ::GetClientRect(*this, &rc);
			int cx = rc.left;
			int cy = rc.top;
			int cw = getWidth(rc);
			int ch = getHeight(rc);

			HDWP dwp = ::BeginDeferWindowPos(3);
			::DeferWindowPos(dwp, checkbox[0], 0, cx, cy + base * 0, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
			::DeferWindowPos(dwp, checkbox[1], 0, cx, cy + base * 1, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
			::DeferWindowPos(dwp, checkbox[2], 0, cx, cy + base * 2, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
			::EndDeferWindowPos(dwp);
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// ダイアログプロシージャです。
		//
		INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					setCheck(IDC_CHECK_RANGE, hive.checkRange);
					setCheck(IDC_CHECK_LAST_FRAME, hive.checkLastFrame);
					setCheck(IDC_CHECK_FRAME_RATE, hive.checkFrameRate);

					break;
				}
			case WM_SIZE:
				{
					onSize();

					break;
				}
			case WM_COMMAND:
				{
					UINT code = HIWORD(wParam);
					UINT id = LOWORD(wParam);
					HWND control = (HWND)lParam;

					switch (id)
					{
					case IDOK:
					case IDCANCEL: return 0;
					case IDC_CHECK_RANGE: hive.checkRange = getCheck(IDC_CHECK_RANGE); break;
					case IDC_CHECK_LAST_FRAME: hive.checkLastFrame = getCheck(IDC_CHECK_LAST_FRAME); break;
					case IDC_CHECK_FRAME_RATE: hive.checkFrameRate = getCheck(IDC_CHECK_FRAME_RATE); break;
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
