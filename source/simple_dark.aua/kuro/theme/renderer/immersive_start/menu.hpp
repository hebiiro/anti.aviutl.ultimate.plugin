#pragma once

namespace apn::dark::kuro::theme::immersive_start
{
	//
	// このクラスはメニューのテーマをフックしてカスタム描画を実行します。
	//
	inline struct menu_renderer_t : base::menu_renderer_t
	{
		virtual HRESULT on_draw_theme_background(HTHEME theme, HDC dc, int part_id, int state_id, LPCRECT rc, LPCRECT rc_clip) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, ({/}), ({/})", theme, dc, part_id, state_id, safe_string(rc), safe_string(rc_clip));

			{
				switch (part_id)
				{
				case MENU_POPUPSEPARATOR:
					{
						// ポップアップメニューのセパレータ用の描画です。

						auto rc2 = *rc;
						::InflateRect(&rc2, 0, -3);

						if (paint::stylus.draw_rect(dc, &rc2, palette, part_id, state_id))
							return S_OK;

						break;
					}
				case MENU_POPUPITEM:
				case MENU_POPUPITEMFOCUSABLE:
					{
						// ポップアップメニューのアイテムを描画します。

						// ホット状態以外の場合は
						if (state_id != MPI_HOT)
						{
							if (paint::stylus.draw_rect(dc, rc, palette, part_id, state_id))
								return S_OK;
						}
						// ホット状態の場合は
						else
						{
							if (paint::stylus.draw_round_rect(dc, rc, palette, part_id, state_id))
								return S_OK;
						}

						break;
					}
				default:
					{
						if (paint::stylus.draw_round_rect(dc, rc, palette, part_id, state_id))
							return S_OK;

						break;
					}
				}
			}

			return __super::on_draw_theme_background(theme, dc, part_id, state_id, rc, rc_clip);
		}
#if 0
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
#endif
		virtual HRESULT on_draw_theme_text_ex(HTHEME theme, HDC dc, int part_id, int state_id, LPCWSTR text, int c, DWORD text_flags, LPRECT rc, const DTTOPTS* options) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/}, {/}, {/}, {/hex}, ({/}), {/hex}", theme, dc, part_id, state_id, safe_string(text, c), text_flags, safe_string(rc), options);

			if (!(text_flags & DT_CALCRECT))
			{
				if (auto pigment = palette.get(part_id, state_id))
				{
					// 内部で::FillRect()や::ExtTextOutW()が呼ばれるのでロックします。
					GdiHookLocker gdi_hook_locker;

					// 構造体のサイズが異なる可能性があるので
					// 自前でメモリを確保してからコピーします。
					auto struct_size = sizeof(*options);
					auto size = options->dwSize;
					auto buffer = std::make_unique<uint8_t[]>(size);
					memcpy(buffer.get(), options, size);
					auto options2 = (DTTOPTS*)buffer.get();
					options2->dwFlags |= DTT_TEXTCOLOR;
					options2->crText = pigment->text.get_win32_color();

					return __super::on_draw_theme_text_ex(theme, dc, part_id, state_id, text, c, text_flags, rc, options2);
				}
			}

			return __super::on_draw_theme_text_ex(theme, dc, part_id, state_id, text, c, text_flags, rc, options);
		}
	} menu_renderer;
}
