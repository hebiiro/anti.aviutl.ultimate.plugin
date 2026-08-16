#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはGDIオブジェクトの属性をDCにセットします。
	//
	template <typename T, auto init>
	struct gdi_object_attribute_t
	{
		//
		// DCのハンドルです。
		//
		HDC dc = {};

		//
		// GDIオブジェクトのハンドルです。
		//
		T handle = {};

		//
		// 以前のGDIオブジェクトのハンドルです。
		//
		T old_handle = {};

		//
		// 削除可能かどうかのフラグです。
		//
		BOOL is_deletable = {};

		//
		// コンストラクタです。
		//
		gdi_object_attribute_t(HDC dc, const pigment_t* pigment)
			: dc(dc)
		{
			init(this, pigment);

			old_handle = (T)::SelectObject(dc, handle);
		}

		//
		// デストラクタです。
		//
		~gdi_object_attribute_t()
		{
			::SelectObject(dc, old_handle);

			if (is_deletable)
				::DeleteObject(handle);
		}

		//
		// GDIオブジェクトのハンドルを返します。
		//
		operator T() const { return handle; }
	};

	//
	// このクラスはペンの属性をDCにセットします。
	//
	using pen_attribute_t = gdi_object_attribute_t<HPEN, [](auto p, const pigment_t* pigment)
	{
		if (pigment->border.is_valid() && pigment->border.is_opaque())
		{
			p->handle = ::CreatePen(PS_INSIDEFRAME, get_border_width_as_int(), pigment->border.get_win32_color());
			p->is_deletable = TRUE;
		}
		else
		{
			p->handle = (HPEN)::GetStockObject(NULL_PEN);
			p->is_deletable = FALSE;
		}
	}>;

	//
	// このクラスはブラシの属性をDCにセットします。
	//
	using brush_attribute_t = gdi_object_attribute_t<HBRUSH, [](auto p, const pigment_t* pigment)
	{
		if (pigment->background.is_valid() && pigment->background.is_opaque())
		{
			p->handle = ::CreateSolidBrush(pigment->background.get_win32_color());
			p->is_deletable = TRUE;
		}
		else
		{
			p->handle = (HBRUSH)::GetStockObject(NULL_BRUSH);
			p->is_deletable = FALSE;
		}
	}>;

	//
	// このクラスはテキスト属性のベースクラスです。
	//
	struct text_attribute_base_t
	{
		//
		// DCのハンドルです。
		//
		HDC dc = {};

		//
		// 以前の背景モードです。
		//
		int old_bk_mode = {};

		//
		// 以前の背景の色です。
		//
		COLORREF old_bk_color = {};

		//
		// 以前のテキストの色です。
		//
		COLORREF old_text_color = {};

		//
		// コンストラクタです。
		//
		text_attribute_base_t(HDC dc, COLORREF background_color, COLORREF text_color, BOOL opaque)
			: dc(dc)
			, old_bk_mode(::GetBkMode(dc))
			, old_bk_color(::GetBkColor(dc))
			, old_text_color(::GetTextColor(dc))
		{
			if (background_color != CLR_NONE && opaque)
				::SetBkColor(dc, background_color);
			else
				::SetBkMode(dc, TRANSPARENT);

			if (text_color != CLR_NONE)
				::SetTextColor(dc, text_color);
		}

		//
		// デストラクタです。
		//
		~text_attribute_base_t()
		{
			::SetTextColor(dc, old_text_color);
			::SetBkColor(dc, old_bk_color);
			::SetBkMode(dc, old_bk_mode);
		}
	};

	//
	// このクラスはテキストの属性をDCにセットします。
	//
	struct text_attribute_t : text_attribute_base_t
	{
		text_attribute_t(HDC dc, const pigment_t* pigment, BOOL opaque = TRUE) : text_attribute_base_t
			(dc, pigment->background.get_win32_color(), pigment->text.get_win32_color(), opaque) {}
	};

	//
	// このクラスは影の属性をDCにセットします。
	//
	struct text_shadow_attribute_t : text_attribute_base_t
	{
		text_shadow_attribute_t(HDC dc, const pigment_t* pigment, BOOL opaque = TRUE) : text_attribute_base_t
			(dc, pigment->background.get_win32_color(), pigment->text_shadow.get_win32_color(), opaque) {}
	};

	//
	// このクラスはアイコンの寸法です。
	//
	struct icon_metrics_t
	{
		int height;
		int width;
		int weight;
	};

	//
	// このクラスはアイコンの属性をDCにセットします。
	//
	struct icon_attribute_t
	{
		my::gdi::unique_ptr<HFONT> font;
		my::gdi::selector font_selector;

		icon_attribute_t(HDC dc, LPCRECT rc, LPCWSTR font_name, const icon_metrics_t& icon_metrics)
			: font(::CreateFontW(
				icon_metrics.height ? icon_metrics.height : my::get_height(*rc),
				icon_metrics.width, 0, 0, icon_metrics.weight,
				FALSE, FALSE, FALSE,
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY, DEFAULT_PITCH, font_name))
			, font_selector(dc, font.get())
		{
		}
	};

	//
	// このクラスはスタイラスです。
	// 主にピグメントを使用して各種図形などを描画します。
	//
	inline struct stylus_t
	{
		//
		// ピグメントを使用して矩形を描画します。
		//
		BOOL draw_rect(HDC dc, LPCRECT rc, const pigment_t* pigment)
		{
			if (pigment->border.is_valid())
			{
				pen_attribute_t pen_attribute(dc, pigment);
				brush_attribute_t brush_attribute(dc, pigment);

				return hive.orig.Rectangle(dc, rc->left, rc->top, rc->right, rc->bottom);
			}
			else
			{
				my::gdi::unique_ptr<HBRUSH> brush(
					::CreateSolidBrush(pigment->background.get_win32_color()));

				return hive.orig.FillRect(dc, rc, brush.get());
			}
		}

		//
		// ピグメントを使用して丸角矩形を描画します。
		//
		BOOL draw_round_rect(HDC dc, LPCRECT arg_rc, const pigment_t* pigment)
		{
			if (!hive.round.flag_use) return draw_rect(dc, arg_rc, pigment);

			auto rc = *arg_rc;
#if 0
			rc.right -= 1;
			rc.bottom -= 1;
#endif
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);
			auto r = std::min(w, h);

			if (hive.d2d.flag_figure)
			{
				auto radius = get_round_as_float(r / 2.0f);

				if (auto result = d2d::recter_t(dc, &rc, pigment).draw_round_rect(radius))
					return result;
			}

			auto round = get_round_as_int(r);

			pen_attribute_t pen_attribute(dc, pigment);
			brush_attribute_t brush_attribute(dc, pigment);

			return ::RoundRect(dc, rc.left, rc.top, rc.right, rc.bottom, round, round);
		}

		//
		// ピグメントを使用して文字列を描画します。
		//
		BOOL ext_text_out(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx, const pigment_t* pigment, BOOL opaque = TRUE)
		{
			// このスコープ内では::ExtTextOutW()をフックしないようにします。
			my::locker_t locker(&ext_text_out_lock);

			// 影を描画する場合は
			if (hive.shadow.flag_use && pigment->text_shadow.is_valid())
			{
				auto offset = get_shadow_offset_as_int();

				auto fill_color = pigment->background.get_win32_color();
				auto text_color = pigment->text.get_win32_color();
				auto text_shadow_color = pigment->text_shadow.get_win32_color();

				auto old_bk_mode = ::GetBkMode(dc);
				auto old_bk_color = ::GetBkColor(dc);
				auto old_text_color = ::GetTextColor(dc);

				if (options & ETO_OPAQUE)
				{
					// 背景を塗りつぶします。

					if (fill_color != CLR_NONE)
						::SetBkColor(dc, fill_color);

					hive.orig.ExtTextOutW(dc, x, y, options, rc, nullptr, 0, dx);
				}

				if (text && c)
				{
					// 文字列を描画します。

					::SetBkMode(dc, TRANSPARENT);

					if (text_shadow_color != CLR_NONE)
					{
						auto align = ::GetTextAlign(dc);
						::SetTextAlign(dc, align & ~TA_UPDATECP);
						::SetTextColor(dc, text_shadow_color);

						x += offset.x, y += offset.y;
						hive.orig.ExtTextOutW(dc, x, y, options & ~ETO_OPAQUE, rc, text, c, dx);
						x -= offset.x, y -= offset.y;

						::SetTextAlign(dc, align);
					}

					::SetTextColor(dc, text_color);

					hive.orig.ExtTextOutW(dc, x, y, options & ~ETO_OPAQUE, rc, text, c, dx);
				}

				::SetBkMode(dc, old_bk_mode);
				::SetBkColor(dc, old_bk_color);
				::SetTextColor(dc, old_text_color);

				return TRUE;
			}
			// 影を描画しない場合は
			else
			{
				// テキストを描画します。
				{
					text_attribute_t text_attribute(dc, pigment, opaque);

					return hive.orig.ExtTextOutW(dc, x, y, options, rc, text, c, dx);
				}
			}
		}

		//
		// ピグメントを使用して文字列を描画します。
		//
		BOOL draw_text(HDC dc, LPCRECT rc, LPCWSTR text, int c, DWORD text_flags, const pigment_t* pigment, BOOL opaque = TRUE)
		{
			// このスコープ内では::ExtTextOutW()をフックしないようにします。
			my::locker_t locker(&ext_text_out_lock);

			// 影を描画する場合は
			if (hive.shadow.flag_use && pigment->text_shadow.is_valid())
			{
				// テキストの影を描画します。
				text_shadow_attribute_t text_attribute(dc, pigment, opaque);

				auto offset = get_shadow_offset_as_int();

				auto rc2 = *rc;
				::OffsetRect(&rc2, offset.x, offset.y);

				hive.orig.DrawTextW(dc, text, c, &rc2, text_flags);
			}

			// テキストを描画します。

			text_attribute_t text_attribute(dc, pigment, FALSE);

			return !!hive.orig.DrawTextW(dc, text, c, (LPRECT)rc, text_flags);
		}

		//
		// ピグメントを使用して絵文字を描画します。
		//
		BOOL draw_icon(HDC dc, LPCRECT rc, const pigment_t* pigment, LPCWSTR font_name, WCHAR char_code, const icon_metrics_t& icon_metrics = {})
		{
			// アイコンの属性をセットします。
			icon_attribute_t icon_attribute(dc, rc, font_name, icon_metrics);

			// アイコンを描画します。
			return draw_text(dc, rc, &char_code, 1,
				DT_CENTER | DT_VCENTER | DT_SINGLELINE, pigment, FALSE);
		}

		//
		// ピグメントを使用して文字列を描画します。
		//
		BOOL d2d_draw_text(HDC dc, LPCRECT rc, LPCWSTR text, int c, DWORD text_flags, const pigment_t* pigment, BOOL opaque = TRUE)
		{
			// D2Dを使用する場合はD2Dで描画します。
			if (hive.d2d.flag_text)
			{
				text_attribute_t text_attribute(dc, pigment, opaque);

				if (auto result = d2d::texter_t(dc, text, c, rc, text_flags, pigment).draw_text())
					return !!result;
			}

			// それ以外の場合はGDIで描画します。
			return draw_text(dc, rc, text, c, text_flags, pigment, opaque);
		}

		//
		// ピグメントを使用して絵文字を描画します。
		//
		BOOL d2d_draw_icon(HDC dc, LPCRECT rc, const pigment_t* pigment, LPCWSTR font_name, WCHAR char_code, const icon_metrics_t& icon_metrics = {})
		{
			// D2Dを使用する場合はD2Dで描画します。
			if (hive.d2d.flag_text)
			{
				icon_attribute_t icon_attribute(dc, rc, font_name, icon_metrics);

				auto text_flags = UINT { DT_CENTER | DT_VCENTER | DT_SINGLELINE };

				if (auto result = d2d::texter_t(dc, &char_code, 1, rc, text_flags, pigment).draw_text())
					return !!result;
			}

			// それ以外の場合はGDIで描画します。
			return draw_icon(dc, rc, pigment, font_name, char_code, icon_metrics);
		}

		//
		// パレットを使用して矩形を描画します。
		//
		inline BOOL draw_rect(HDC dc, LPCRECT rc,
			const paint::palette_t& palette, int part_id, int state_id)
		{
			if (auto pigment = palette.get(part_id, state_id))
				return draw_rect(dc, rc, pigment);

			return FALSE;
		}

		//
		// パレットを使用して丸角矩形を描画します。
		//
		inline BOOL draw_round_rect(HDC dc, LPCRECT rc,
			const paint::palette_t& palette, int part_id, int state_id)
		{
			if (auto pigment = palette.get(part_id, state_id))
				return draw_round_rect(dc, rc, pigment);

			return FALSE;
		}

		//
		// パレットを使用して文字列を描画します。
		//
		inline BOOL ext_text_out(HDC dc, int x, int y, UINT options, LPCRECT rc, LPCWSTR text, UINT c, CONST INT* dx,
			const paint::palette_t& palette, int part_id, int state_id, BOOL opaque = TRUE)
		{
			if (auto pigment = palette.get(part_id, state_id))
				return ext_text_out(dc, x, y, options, rc, text, c, dx, pigment, opaque);

			return FALSE;
		}

		//
		// パレットを使用して文字列を描画します。
		//
		inline BOOL draw_text(HDC dc, LPCRECT rc, LPCWSTR text, int c, DWORD text_flags,
			const paint::palette_t& palette, int part_id, int state_id, BOOL opaque = TRUE)
		{
			if (auto pigment = palette.get(part_id, state_id))
				return draw_text(dc, rc, text, c, text_flags, pigment, opaque);

			return FALSE;
		}

		//
		// パレットを使用して絵文字を描画します。
		//
		inline BOOL draw_icon(HDC dc, LPCRECT rc,
			const paint::palette_t& palette, int part_id, int state_id,
			LPCWSTR font_name, WCHAR char_code, const icon_metrics_t& icon_metrics = {})
		{
			if (auto pigment = palette.get(part_id, state_id))
				return draw_icon(dc, rc, pigment, font_name, char_code, icon_metrics);

			return FALSE;
		}

		//
		// パレットを使用して文字列を描画します。
		//
		inline BOOL d2d_draw_text(HDC dc, LPCRECT rc, LPCWSTR text, int c, DWORD text_flags,
			const paint::palette_t& palette, int part_id, int state_id, BOOL opaque = TRUE)
		{
			if (auto pigment = palette.get(part_id, state_id))
				return d2d_draw_text(dc, rc, text, c, text_flags, pigment, opaque);

			return FALSE;
		}

		//
		// パレットを使用して絵文字を描画します。
		//
		inline BOOL d2d_draw_icon(HDC dc, LPCRECT rc,
			const paint::palette_t& palette, int part_id, int state_id,
			LPCWSTR font_name, WCHAR char_code, const icon_metrics_t& icon_metrics = {})
		{
			if (auto pigment = palette.get(part_id, state_id))
				return d2d_draw_icon(dc, rc, pigment, font_name, char_code, icon_metrics);

			return FALSE;
		}
	} stylus;
}
