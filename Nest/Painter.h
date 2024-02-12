#pragma once
#include "Hive.h"

namespace fgo::nest
{
	//
	// このクラスは描画を実行します。
	//
	inline struct Painter
	{
		//
		// 矩形を塗りつぶします。
		//
		void fillBackground(HDC dc, LPCRECT rc)
		{
			fillBackground(dc, rc, hive.fillColor, WP_DIALOG, 0);
		}

		//
		// 矩形を塗りつぶします。
		//
		void fillBackground(HDC dc, LPCRECT rc, COLORREF color, int partId, int stateId)
		{
			// テーマを使用するなら
			if (hive.useTheme)
			{
				// テーマ API を使用してボーダーを描画する。
				::DrawThemeBackground(hive.theme, dc, partId, stateId, rc, 0);
			}
			// テーマを使用しないなら
			else
			{
				// ブラシで塗りつぶす。
				HBRUSH brush = ::CreateSolidBrush(color);
				::FillRect(dc, rc, brush);
				::DeleteObject(brush);
			}
		}
	} painter;
}
