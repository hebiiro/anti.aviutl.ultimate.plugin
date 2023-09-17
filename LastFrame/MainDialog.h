#pragma once
#include "Resource.h"
#include "Hive.h"

namespace fgo::last_frame
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
		// コントロールサイズの基準となる数値を返します。
		// 内部的に使用されます。
		//
		int getBase()
		{
			// フォントを取得します。
			HFONT font = (HFONT)::SendMessage(*this, WM_GETFONT, 0, 0);
			MY_TRACE_HEX(font);
			if (!font) font = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

			// フォントのメトリックを取得します。
			ClientDC dc(*this);
			GdiObjSelector selector(dc, font);
			TEXTMETRIC tm = {}; ::GetTextMetrics(dc, &tm);

			// フォントの高さを返します。
			// コントロールのエッジ幅(2 * 2) + 余白(2 * 2)
			return tm.tmHeight + tm.tmInternalLeading + 8;
		}

		//
		// コントロールの位置を更新します。
		//
		void onSize()
		{
			MY_TRACE(_T("MainDialog::onSize()\n"));

			int base = getBase();
			if (base <= 0) base = 24;
			MY_TRACE_INT(base);

			HWND checkbox[] = {
				::GetDlgItem(*this, IDC_ENABLE),
			};

			RECT rc; ::GetClientRect(*this, &rc);
			int cx = rc.left;
			int cy = rc.top;
			int cw = getWidth(rc);
			int ch = getHeight(rc);

			HDWP dwp = ::BeginDeferWindowPos(1);
			::DeferWindowPos(dwp, checkbox[0], 0, cx, cy, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
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
					setCheck(IDC_ENABLE, hive.enable);

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
					case IDC_ENABLE:
						{
							hive.enable = getCheck(IDC_ENABLE);

							break;
						}
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
