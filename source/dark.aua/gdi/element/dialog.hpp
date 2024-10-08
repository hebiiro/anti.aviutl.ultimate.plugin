#pragma once

namespace apn::dark::gdi
{
	struct DialogRenderer : RendererNc
	{
		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, dc, control, brush);

			if (auto theme = skin::theme::manager.get_theme(VSCLASS_DIALOG))
			{
				auto part_id = WP_DIALOG;
				auto state_id = ::IsWindowEnabled(hwnd) ? ETS_NORMAL : ETS_DISABLED;

				return skin::theme::manager.get_fill_brush(dc, brush, theme, part_id, state_id);
			}

			return __super::on_ctl_color(hwnd, message, dc, control, brush);
		}

		virtual BOOL on_fill_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {:#010x}", dc, safe_string(rc), brush);
#if 1
			auto instance = (HINSTANCE)::GetWindowLongPtr(current_state->hwnd, GWLP_HINSTANCE);
			auto style = my::get_style(current_state->hwnd);
			if (instance == ::GetModuleHandle(_T("comdlg32.dll")) && style & WS_THICKFRAME)
			{
				// ファイル選択ダイアログのコンボボックス＆コンボリストボックス用の処理です。
				// WS_THICKFRAMEがない場合はカラーピッカーダイアログなので何もしません。

				if (auto theme = skin::theme::manager.get_theme(VSCLASS_LISTBOX))
				{
					auto color = painter::get_brush_color(brush);
					if (color == ::GetSysColor(COLOR_WINDOW))
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, EP_EDITTEXT, ETS_NORMAL, rc))
							return TRUE;
					}
					else
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, EP_EDITTEXT, ETS_SELECTED, rc))
							return TRUE;
					}
				}
			}
#endif
			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(MessageState* current_state, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {}, {:#010x}", dc, safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(MessageState* current_state, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {:#010x}, {:#010x}", dc, safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(MessageState* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {:#010x}", dc, safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(MessageState* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{:#010x}, ({}), {:#010x}, {:#010x}", dc, safe_string(rc), edge, flags);

			int part_id = 0;
			int state_id = 0;

			switch (edge)
			{
			case EDGE_RAISED: part_id = WP_RAISED_EDGE; break;
			case EDGE_SUNKEN: part_id = WP_SUNKEN_EDGE; break;
			case EDGE_BUMP: part_id = WP_BUMP_EDGE; break;
			case EDGE_ETCHED: part_id = WP_ETCHED_EDGE; break;
			}

			if (part_id != 0)
			{
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_WINDOW))
				{
					BOOL result = FALSE;

					if (flags & BF_MIDDLE)
					{
						if (python.call_draw_figure(current_state->hwnd, theme, dc, WP_DIALOG, PBS_NORMAL, rc))
							result |= TRUE;
					}

					if (python.call_draw_figure(current_state->hwnd, theme, dc, part_id, state_id, rc))
					{
						if (flags & BF_ADJUST)
							::InflateRect(rc, -2, -2);

						result |= TRUE;
					}

					if (result) return TRUE;
				}
			}

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(MessageState* current_state, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{:#010x}, ({})", dc, safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(MessageState* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}, {}, {}, {:#010x}", dc, fore, x, y, cx, cy, flags);

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(MessageState* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{:#010x}, {}, {}, {:#010x}, {}, {}, {}, {:#010x}, {:#010x}, {:#010x}", dc, x, y, options, safe_string(rc), text, c, dx, ::GetBkColor(dc), ::GetTextColor(dc));
#if 1
			if (options == ETO_OPAQUE)
			{
				auto instance = (HINSTANCE)::GetWindowLongPtr(current_state->hwnd, GWLP_HINSTANCE);
				if (instance == ::GetModuleHandle(_T("comdlg32.dll")))
				{
					// ファイル選択ダイアログのコンボボックス＆コンボリストボックス用の処理です。

					if (auto theme = skin::theme::manager.get_theme(VSCLASS_LISTBOX))
					{
						auto bk_color = ::GetBkColor(dc);
						if (bk_color == RGB(0xff, 0xff, 0xff))
						{
							if (python.call_text_out(current_state->hwnd, theme, dc, EP_EDITTEXT, ETS_NORMAL, x, y, options, rc, text, c, dx))
								return TRUE;
						}
						else
						{
							if (python.call_text_out(current_state->hwnd, theme, dc, EP_EDITTEXT, ETS_SELECTED, x, y, options, rc, text, c, dx))
								return TRUE;
						}
					}
				}
			}
#endif
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_pat_blt(MessageState* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{:#010x}, {}, {}, {}, {}, {:#010x}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
