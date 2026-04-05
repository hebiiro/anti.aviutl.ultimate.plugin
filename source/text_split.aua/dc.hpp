#pragma once

namespace apn::text_split
{
	//
	// このクラスはDCを管理します。
	//
	struct dc_t : my::ClientDC
	{
		my::gdi::unique_ptr<HFONT> font;
		my::gdi::selector font_selector;

		//
		// コンストラクタです。
		//
		dc_t(HWND hwnd, text_object_t& text_object)
			: my::ClientDC(hwnd)
			, font(::CreateFontA(-text_object.font.size, 0, 0, 0,
				text_object.font.bold ? FW_BOLD : FW_NORMAL, text_object.font.italic, FALSE, FALSE,
				DEFAULT_CHARSET, 0, 0, 0, 0, text_object.font.name.c_str()))
			, font_selector(dc, font.get())
		{
			::SetTextCharacterExtra(dc, text_object.font.spacing.x);
		}
	};
}
