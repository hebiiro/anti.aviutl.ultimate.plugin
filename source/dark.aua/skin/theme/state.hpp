#pragma once

namespace apn::dark::skin::theme
{
	//
	// このクラスはビジュアルスタイルのステートです。
	//
	struct State
	{
		//
		// このステートに関連付けられているスタッフです。
		//
		Stuff stuff;

		//
		// 塗りつぶし用のブラシです。
		//
		my::gdi::unique_ptr<HBRUSH> fill_brush;

		//
		// 塗りつぶし用のブラシを返します。
		//
		HBRUSH get_fill_brush()
		{
			if (!fill_brush && stuff.fill.color != CLR_NONE)
				fill_brush.reset(::CreateSolidBrush(stuff.fill.color));
			return fill_brush.get();
		}

		//
		// DCに塗りつぶし設定を適用します。
		//
		HBRUSH apply_fill(HDC dc, HBRUSH default_brush)
		{
			if (stuff.fill.color == CLR_NONE) return default_brush;
			::SetBkColor(dc, stuff.fill.color);
			if (!fill_brush) fill_brush.reset(::CreateSolidBrush(stuff.fill.color));
			return fill_brush.get();
		}
	};
}
