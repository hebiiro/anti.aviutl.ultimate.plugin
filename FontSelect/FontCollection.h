#pragma once
#include "Hive.h"

namespace fgo::font_select
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
		static std::wstring getTextFromComboBox(HWND combobox, int index)
		{
			// 指定されたアイテムのテキストの長さを取得します。
			int textLength = ::SendMessageW(combobox, CB_GETLBTEXTLEN, index, 0);
			if (textLength <= 0) return L"";

			// 指定されたアイテムのテキストを取得します。
			std::wstring text(textLength, L'\0');
			::SendMessageW(combobox, CB_GETLBTEXT, index, (LPARAM)&text[0]);

			return text;
		}

		//
		// コンストラクタです。
		//
		FontCollection(HWND combobox)
		{
			int c = (int)::SendMessageW(combobox, CB_GETCOUNT, 0, 0);
			for (int i = 0; i < c; i++)
			{
				collection.emplace_back(getTextFromComboBox(combobox, i));
			}
		}
	};
}
