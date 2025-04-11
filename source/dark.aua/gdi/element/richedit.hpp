#pragma once

namespace apn::dark::gdi
{
	struct RichEditRenderer : EditBoxRenderer
	{
#if 1
		virtual LRESULT on_subclass_proc(MessageState* current_state) override
		{
			switch (current_state->message)
			{
			case WM_PAINT:
				{
					MY_TRACE_FUNC("WM_PAINT, {/hex}, {/hex}", current_state->wParam, current_state->lParam);

					break;
				}
			case WM_ERASEBKGND:
				{
					MY_TRACE_FUNC("WM_ERASEBKGND, {/hex}, {/hex}", current_state->wParam, current_state->lParam);

					auto hwnd = current_state->hwnd;
					auto dc = (HDC)current_state->wParam;
					auto rc = my::get_client_rect(hwnd);

					// エディットボックスとして描画します。
					if (auto theme = skin::theme::manager.get_theme(VSCLASS_EDIT))
					{
						auto part_id = EP_EDITTEXT;
						auto state_id = ::IsWindowEnabled(hwnd) ? ETS_NORMAL : ETS_DISABLED;
#if 1
						// リッチエディット専用メッセージで背景色を変更します。
						if (auto state = skin::theme::manager.get_state(theme, part_id, state_id))
							::SendMessage(hwnd, EM_SETBKGNDCOLOR, FALSE, state->stuff.fill.color);
#else
						// この処理では背景色を変えられませんでした。
						if (python.call_draw_figure(hwnd, theme, dc, part_id, state_id, &rc))
							return TRUE;
#endif
					}

					break;
				}
			case WM_PRINT:
				{
					MY_TRACE_FUNC("WM_PRINT, {/hex}, {/hex}", current_state->wParam, current_state->lParam);

					break;
				}
			case WM_PRINTCLIENT:
				{
					MY_TRACE_FUNC("WM_PRINTCLIENT, {/hex}, {/hex}", current_state->wParam, current_state->lParam);

					break;
				}
			default:
				{
					MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}",
						current_state->hwnd, current_state->message, current_state->wParam, current_state->lParam);

					break;
				}
			}

			return __super::on_subclass_proc(current_state);
		}
#endif
	};
}
