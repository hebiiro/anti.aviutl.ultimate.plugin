#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはトラックバーのテーマをフックしてカスタム描画を実行します。
	//
	inline struct trackbar_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::trackbar_material.palette;

		//
		// 水平方向のトラックを描画します。
		//
		BOOL draw_horz_track(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return paint::stylus.draw_rect(dc, arg_rc, palette, part_id, state_id);
		}

		//
		// 水平方向のつまみを描画します。
		//
		BOOL draw_horz_thumb(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			auto rc = *arg_rc;
			rc.right -= 1;
			rc.bottom -= 1;

			return paint::stylus.draw_round_rect(dc, &rc, palette, part_id, state_id);
		}

		//
		// 垂直方向のトラックを描画します。
		//
		BOOL draw_vert_track(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return paint::stylus.draw_rect(dc, arg_rc, palette, part_id, state_id);
		}

		//
		// 垂直方向のつまみを描画します。
		//
		BOOL draw_vert_thumb(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			auto rc = *arg_rc;
			rc.right -= 1;
			rc.bottom -= 1;

			return paint::stylus.draw_round_rect(dc, &rc, palette, part_id, state_id);
		}

		HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));
#if 0
// vsstyle.hにあるトラックバーのパーツです。
enum TRACKBARPARTS {
	TKP_TRACK = 1,
	TKP_TRACKVERT = 2,
	TKP_THUMB = 3,
	TKP_THUMBBOTTOM = 4,
	TKP_THUMBTOP = 5,
	TKP_THUMBVERT = 6,
	TKP_THUMBLEFT = 7,
	TKP_THUMBRIGHT = 8,
	TKP_TICS = 9,
	TKP_TICSVERT = 10,
};
#endif
			switch (part_id)
			{
			case TKP_TRACK:
				{
					// 水平方向のトラックを描画します。
					if (draw_horz_track(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case TKP_THUMB:
			case TKP_THUMBBOTTOM:
			case TKP_THUMBTOP:
				{
					// 水平方向のつまみを描画します。
					if (draw_horz_thumb(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case TKP_TRACKVERT:
				{
					// 垂直方向のトラックを描画します。
					if (draw_vert_track(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case TKP_THUMBVERT:
			case TKP_THUMBLEFT:
			case TKP_THUMBRIGHT:
				{
					// 垂直方向のつまみを描画します。
					if (draw_vert_thumb(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			}

			return hive.orig.DrawThemeBackground(theme, dc, part_id, state_id, rc, rc_clip);
		}

		HRESULT on_draw_theme_background_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(rc), options);

			return hive.orig.DrawThemeBackgroundEx(theme, dc, part_id, state_id, rc, options);
		}

		HRESULT on_draw_theme_text(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, ({/})", theme, dc, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

			return hive.orig.DrawThemeText(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
		}

		HRESULT on_draw_theme_text_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

			return hive.orig.DrawThemeTextEx(theme, dc, part_id, state_id, text, c, text_flags, rc, options);
		}

		HRESULT on_draw_theme_icon(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int image_index) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/}", theme, dc, part_id, state_id, safe_string(rc), image_list, image_index);

			return hive.orig.DrawThemeIcon(theme, dc, part_id, state_id, rc, image_list, image_index);
		}

		HRESULT on_draw_theme_edge(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/hex}", theme, dc, part_id, state_id, safe_string(dest_rect), edge, flags);

			return hive.orig.DrawThemeEdge(theme, dc, part_id, state_id, dest_rect, edge, flags, content_rect);
		}
	} trackbar_renderer;
}
