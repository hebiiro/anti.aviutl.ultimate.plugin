#pragma once

namespace apn::dark::kuro::theme
{
	//
	// このクラスはスクロールバーのテーマをフックしてカスタム描画を実行します。
	//
	inline struct scrollbar_renderer_t : renderer_t
	{
		const paint::palette_t& palette = paint::scrollbar_material.palette;

		//
		// スクロールバーの太さの倍率を返します。
		//
		float get_ratio()
		{
			return (100 - hive.scrollbar.reduction) / 200.0f;
		}

		//
		// スクロールバーの背景を描画します。
		//
		BOOL draw_background(HDC dc, LPCRECT arg_rc)
		{
#if 1
			return paint::stylus.draw_rect(dc, arg_rc, palette, 0, 0);
#else
			// 背景はダイアログのマテリアルで描画します。
			const auto& palette = paint::dialog_material.palette;

			auto part_id = WP_DIALOG;
			auto state_id = ETS_NORMAL;

			return paint::stylus.draw_rect(dc, arg_rc, palette, part_id, state_id);
#endif
		}

		//
		// 水平方向の矢印を描画します。
		//
		BOOL draw_horz_arrow(HDC dc, LPCRECT arg_rc, int part_id, int state_id, BOOL near_arrow)
		{
			draw_background(dc, arg_rc);

			auto rc = *arg_rc;

			if (hive.scrollbar.flag_arrow_as_button)
			{
				auto ratio = get_ratio();
				::InflateRect(&rc, -int(my::get_width(rc) * ratio), -int(my::get_height(rc) * ratio));

				paint::stylus.draw_round_rect(dc, &rc, palette, part_id, state_id);
			}

			auto size = my::get_height(rc) - 4;
			auto icon_metrics = paint::icon_metrics_t { size };

			return paint::stylus.draw_icon(dc, &rc, palette, part_id, state_id, paint::symbol::k_font_name,
				near_arrow ? paint::symbol::k_chevron_left : paint::symbol::k_chevron_right, icon_metrics);
		}

		//
		// 垂直方向の矢印を描画します。
		//
		BOOL draw_vert_arrow(HDC dc, LPCRECT arg_rc, int part_id, int state_id, BOOL near_arrow)
		{
			draw_background(dc, arg_rc);

			auto rc = *arg_rc;

			if (hive.scrollbar.flag_arrow_as_button)
			{
				auto ratio = get_ratio();
				::InflateRect(&rc, -int(my::get_width(rc) * ratio), -int(my::get_height(rc) * ratio));

				paint::stylus.draw_round_rect(dc, &rc, palette, part_id, state_id);
			}

			auto size = my::get_width(rc) - 4;
			auto icon_metrics = paint::icon_metrics_t { size };

			return paint::stylus.draw_icon(dc, &rc, palette, part_id, state_id, paint::symbol::k_font_name,
				near_arrow ? paint::symbol::k_chevron_up : paint::symbol::k_chevron_down, icon_metrics);
		}

		//
		// 左矢印を描画します。
		//
		BOOL draw_left(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_horz_arrow(dc, arg_rc, part_id, state_id, TRUE);
		}

		//
		// 右矢印を描画します。
		//
		BOOL draw_right(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_horz_arrow(dc, arg_rc, part_id, state_id, FALSE);
		}

		//
		// 上矢印を描画します。
		//
		BOOL draw_up(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_vert_arrow(dc, arg_rc, part_id, state_id, TRUE);
		}

		//
		// 下矢印を描画します。
		//
		BOOL draw_down(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_vert_arrow(dc, arg_rc, part_id, state_id, FALSE);
		}

		//
		// 水平方向のつまみを描画します。
		//
		BOOL draw_horz_thumb(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			draw_background(dc, arg_rc);

			auto rc = *arg_rc;
			auto ratio = get_ratio();
			::InflateRect(&rc, 0, -int(my::get_height(rc) * ratio));

			return paint::stylus.draw_round_rect(dc, &rc, palette, part_id, state_id);
		}

		//
		// 垂直方向のつまみを描画します。
		//
		BOOL draw_vert_thumb(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			draw_background(dc, arg_rc);

			auto rc = *arg_rc;
			auto ratio = get_ratio();
			::InflateRect(&rc, -int(my::get_width(rc) * ratio), 0);

			return paint::stylus.draw_round_rect(dc, &rc, palette, part_id, state_id);
		}

		//
		// 水平方向の下層トラックを描画します。
		//
		BOOL draw_horz_lower_track(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_background(dc, arg_rc);
		}

		//
		// 水平方向の上層トラックを描画します。
		//
		BOOL draw_horz_upper_track(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_background(dc, arg_rc);
		}

		//
		// 垂直方向の下層トラックを描画します。
		//
		BOOL draw_vert_lower_track(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_background(dc, arg_rc);
		}

		//
		// 垂直方向の上層トラックを描画します。
		//
		BOOL draw_vert_upper_track(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			return draw_background(dc, arg_rc);
		}

		//
		// 水平方向のグリッパーを描画します。
		//
		BOOL draw_horz_gripper(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			if (!hive.scrollbar.flag_has_gripper) return FALSE;

			auto rc = *arg_rc;

			// 矩形を正方形にします。
			auto h = my::get_height(rc);
			rc.left = (rc.left + rc.right - h) / 2;
			rc.right = rc.left + h;

			auto ratio = get_ratio();
			auto inflate = -int(h * ratio) - 1;
			::InflateRect(&rc, -inflate, inflate);

			auto icon_metrics = paint::icon_metrics_t { my::get_height(rc), my::get_width(rc) };

			return paint::stylus.draw_icon(dc, &rc, palette, part_id, state_id, paint::symbol::k_font_name, paint::symbol::k_gripper_bar_vertical, icon_metrics);
		}

		//
		// 垂直方向のグリッパーを描画します。
		//
		BOOL draw_vert_gripper(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			if (!hive.scrollbar.flag_has_gripper) return FALSE;

			auto rc = *arg_rc;

			// 矩形を正方形にします。
			auto w = my::get_width(rc);
			rc.top = (rc.top + rc.bottom - w) / 2;
			rc.bottom = rc.top + w;

			auto ratio = get_ratio();
			auto inflate = -int(w * ratio) - 1;
			::InflateRect(&rc, inflate, -inflate);

			auto icon_metrics = paint::icon_metrics_t { my::get_height(rc), my::get_width(rc) };

			return paint::stylus.draw_icon(dc, &rc, palette, part_id, state_id, paint::symbol::k_font_name, paint::symbol::k_gripper_bar_horizontal, icon_metrics);
		}

		//
		// サイズ変更グリップを描画します。
		//
		BOOL draw_sizebox(HDC dc, LPCRECT arg_rc, int part_id, int state_id)
		{
			// 背景を描画してから
			if (paint::stylus.draw_rect(dc, arg_rc, palette, part_id, state_id))
			{
#if 1
				// 該当するアイコンが存在しないので、何も描画しません。
				return TRUE;
#else
				// サイズグリップ用のアイコンを描画します。
				return paint::stylus.draw_icon(dc, arg_rc, palette, part_id, state_id,
					paint::symbol::k_font_name, paint::symbol::k_sip_move);
//					paint::symbol::k_font_name, paint::symbol::k_gripper_resize);
#endif
			}

			return FALSE;
		}

		virtual HRESULT on_get_theme_color(HTHEME theme, int part_id, int state_id, int prop_id, COLORREF* result) override
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}, {/}", theme, part_id, state_id, prop_id);

