#pragma once

namespace apn::workspace
{
	//
	// このクラスは再生ウィンドウ用のシャトルです。
	//
	struct PlayerWindow : Shuttle
	{
		//
		// ターゲットのウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_WINDOWPOSCHANGING:
				{
					MY_TRACE_FUNC("{}, WM_WINDOWPOSCHANGING, {:#010x}, {:#010x}", name, wParam, lParam);

					// ウィンドウサイズを変更する場合は
					auto wp = (WINDOWPOS*)lParam;
					if (!(wp->flags & SWP_NOSIZE))
					{
						auto before_rc = my::get_window_rect(hwnd);
						auto result = __super::on_wnd_proc(hwnd, message, wParam, lParam);
						auto after_rc = my::get_window_rect(hwnd);

						// ウィンドウサイズが変更されていない場合は
						if (::EqualRect(&before_rc, &after_rc))
						{
							// 手動でリサイズ系ウィンドウメッセージを発行します。
							// ※この2つのメッセージを両方発行しないと
							// 再生ウィンドウのリサイズ処理が発生しません。
							::SendMessage(hwnd, WM_WINDOWPOSCHANGED, wParam, lParam);
							::SendMessage(hwnd, WM_SIZE, 0, 0);
						}

						return result;
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	}; inline auto player_window = std::make_shared<PlayerWindow>();
}
