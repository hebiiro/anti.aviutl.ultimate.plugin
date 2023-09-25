#pragma once
#include "Resource.h"
#include "Familiar.h"

namespace fgo::item_align
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

			HWND frameLabel = ::GetDlgItem(*this, IDC_FRAME_LABEL);
			HWND frame = ::GetDlgItem(*this, IDC_FRAME);
			HWND buttons[] = {
				::GetDlgItem(*this, IDC_SHIFT),
				::GetDlgItem(*this, IDC_MOVE_TO_BEGIN),
				::GetDlgItem(*this, IDC_MOVE_TO_END),
				::GetDlgItem(*this, IDC_MOVE_BEGIN),
				::GetDlgItem(*this, IDC_MOVE_END),
				::GetDlgItem(*this, IDC_STRETCH_BEGIN),
				::GetDlgItem(*this, IDC_STRETCH_END),
				::GetDlgItem(*this, IDC_DELETE_MIDPT),
			};

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

				int left = cx, right = ::MulDiv(cw, 1, 4);
				::DeferWindowPos(dwp, frameLabel, 0, left, y, right - left, base, SWP_NOZORDER | SWP_NOACTIVATE);
				left = right, right = ::MulDiv(cw, 2, 4);
				::DeferWindowPos(dwp, frame, 0, left, y, right - left, base, SWP_NOZORDER | SWP_NOACTIVATE);
			}

			{
				// 2行目以降のコントロールを配置変更します。

				for (size_t i = 0; i < std::size(buttons); i++)
				{
					y += base;
					::DeferWindowPos(dwp, buttons[i], 0, cx, y, cw, base, SWP_NOZORDER | SWP_NOACTIVATE);
				}
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
					case IDC_FRAME: if (code == EN_CHANGE) hive.frame = getInt(id); break;
					case IDC_SHIFT: Familiar().shift(); break;
					case IDC_MOVE_TO_BEGIN: Familiar().moveToBegin(); break;
					case IDC_MOVE_TO_END: Familiar().moveToEnd(); break;
					case IDC_MOVE_BEGIN: Familiar().moveBegin(); break;
					case IDC_MOVE_END: Familiar().moveEnd(); break;
					case IDC_STRETCH_BEGIN: Familiar().stretchBegin(); break;
					case IDC_STRETCH_END: Familiar().stretchEnd(); break;
					case IDC_DELETE_MIDPT: Familiar().deleteMidPt(); break;
					}

					break;
				}
			}

			return __super::onDlgProc(hwnd, message, wParam, lParam);
		}
	};
}
