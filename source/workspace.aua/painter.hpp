#pragma once

namespace apn::workspace
{
	//
	// このクラスは描画を実行します。
	//
	inline struct Painter
	{
		//
		// 矩形を塗りつぶします。
		//
		void fill_background(HDC dc, LPCRECT rc)
		{
			fill_background(dc, rc, hive.fill_color, WP_DIALOG, 0);
		}

		//
		// 矩形を塗りつぶします。
		//
		void fill_background(HDC dc, LPCRECT rc, COLORREF color, int part_id, int state_id)
		{
			return fill_background(dc, rc, color, hive.theme.get(), part_id, state_id);
		}

		//
		// 矩形を塗りつぶします。
		//
		void fill_background(HDC dc, LPCRECT rc, COLORREF color, HTHEME theme, int part_id, int state_id)
		{
			// テーマを使用する場合は
			if (hive.use_theme)
			{
				// テーマAPIを使用して描画します。
				::DrawThemeBackground(theme, dc, part_id, state_id, rc, nullptr);
			}
			// テーマを使用しない場合は
			else
			{
				// ブラシで塗りつぶします。
				my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(color));

				::FillRect(dc, rc, brush.get());
			}
		}
	} painter;
}
