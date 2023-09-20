#pragma once
#include "Resource.h"
#include "Familiar.h"

namespace fgo::text_split
{
	//
	// このクラスはメインダイアログです。
	//
	struct MainDialog : Tools::Dialog
	{
		//
		// コンストラクタです。
		//
		MainDialog()
		{
		}

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

			HWND splitText = ::GetDlgItem(*this, IDC_SPLIT_TEXT);
			HWND frameLabel = ::GetDlgItem(*this, IDC_FRAME_LABEL);
			HWND frame = ::GetDlgItem(*this, IDC_FRAME);
			HWND fitWidthLabel = ::GetDlgItem(*this, IDC_FIT_WIDTH_LABEL);
			HWND fitWidth = ::GetDlgItem(*this, IDC_FIT_WIDTH);
			HWND absolute = ::GetDlgItem(*this, IDC_ABSOLUTE);
			HWND row = ::GetDlgItem(*this, IDC_ROW);
			HWND fit = ::GetDlgItem(*this, IDC_FIT);
			HWND deleteOrig = ::GetDlgItem(*this, IDC_DELETE_ORIG);

			RECT rc; ::GetClientRect(*this, &rc);
			int cx = rc.left;
			int cy = rc.top;
			int cw = getWidth(rc);
			int ch = getHeight(rc);

			// コントロールの配置変更を開始します。
			HDWP dwp = ::BeginDeferWindowPos(9);

			int y = cy;

			{
				// 1行目のコントロールを配置変更します。

				::DeferWindowPos(dwp, splitText, 0, cx, y, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
			}

			{
				// 2行目のコントロールを配置変更します。

				y += base;
				int left = cx, right = ::MulDiv(cw, 1, 4);
				::DeferWindowPos(dwp, frameLabel, 0, left, y, right - left, base, SWP_NOZORDER | SWP_NOACTIVATE);
				left = right, right = ::MulDiv(cw, 2, 4);
				::DeferWindowPos(dwp, frame, 0, left, y, right - left, base, SWP_NOZORDER | SWP_NOACTIVATE);
				left = right, right = ::MulDiv(cw, 3, 4);
				::DeferWindowPos(dwp, fitWidthLabel, 0, left, y, right - left, base, SWP_NOZORDER | SWP_NOACTIVATE);
				left = right, right = ::MulDiv(cw, 4, 4);
				::DeferWindowPos(dwp, fitWidth, 0, left, y, right - left, base, SWP_NOZORDER | SWP_NOACTIVATE);
			}

			int left = cx, center = cx + cw / 2, right = cx + cw;

			{
				// 3行目のコントロールを配置変更します。

				y += base;
				::DeferWindowPos(dwp, absolute, 0, left, y, center - left, base, SWP_NOZORDER | SWP_NOACTIVATE);
				::DeferWindowPos(dwp, row, 0, center, y, right - center, base, SWP_NOZORDER | SWP_NOACTIVATE);
			}

			{
				// 3行目のコントロールを配置変更します。

				y += base;
				::DeferWindowPos(dwp, fit, 0, left, y, center - left, base, SWP_NOZORDER | SWP_NOACTIVATE);
				::DeferWindowPos(dwp, deleteOrig, 0, center, y, right - center, base, SWP_NOZORDER | SWP_NOACTIVATE);
			}

			// コントロールの配置変更を終了します。
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
					MY_TRACE_FUNC("WM_INITDIALOG, 0x%08X, 0x%08X", wParam, lParam);

					setInt(IDC_FRAME, hive.frame);
					setInt(IDC_FIT_WIDTH, hive.fitWidth);
					setCheck(IDC_ABSOLUTE, hive.absolute);
					setCheck(IDC_ROW, hive.row);
					setCheck(IDC_FIT, hive.fit);
					setCheck(IDC_DELETE_ORIG, hive.deleteOrig);

					break;
				}
			case WM_SIZE:
				{
					onSize();

					break;
				}
			case WM_COMMAND:
			case AviUtl::FilterPlugin::WindowMessage::Command:
				{
					MY_TRACE_FUNC("WM_COMMAND, 0x%08X, 0x%08X", wParam, lParam);

					UINT code = HIWORD(wParam);
					UINT id = LOWORD(wParam);
					HWND control = (HWND)lParam;

					switch (id)
					{
					case IDOK:
					case IDCANCEL: return 0;
					case IDC_SPLIT_TEXT: Familiar().work(); break;
					case IDC_FRAME: if (code == EN_CHANGE) hive.frame = getInt(id); break;
					case IDC_FIT_WIDTH: if (code == EN_CHANGE) hive.fitWidth = getInt(id); break;
					case IDC_ABSOLUTE: hive.absolute = getCheck(id); break;
					case IDC_ROW: hive.row = getCheck(id); break;
					case IDC_FIT: hive.fit = getCheck(id); break;
					case IDC_DELETE_ORIG: hive.deleteOrig = getCheck(id); break;
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
