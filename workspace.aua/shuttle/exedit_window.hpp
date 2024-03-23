#pragma once

namespace apn::workspace
{
	//
	// このクラスは拡張編集ウィンドウ用のシャトルです。
	//
	struct ExEditWindow : Shuttle
	{
		my::Window dummy;

		//
		// ターゲットにアタッチします。
		//
		void init(const std::wstring& name, HWND hwnd)
		{
			MY_TRACE_FUNC("{}, {:#010x}", name, hwnd);

			__super::init(name, hwnd);

			// フローティングコンテナのアイコンを設定します。
			auto icon = (HICON)::GetClassLong(*this, GCL_HICON);
			::SendMessage(*float_container, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			::SendMessage(*float_container, WM_SETICON, ICON_BIG, (LPARAM)icon);

			{
				// 「VoiceroidUtil」用の処理です。ダミーの拡張編集ウィンドウを作成します。
				// ::FindWindow()で見つけられるように、オーバーラップウィンドウとして作成します。

				WNDCLASS wc = {};
				wc.style = 0;
				wc.hCursor = 0;
				wc.lpfnWndProc = ::DefWindowProc;
				wc.hInstance = ::GetModuleHandle(_T("exedit.auf"));
				wc.lpszClassName = _T("AviUtl");
				ATOM atom = ::RegisterClass(&wc);

				dummy.create(
					WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
					_T("AviUtl"),
					_T("拡張編集"),
					WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
					0, 0, 0, 0,
					hive.main_window, 0, wc.hInstance, 0);
			}
		}

		virtual DWORD on_get_target_new_style() override
		{
			// ※WS_CAPTIONを外すとウィンドウの高さ調節にズレが生じるので外さないようにします。
			DWORD style = my::get_style(*this);
			style &= ~(WS_POPUP | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
			return style;
		}

		//
		// ターゲットウィンドウのサイズを修正します。
		//
		virtual BOOL revise_content_position(LPRECT rc) override
		{
			rc->bottom -= magi.exin.get_layer_height() / 2;
			::SendMessage(*this, WM_SIZING, WMSZ_BOTTOM, (LPARAM)rc);

			return TRUE;
		}

		//
		// ターゲットのウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// このタイミングでサブクラス化を解除して、後始末処理を省略します。
					unsubclass();

					break;
				}
			case WM_SHOWWINDOW:
				{
					// ダミーウィンドウの表示状態を更新します。
					::ShowWindow(dummy, wParam ? SW_SHOW : SW_HIDE);

					break;
				}
			case WM_SETTEXT:
				{
					// ダミーウィンドウのテキストを更新します。
					::SetWindowText(dummy, (LPCTSTR)lParam);

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			case WM_KILLFOCUS:
				{
					MY_TRACE_FUNC("WM_KILLFOCUS, {:#010x}, {:#010x}", wParam, lParam);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	}; inline auto exedit_window = std::make_shared<ExEditWindow>();
}
