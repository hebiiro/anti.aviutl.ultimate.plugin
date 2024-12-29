#pragma once

namespace apn::font_select
{
	//
	// フォントコンボボックスからフォント名のコレクションを作成して返します。
	//
	inline std::vector<std::wstring> create_font_collection(HWND font_combobox)
	{
		std::vector<std::wstring> collection;
		auto c = (int32_t)::SendMessageW(font_combobox, CB_GETCOUNT, 0, 0);
		for (decltype(c) i = 0; i < c; i++)
			collection.emplace_back(my::get_text_from_combobox(font_combobox, i));
		return collection;
	}

	//
	// メニューデータからメニューを作成して返します。
	//
	inline HMENU create_menu(const Hive::Menu::Node& node)
	{
		auto menu = ::CreatePopupMenu();

		for (const auto& sub_node : node.nodes)
		{
			// サブメニューの場合は
			if (sub_node.flags & MF_POPUP)
			{
				// サブメニューを作成します。
				if (auto sub_menu = create_menu(sub_node))
				{
					// サブメニューをメニューに追加します。
					::AppendMenuW(menu, sub_node.flags, (UINT_PTR)sub_menu, sub_node.name.c_str());
				}
			}
			// サブメニューではない場合は
			else
			{
				// アイテムをメニューに追加します。
				::AppendMenuW(menu, sub_node.flags, sub_node.command_id, sub_node.name.c_str());
			}
		}

		return menu;
	}
}
