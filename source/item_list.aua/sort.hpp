#pragma once

namespace apn::item_list
{
	//
	// このクラスはアイテム一覧をソートします。
	//
	inline struct sort_t
	{
		//
		// このクラスはソートノードです。
		//
		struct node_t
		{
			//
			// TRUEの場合は昇順にソートします。
			//
			BOOL ascendant = FALSE;

			//
			// 列インデックスです。
			//
			int32_t col_index = -1;
		};

		//
		// ソートノードのデックです。
		//
		std::deque<std::shared_ptr<node_t>> nodes;

		//
		// ソートノードを追加します。
		//
		void add(int32_t col_index)
		{
			// ソート順です。
			BOOL ascendant = FALSE;

			// 既存のソートノードを走査します。
			for (auto it = nodes.begin(); it != nodes.end(); it++)
			{
				// ソートノードを取得します。
				const auto& node = *it;

				// 列インデックスが一致する場合は
				if (node->col_index == col_index)
				{
					// ソート順を反転します。
					ascendant = !node->ascendant;

					// 一旦デックから削除します。
					nodes.erase(it);

					// ループを終了します。
					break;
				}
			}

			// ソートノードが多すぎる場合は末尾のノードを削除します。
			if (nodes.size() >= 3) nodes.pop_back();

			// ソートノードを新規作成してデックに追加します。
			nodes.emplace_front(std::make_shared<node_t>(ascendant, col_index));
		}

		//
		// アイテム一覧のリストをソートします。
		//
		BOOL apply()
		{
			//
			// この関数は、リストノードを比較します。
			//
			constexpr auto compare = [](const auto& a, const auto& b, int32_t col_index)
			{
				// ノードの文字列を取得します。
				const auto& a_str = a->get_text(col_index).c_str();
				const auto& b_str = b->get_text(col_index).c_str();

				// 列が数値型の場合は
				if (list.k_column.k_datas[col_index].flag_number)
				{
					// 数値化します。
					auto a_num = (int)wcstol(a_str, nullptr, 0);
					auto b_num = (int)wcstol(b_str, nullptr, 0);

					// 数値として比較します。
					if (a_num != b_num) return a_num - b_num;
				}

				// 文字列として比較します。
				return ::lstrcmpW(a_str, b_str);
			};

			// アイテム一覧のリストをソートします。
			std::sort(list.nodes.begin(), list.nodes.end(),
				[&](const auto& a, const auto& b) -> bool
			{
				for (const auto& node : sort.nodes)
				{
					if (node->ascendant)
					{
						if (auto result = compare(b, a, node->col_index))
							return result < 0;
					}
					else
					{
						if (auto result = compare(a, b, node->col_index))
							return result < 0;
					}
				}

				return a->orig_index < b->orig_index;
			});

			return TRUE;
		}

		//
		// アイテム一覧のリストを元の並びに戻します。
		//
		BOOL reset()
		{
			// アイテム一覧のリストをソートします。
			std::sort(list.nodes.begin(), list.nodes.end(),
				[&](const auto& a, const auto& b) -> bool
			{
				return a->orig_index < b->orig_index;
			});

			return TRUE;
		}
	} sort;
}
