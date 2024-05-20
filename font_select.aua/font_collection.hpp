#pragma once

namespace apn::font_select
{
	//
	// このクラスはフォントのコレクションです。
	//
	struct FontCollection
	{
		std::vector<std::wstring> collection;

		//
		// コンストラクタです。
		//
		FontCollection(HWND combobox)
		{
			auto c = (int)::SendMessageW(combobox, CB_GETCOUNT, 0, 0);
			for (decltype(c) i = 0; i < c; i++)
			{
				collection.emplace_back(my::get_text_from_combobox(combobox, i));
			}
		}
	};
}
