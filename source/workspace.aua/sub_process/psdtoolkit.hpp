#pragma once

namespace apn::workspace
{
	//
	// このクラスはPSDToolKit用のシャトルです。
	//
	struct PSDToolKit : SubProcess
	{
		//
		// このクラスはPSDToolKitウィンドウのドッキングを一時的に解除します。
		//
		struct
		{
			HWND dialog;
			HWND hwnd;
			HWND parent;
			HWND owner;
			DWORD style;
			DWORD ex_style;

			//
			// PSDToolKitウィンドウのドッキングを解除します。
			//
			void init(PSDToolKit* shuttle, HWND dialog)
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}", (HWND)*shuttle, dialog);

				this->dialog = dialog;

				// 「口パク」「目パチ」ダイアログだけに限定します。
				if (!::FindWindowEx(dialog, nullptr, WC_STATIC, _T("開き"))) return;

				// サブプロセスウィンドウのハンドルを取得します。
				hwnd = shuttle->window;
				MY_TRACE_HWND(hwnd);
				if (!hwnd) return;

				// サブプロセスウィンドウのクライアント矩形を取得します。
				auto rc = my::get_client_rect(hwnd);

				// クライアント矩形をスクリーン座標に変換します。
				my::map_window_points(hwnd, nullptr, &rc);

				// サブプロセスウィンドウの元の状態を取得しておきます。
				parent = ::GetParent(hwnd);
				owner = (HWND)::GetWindowLong(hwnd, GWL_HWNDPARENT);
				style = ::GetWindowLong(hwnd, GWL_STYLE);
				ex_style = ::GetWindowLong(hwnd, GWL_EXSTYLE);

				// フローティング状態になるようにサブプロセスウィンドウの状態を変更します。
				::ShowWindow(hwnd, SW_HIDE);
				::SetParent(hwnd, nullptr);
//				::SetWindowLong(hwnd, GWL_HWNDPARENT, 0); // この処理は必要ないようです。
				::SetWindowLong(hwnd, GWL_STYLE, WS_CAPTION | WS_SYSMENU |
					WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPSIBLINGS);
				::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_FRAMECHANGED);

				// クライアント矩形をウィンドウ矩形に変換します。
				my::client_to_window(hwnd, &rc);

				// サブプロセスウィンドウの位置を調整します。
				auto x = rc.left;
				auto y = rc.top;
				auto w = my::get_width(rc);
				auto h = my::get_height(rc);
				::SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW);

				// サブプロセスウィンドウの後ろに行かないように、
				// ダイアログの親ウィンドウをサブプロセスウィンドウに変更します。
				::SetWindowLong(dialog, GWL_HWNDPARENT, (LONG)hwnd);
			}

			//
			// PSDToolKitウィンドウを元の状態に戻します。
			//
			void exit()
			{
				MY_TRACE_FUNC("");

				if (!hwnd) return;

				::ShowWindow(hwnd, SW_HIDE);
				::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
				::SetWindowLong(hwnd, GWL_STYLE, style);
//				::SetWindowLong(hwnd, GWL_HWNDPARENT, (LONG)owner);
				::SetParent(hwnd, parent);
				::SendMessage(parent, WM_SIZE, 0, 0);

				hwnd = nullptr;
			}
		} undock = {};

		//
		// PSDToolKitウィンドウを見つけます。
		//
		virtual BOOL on_find_window() override
		{
			MY_TRACE_FUNC("");

			window = ::FindWindow(_T("NuklearWindowClass"), nullptr);

			return window != nullptr;
		}
	}; inline auto psdtoolkit = std::make_shared<PSDToolKit>();
}
