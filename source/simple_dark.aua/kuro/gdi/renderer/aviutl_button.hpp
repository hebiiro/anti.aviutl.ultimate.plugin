#pragma once

namespace apn::dark::kuro::gdi
{
	struct aviutl_button_renderer_t : renderer_t
	{
		virtual BOOL on_draw_edge(message_state_t* current_state, HDC dc, LPRECT rc, UINT edge, UINT flags) override
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/hex}, {/hex}", dc, safe_string(rc), edge, flags);

			auto hwnd = current_state->hwnd;
			auto icon = (HICON)::GetWindowLongPtr(hwnd, 0);
			auto x = rc->left;
			auto y = rc->top;
			auto w = rc->right - rc->left;
			auto h = rc->bottom - rc->top;

			// ボタンとして描画します。
			const auto& palette = paint::button_material.palette;

			auto part_id = BP_PUSHBUTTON;
			auto state_id = PBS_NORMAL;

			if (edge == EDGE_SUNKEN)
			{
				x += 1;
				y += 1;
				state_id = PBS_PRESSED;
			}

			if (auto pigment = palette.get(part_id, state_id))
			{
				paint::stylus.draw_round_rect(dc, rc, pigment);

				return ::DrawIconEx(dc, x, y, icon, w, h, 0, 0, DI_NORMAL);
			}

			return hive.orig.DrawEdge(dc, rc, edge, flags);
		}
	};
}
