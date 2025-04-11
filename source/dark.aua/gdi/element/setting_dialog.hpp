#pragma once

namespace apn::dark::gdi
{
	struct SettingDialogRenderer : Renderer
	{
		inline static HWND get_combobox_index(HWND setting_dialog)
		{
			for (UINT i = 8147; i >= 8100; i--)
			{
				// ウィンドウハンドルを取得します。
				auto hwnd = ::GetDlgItem(setting_dialog, i);

				// コンボボックスかどうかクラス名で調べます。
				auto class_name = my::get_class_name(hwnd);
				if (class_name != WC_COMBOBOX) continue;

				if (::IsWindowVisible(hwnd)) // ウィンドウが可視なら
				{
					// ID - 2 のウィンドウを返します。
					return ::GetDlgItem(setting_dialog, i - 2);
				}
			}

			return 0;
		}

		virtual LRESULT on_subclass_proc(MessageState* current_state) override
		{
			auto hwnd = current_state->hwnd;
			auto message = current_state->message;
			auto wParam = current_state->wParam;
			auto lParam = current_state->lParam;

//			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_ERASEBKGND:
				{
//					MY_TRACE_FUNC("WM_ERASEBKGND, {/hex}, {/hex}", wParam, lParam);

					// 「設定ダイアログ画面サイズ固定化プラグイン」用の処理です。

					auto dc = (HDC)wParam;
					auto rc = my::get_client_rect(hwnd);

					if (auto theme = skin::theme::manager.get_theme(VSCLASS_WINDOW))
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_DIALOG, PBS_NORMAL, &rc))
							return TRUE;
					}

					break;
				}
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto id = LOWORD(wParam);
					auto sender = (HWND)lParam;

//					MY_TRACE_FUNC("WM_COMMAND, {/hex16}, {/hex16}, {/hex}", code, id, sender);

					// 「スクリプト並び替えプラグイン」用の処理です。

					if (id == 2079)
					{
						MY_TRACE("アニメーション効果が追加されました\n");

						auto result = __super::on_subclass_proc(current_state);
						auto combobox = get_combobox_index(hwnd);
						MY_TRACE_HEX(combobox);
						::SendMessage(hwnd, WM_CTLCOLOREDIT, 0, (LPARAM)combobox);
						return result;
					}

					break;
				}
			}

			return __super::on_subclass_proc(current_state);
		}

		virtual BOOL on_fill_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			if (brush == (HBRUSH)(COLOR_BTNFACE + 1))
			{
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_WINDOW))
				{
					if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_DIALOG, PBS_NORMAL, rc))
						return TRUE;
				}
			}

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(MessageState* current_state, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/}, {/hex}", dc, safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(MessageState* current_state, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(MessageState* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), edge, flags);

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(MessageState* current_state, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, ({/})", dc, safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(MessageState* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(MessageState* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, {/hex}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			// 「設定ダイアログ画面サイズ固定化プラグイン」用の処理です。
			// ::ExtTextOut()を使用した塗りつぶし処理をフックします。

			if (options == ETO_OPAQUE)
			{
				auto bk_color = ::GetBkColor(dc);
				if (bk_color == ::GetSysColor(COLOR_BTNFACE))
				{
					if (auto theme = skin::theme::manager.get_theme(VSCLASS_WINDOW))
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_DIALOG, PBS_NORMAL, rc))
							return TRUE;
					}
				}
			}

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(MessageState* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