			return __super::on_get_theme_color(theme, part_id, state_id, prop_id, result);
		}

		virtual HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			switch (part_id)
			{
			case SBP_ARROWBTN:
				{
					switch (state_id)
					{
					case ABS_LEFTNORMAL:
					case ABS_LEFTHOVER:
					case ABS_LEFTHOT:
					case ABS_LEFTPRESSED:
					case ABS_LEFTDISABLED:
						{
							if (draw_left(dc, rc, part_id, state_id))
								return S_OK;

							break;
						}
					case ABS_RIGHTNORMAL:
					case ABS_RIGHTHOVER:
					case ABS_RIGHTHOT:
					case ABS_RIGHTPRESSED:
					case ABS_RIGHTDISABLED:
						{
							if (draw_right(dc, rc, part_id, state_id))
								return S_OK;

							break;
						}
					case ABS_UPNORMAL:
					case ABS_UPHOVER:
					case ABS_UPHOT:
					case ABS_UPPRESSED:
					case ABS_UPDISABLED:
						{
							if (draw_up(dc, rc, part_id, state_id))
								return S_OK;

							break;
						}
					case ABS_DOWNNORMAL:
					case ABS_DOWNHOVER:
					case ABS_DOWNHOT:
					case ABS_DOWNPRESSED:
					case ABS_DOWNDISABLED:
						{
							if (draw_down(dc, rc, part_id, state_id))
								return S_OK;

							break;
						}
					}

					break;
				}
			case SBP_THUMBBTNHORZ:
				{
					if (draw_horz_thumb(dc, rc, part_id, state_id))
					{
						draw_horz_gripper(dc, rc, part_id, state_id);

						return S_OK;
					}

					break;
				}
			case SBP_THUMBBTNVERT:
				{
					if (draw_vert_thumb(dc, rc, part_id, state_id))
					{
						draw_vert_gripper(dc, rc, part_id, state_id);

						return S_OK;
					}

					break;
				}
			case SBP_LOWERTRACKHORZ:
				{
					if (draw_horz_lower_track(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case SBP_UPPERTRACKHORZ:
				{
					if (draw_horz_upper_track(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case SBP_LOWERTRACKVERT:
				{
					if (draw_vert_lower_track(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case SBP_UPPERTRACKVERT:
				{
					if (draw_vert_upper_track(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case SBP_GRIPPERHORZ:
				{
					break;
				}
			case SBP_GRIPPERVERT:
				{
					break;
				}
			case SBP_SIZEBOX:
				{
					if (draw_sizebox(dc, rc, part_id, state_id))
						return S_OK;

					break;
				}
			case SBP_SIZEBOXBKGND:
				{
					break;
				}
			}

			return __super::on_draw_theme_background(theme, dc, part_id, state_id, rc, rc_clip);
		}

		virtual HRESULT on_draw_theme_background_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, const DTBGOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(rc), options);

			return __super::on_draw_theme_background_ex(theme, dc, part_id, state_id, rc, options);
		}

		virtual HRESULT on_draw_theme_text(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, DWORD text_flags2, LPCRECT rc) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}, ({/})", theme, dc, part_id, state_id, safe_string(text, c), text_flags, text_flags2, safe_string(rc));

			return __super::on_draw_theme_text(theme, dc, part_id, state_id, text, c, text_flags, text_flags2, rc);
		}

		virtual HRESULT on_draw_theme_text_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

			return __super::on_draw_theme_text_ex(theme, dc, part_id, state_id, text, c, text_flags, rc, options);
		}

		virtual HRESULT on_draw_theme_icon(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, HIMAGELIST image_list, int image_index) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/}", theme, dc, part_id, state_id, safe_string(rc), image_list, image_index);

			return __super::on_draw_theme_icon(theme, dc, part_id, state_id, rc, image_list, image_index);
		}

		virtual HRESULT on_draw_theme_edge(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT dest_rect, UINT edge, UINT flags, LPRECT content_rect) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), {/hex}, {/hex}", theme, dc, part_id, state_id, safe_string(dest_rect), edge, flags);

			return __super::on_draw_theme_edge(theme, dc, part_id, state_id, dest_rect, edge, flags, content_rect);
		}
	} scrollbar_renderer;
}
