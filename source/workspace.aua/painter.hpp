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
			// テーマを使用するなら
			if (hive.use_theme)
			{
				// テーマAPIを使用してボーダーを描画します。
				::DrawThemeBackground(hive.theme.get(), dc, part_id, state_id, rc, 0);
			}
			// テーマを使用しないなら
			else
			{
				// ブラシで塗りつぶします。
				my::gdi::unique_ptr<HBRUSH> brush(::CreateSolidBrush(color));

				::FillRect(dc, rc, brush.get());
			}
		}
	} painter;
}
