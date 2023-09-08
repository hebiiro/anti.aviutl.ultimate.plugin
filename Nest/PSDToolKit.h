#pragma once
#include "SubProcess.h"

namespace fgo::nest
{
	//
	// このクラスはPSDToolKit用のシャトルです。
	//
	struct PSDToolKit : SubProcess
	{
		struct Undock
		{
			HWND dialog;
			HWND hwnd;
			HWND parent;
			HWND owner;
			DWORD style;
			DWORD exStyle;

			//
			// PSDToolKitウィンドウのドッキングを解除します。
			//
			void init(PSDToolKit* shuttle, HWND dialog)
			{
				MY_TRACE(_T("PSDToolKit::Undock::init(0x%08X, 0x%08X)\n"), (HWND)*shuttle, dialog);

				this->dialog = dialog;

				// 「口パク」「目パチ」ダイアログだけに限定します。
				if (!::FindWindowEx(dialog, 0, WC_STATIC, _T("開き"))) return;

				// ターゲットウィンドウのハンドルを取得します。
				hwnd = shuttle->target;
				MY_TRACE_HWND(hwnd);
				if (!hwnd) return;

				// ターゲットウィンドウのクライアント矩形を取得します。
				RECT rc; ::GetClientRect(hwnd, &rc);

				// クライアント矩形をスクリーン座標に変換します。
				::MapWindowPoints(hwnd, 0, (LPPOINT)&rc, 2);

				// ウィンドウの元の状態を取得しておきます。
				parent = ::GetParent(hwnd);
				owner = (HWND)::GetWindowLong(hwnd, GWL_HWNDPARENT);
				style = ::GetWindowLong(hwnd, GWL_STYLE);
				exStyle = ::GetWindowLong(hwnd, GWL_EXSTYLE);

				// フローティング状態になるようにウィンドウの状態を変更します。
				::ShowWindow(hwnd, SW_HIDE);
				::SetParent(hwnd, 0);
//				::SetWindowLong(hwnd, GWL_HWNDPARENT, 0); // この処理は必要ないようです。
				::SetWindowLong(hwnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU |
					WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPSIBLINGS);
				hive.true_SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_FRAMECHANGED);

				// クライアント矩形をウィンドウ矩形に変換します。
				clientToWindow(hwnd, &rc);

				// ウィンドウの位置を調整します。
				int x = rc.left;
				int y = rc.top;
				int w = getWidth(rc);
				int h = getHeight(rc);
				hive.true_SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW);

				// PSDToolKitウィンドウの後ろに隠れてしまうので、
				// ダイアログも最上位ウィンドウにします。
				::SetWindowLong(dialog, GWL_HWNDPARENT, (LONG)hwnd);
//				hive.true_SetWindowPos(dialog, HWND_TOPMOST, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
			}

			//
			// PSDToolKitウィンドウを元の状態に戻します。
			//
			void exit()
			{
				MY_TRACE(_T("PSDToolKit::Undock::exit()\n"));

				if (!hwnd) return;

				::ShowWindow(hwnd, SW_HIDE);
				::SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
				::SetWindowLong(hwnd, GWL_STYLE, style);
//				::SetWindowLong(hwnd, GWL_HWNDPARENT, (LONG)owner);
				::SetParent(hwnd, parent);
				::SendMessage(parent, WM_SIZE, 0, 0);

				hwnd = 0;
			}
		} undock = {};

		HWND findWindow() override
		{
			return ::FindWindow(_T("NuklearWindowClass"), 0);
		}
	}; inline std::shared_ptr<PSDToolKit> psdtoolkit;
}
