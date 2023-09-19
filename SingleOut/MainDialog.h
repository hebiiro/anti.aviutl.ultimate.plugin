#pragma once
#include "Resource.h"
#include "Hive.h"
#include "Common/AviUtl/SliderPanel.h"

namespace fgo::single_out
{
	//
	// このクラスはメインダイアログです。
	//
	struct MainDialog : Tools::Dialog
	{
		Tools::AviUtl::SliderPanel<int, Tools::AviUtl::IntExchanger<int>> quality;

		//
		// コンストラクタです。
		//
		MainDialog()
			: quality(0, 100)
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

			HWND button[] = {
				::GetDlgItem(*this, IDC_EXPORT_FRAME_RGB),
				::GetDlgItem(*this, IDC_EXPORT_FRAME_RGBA),
				::GetDlgItem(*this, IDC_EXPORT_ITEM_RGB),
				::GetDlgItem(*this, IDC_EXPORT_ITEM_RGBA),
			};

			RECT rc; ::GetClientRect(*this, &rc);
			int cx = rc.left;
			int cy = rc.top;
			int cw = getWidth(rc);
			int ch = getHeight(rc);

			HDWP dwp = ::BeginDeferWindowPos(7);
			int y = cy;
			quality.move(dwp, cx, y, cw, base);
			y += base;
			for (size_t i = 0; i < std::size(button); i++)
				::DeferWindowPos(dwp, button[i], 0, cx, y + base * i, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
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

					quality.init(hwnd, IDC_QUALITY);
					quality.setValue(hive.quality);

					break;
				}
			case WM_SIZE:
				{
					onSize();

					break;
				}
			case WM_HSCROLL:
				{
					MY_TRACE_FUNC("WM_HSCROLL, 0x%08X, 0x%08X", wParam, lParam);

					HWND control = (HWND)lParam;

					if (control == quality.slider)
						hive.quality = quality.onChangePos();

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
					case IDC_EXPORT_FRAME_RGB: hive.commander->onExportImage(FALSE, FALSE); break;
					case IDC_EXPORT_FRAME_RGBA: hive.commander->onExportImage(TRUE, FALSE); break;
					case IDC_EXPORT_ITEM_RGB: hive.commander->onExportImage(FALSE, TRUE); break;
					case IDC_EXPORT_ITEM_RGBA: hive.commander->onExportImage(TRUE, TRUE); break;
					case IDC_QUALITY: if (code == EN_CHANGE) hive.quality = quality.onChangeText(); break;
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
