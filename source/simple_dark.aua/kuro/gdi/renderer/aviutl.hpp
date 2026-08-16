#pragma once

#pragma once

namespace apn::dark::kuro::gdi
{
	struct aviutl_renderer_t : renderer_t
	{
		virtual LRESULT on_subclass_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/hex}", hwnd, my::message_to_string(message), w_param, l_param);

			if (message == hive.c_message.c_post_init)
			{
				// 初期化後処理を実行します。
				app->on_post_init();
			}

			return __super::on_subclass_proc(hwnd, message, w_param, l_param);
		}

		virtual BOOL on_fill_rect(message_state_t* current_state, HDC dc, LPCRECT rc, HBRUSH brush) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}", dc, safe_string(rc), brush);

			// コモンダイアログが表示されている場合は
			if (hive.comdlg32_visible_count)
			{
				if (brush == hive.orig.GetSysColorBrush(COLOR_WINDOW) ||
					paint::get_brush_color(brush) == hive.orig.GetSysColor(COLOR_WINDOW))
				{
					MY_TRACE("コモンダイアログの背景を描画します\n");
#if 0
					// ボタンの背景を使用してダイアログの背景を描画します。
					if (draw_button_background(dc, rc))
						return TRUE;
#else
					// ダイアログの背景を描画します。
					if (draw_dialog_background(dc, rc))
						return TRUE;
#endif
				}
			}

			if (brush == (HBRUSH)(COLOR_BTNFACE + 1))
			{
				// ダイアログとして描画します。
				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ETS_NORMAL;

				if (auto pigment = palette.get(part_id, state_id))
					return paint::stylus.draw_rect(dc, rc, pigment);
			}

			return hive.orig.FillRect(dc, rc, brush);
		}

		virtual BOOL on_draw_edge(message_state_t* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), edge, flags);

			// ダイアログとして描画します。
			const auto& palette = paint::dialog_material.palette;

			auto result = FALSE;

			if (flags & BF_MIDDLE)
			{
				if (auto pigment = palette.get(WP_DIALOG, PBS_NORMAL))
					result |= paint::stylus.draw_rect(dc, rc, pigment);
			}

			switch (edge)
			{
			case BDR_RAISEDOUTER:
				{
					paint::draw_single_raised_edge(dc, rc), result |= TRUE;

					break;
				}
			case BDR_SUNKENOUTER:
				{
					paint::draw_single_sunken_edge(dc, rc), result |= TRUE;

					break;
				}
			case EDGE_RAISED:
				{
					paint::draw_raised_edge(dc, rc), result |= TRUE;

					break;
				}
			case EDGE_SUNKEN:
				{
					paint::draw_sunken_edge(dc, rc), result |= TRUE;

					break;
				}
			case EDGE_BUMP:
				{
					paint::draw_bump_edge(dc, rc), result |= TRUE;

					break;
				}
			case EDGE_ETCHED:
				{
					paint::draw_etched_edge(dc, rc), result |= TRUE;

					break;
				}
			}

			if (result) return result;

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}

		virtual BOOL on_ext_text_out_w(message_state_t* current_state, HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, options = {/hex}, {/}, {/}, {/}, {/hex}, bk_color = {/hex}, text_color = {/hex}",
				dc, x, y, options, safe_string(rc), safe_string(text, c, options), c, dx, ::GetBkColor(dc), ::GetTextColor(dc));

			// コモンダイアログが表示されている場合は
			if (hive.comdlg32_visible_count)
			{
				// 背景色を取得します。
				auto bk_color = ::GetBkColor(dc);

				// エディットボックスとして描画します。
				const auto& palette = paint::editbox_material.palette;

				auto part_id = EP_EDITTEXT;
				auto state_id = ETS_NORMAL;

				// 背景色がウィンドウの色ではない場合は
				if (bk_color != hive.orig.GetSysColor(COLOR_WINDOW))
					state_id = ETS_SELECTED; // 選択状態として描画します。

				if (auto pigment = palette.get(part_id, state_id))
					return paint::stylus.ext_text_out(dc, x, y, options, rc, text, c, dx, pigment);
			}

			return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
		}
	};
}
