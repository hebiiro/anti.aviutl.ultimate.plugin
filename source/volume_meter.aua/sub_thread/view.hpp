#pragma once

namespace apn::volume_meter::sub_thread
{
	//
	// このクラスはサブスレッドのビューです。
	// メインスレッドのビューの子ウィンドウとして作成されます。
	//
	inline struct view_t : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("");

			WNDCLASSEX wc = { sizeof(wc) };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = _T("volume_meter.sub_thread.view");
			::RegisterClassEx(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				wc.lpszClassName,
				wc.lpszClassName,
				WS_VISIBLE | WS_CHILD |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				parent, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを破壊します。
			return destroy();
		}

		//
		// 音量を再描画します。
		//
		BOOL redraw()
		{
			MY_TRACE_FUNC("");

			// ウィンドウを再描画します。
//			return ::InvalidateRect(*this, nullptr, FALSE);
			return ::InvalidateRect(*this, nullptr, FALSE), ::UpdateWindow(*this);
		}

		//
		// WM_PAINTを処理します。
		//
		LRESULT on_paint(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param)
		{
			// ペインターに描画処理を任せて、0を返します。
			return painter_t(hwnd)(), 0;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// ウィンドウが作成されたことをメインスレッドのビューに通知します。
					::PostMessage(::GetParent(hwnd), hive.c_message.c_post_init, 0, (LPARAM)hwnd);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					break;
				}
			case WM_PAINT:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					return on_paint(hwnd, message, w_param, l_param);
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} view;
}
