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
		// コンボボックス内の指定されたアイテムのテキストを返します。
		//
		static std::wstring get_text_from_combobox(HWND combobox, int index)
		{
			// 指定されたアイテムのテキストの長さを取得します。
			auto text_length = (int)::SendMessageW(combobox, CB_GETLBTEXTLEN, index, 0);
			if (text_length <= 0) return L"";

			// 指定されたアイテムのテキストを取得します。
			std::wstring text(text_length, L'\0');
			::SendMessageW(combobox, CB_GETLBTEXT, index, (LPARAM)text.data());

			return text;
		}

		//
		// コンストラクタです。
		//
		FontCollection(HWND combobox)
		{
			auto c = (int)::SendMessageW(combobox, CB_GETCOUNT, 0, 0);
			for (decltype(c) i = 0; i < c; i++)
			{
				collection.emplace_back(get_text_from_combobox(combobox, i));
			}
		}
	};
}
