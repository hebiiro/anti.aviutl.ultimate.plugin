#pragma once

namespace apn::dark::kuro::gdi
{
	struct static_renderer_t : renderer_nc_t
	{
		virtual HBRUSH on_ctl_color(HWND hwnd, UINT message, HDC dc, HWND control, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}, {/hex}, {/hex}", hwnd, message, dc, control, brush);

			// スタティックコントロールの文字色と背景色を変更します。
			{
				// カラーダイアログのスタティックコントロール用の処理です。
				if (brush == (HBRUSH)::GetStockObject(DC_BRUSH))
					return __super::on_ctl_color(hwnd, message, dc, control, brush);

				// カラーダイアログのスタティックコントロール用の処理です。
				if (my::get_style(control) & SS_SUNKEN && ::GetWindowTextLength(control) == 0)
					return __super::on_ctl_color(hwnd, message, dc, control, brush);

				// ダイアログのパレットを使用します。
				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ::IsWindowEnabled(hwnd) ? ETS_NORMAL : ETS_DISABLED;

				if (auto pigment = palette.get(part_id, state_id))
				{
					// 文字色と背景色を変更します。
					::SetTextColor(dc, pigment->text.get_win32_color());
					::SetBkColor(dc, pigment->background.get_win32_color());

					// 背景色のブラシを返します。
					return pigment->background.get_brush();
				}
			}

			return __super::on_ctl_color(hwnd, message, dc, control, brush);
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
//			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_frame(message_state_t* current_state, HDC dc, LPRECT rc, UINT width, UINT type) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/}, {/hex}", dc, safe_string(rc), width, type);

			return hive.orig.DrawFrame(dc, rc, width, type);
		}

		virtual BOOL on_draw_frame_control(message_state_t* current_state, HDC dc, LPRECT rc, UINT type, UINT state) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), type, state);

			return hive.orig.DrawFrameControl(dc, rc, type, state);
		}

		virtual BOOL on_frame_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			return hive.orig.FrameRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(message_state_t* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), edge, flags);

			if (edge == EDGE_ETCHED && flags == BF_RECT)
			{
				// 凹エッジを描画します。
				return paint::draw_etched_edge(dc, rc), TRUE;
			}

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_draw_focus_rect(message_state_t* current_state, HDC dc, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, ({/})", dc, safe_string(rc));

			return hive.orig.DrawFocusRect( dc, rc);
		}

		virtual BOOL on_draw_state_w(message_state_t* current_state, HDC dc, HBRUSH fore, DRAWSTATEPROC cb, LPARAM lData, WPARAM wData, int x, int y, int cx, int cy, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, fore, x, y, cx, cy, flags);

			// 無効状態の文字列を描画する場合は
			if (flags == DSS_DISABLED)
			{
				// ダイアログのパレットを使用します。
				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ETS_DISABLED;

				auto rc = RECT { x, y, x + cx, y + cy };
				auto ws = my::get_window_text(current_state->hwnd);

				auto text_flags = DT_LEFT | DT_VCENTER | DT_WORDBREAK | DT_SINGLELINE | DT_EXPANDTABS | DT_NOCLIP | DT_HIDEPREFIX;

				return paint::stylus.d2d_draw_text(dc, &rc, ws.c_str(), (int)ws.length(), text_flags, palette, part_id, state_id);
			}

			return hive.orig.DrawStateW(dc, fore, cb, lData, wData, x, y, cx, cy, flags);
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
//			MY_TRACE_FUNC("{/hex}, {/}, {/}, options = {/hex}, {/}, {/}, {/}, {/hex}, bk_color = {/hex}, text_color = {/hex}",
//				dc, x, y, options, safe_string(rc), safe_string(text, c, options), c, dx, ::GetBkColor(dc), ::GetTextColor(dc));
#if 0 // ここの代わりにon_ctl_color()とon_draw_text_ex_w()で処理するように変更しました。
//			if (!(options & ETO_IGNORELANGUAGE))
			{
				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ::IsWindowEnabled(current_state->hwnd) ? ETS_NORMAL : ETS_DISABLED;

				if (auto pigment = palette.get(part_id, state_id))
					return paint::stylus.ext_text_out(dc, x, y, options, rc, text, c, dx, pigment);
			}
#endif
			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}

		virtual BOOL on_draw_text_ex_w(message_state_t* current_state, HDC dc, LPWSTR text, int c, LPRECT rc, UINT flags, LPDRAWTEXTPARAMS dtp) override
		{
			MY_TRACE_FUNC("dc = {/hex}, flags = {/hex}, rc = ({/}), text = {/}, bk_color = {/hex}, text_color = {/hex}",
				dc, flags, safe_string(rc), safe_string(text, c), ::GetBkColor(dc), ::GetTextColor(dc));
#if 1
			// 有効状態の文字列だけを描画対象にします。
			if (!(flags & (DT_CALCRECT | DT_MODIFYSTRING)) && !dtp && ::IsWindowEnabled(current_state->hwnd))
			{
				// ダイアログのパレットを使用します。
				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ETS_NORMAL;

				return paint::stylus.d2d_draw_text(dc, rc, text, c, flags, palette, part_id, state_id);
			}
#endif
			return hive.orig.DrawTextExW(dc, text, c, rc, flags, dtp);
		}

		virtual BOOL on_pat_blt(message_state_t* current_state, HDC dc, int x, int y, int w, int h, DWORD rop) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}, {/}, {/hex}", dc, x, y, w, h, rop);

			return hive.orig.PatBlt(dc, x, y, w, h, rop);
		}
	};
}
