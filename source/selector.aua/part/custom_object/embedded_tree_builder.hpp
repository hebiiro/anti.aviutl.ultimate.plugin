#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスは組み込みのツリーノードを構築します。
	//
	inline struct EmbededTreeBuilder
	{
		//
		// 文字列を分割して配列にして返します。
		//
		inline static std::vector<std::wstring> split(const std::wstring& str, wchar_t delimiter = L' ')
		{
			std::vector<std::wstring> vec;
			std::wstringstream ss(str);
			std::wstring buffer;
			while (std::getline(ss, buffer, delimiter)) vec.emplace_back(buffer);
			return vec;
		}

		//
		// 文字列をエスケープして返します。
		//
		inline static std::wstring escape(const std::wstring& str)
		{
			std::wstring buffer;
			for (auto ch : str) {
				switch (ch) {
				case L'\\':
				case L'(':
				case L')':
				case L'{':
				case L'}':
				case L'[':
				case L']':
				case L'^':
				case L'$':
				case L'.':
				case L'*':
				case L'+':
				case L'?':
				case L'|':
					buffer += L'\\'; break;
				}
				buffer += ch;
			}
			return buffer;
		}

		//
		// IDを作成して返します。
		//
		inline static std::wstring join_id(const std::wstring& parent_id, const std::wstring& name)
		{
			return parent_id + L"/" + name;
		}

		//
		// デフォルトのツリーノードを構築します。
		//
		BOOL build()
		{
			MY_TRACE_FUNC("");

			// ルートノードの名前です。
			auto root_name = agit.tree.root->name;

			// ルートノードのIDです。
			auto root_id = root_name;

			// ルートノードを作成します。
			auto root_node = agit.tree.root;

			{
				// 『作者』ノードの名前です。
				auto authors_name = std::wstring(L"作者");

				// 『作者』ノードのIDです。
				auto authors_id = join_id(root_id, authors_name);

				// 『作者』ノードを作成します。
				auto authors_node = agit.tree.add(root_node, root_node->vec.end(),
					authors_id, authors_name, L"", agit.c_column.c_author, agit.tree.c_mode.c_on);

				// 作者名のコレクションを作成します。
				std::set<std::wstring> authors;
				for (const auto& node : agit.list.collection.raw.vec)
					authors.emplace(node->author);

				// 作者名のコレクションを走査します。
				for (const auto& author : authors)
				{
					// 作者名が空の場合は何もしません。
					if (author.empty()) continue;

					// 作者名のIDです。
					auto id = join_id(authors_id, author);

					// 作者検索用のパターンです。
					auto pattern = L"^" + escape(author) + L"$";

					// 作者名のノードを追加します。
					agit.tree.add(authors_node, authors_node->vec.end(),
						id, author, pattern, agit.c_column.c_author, agit.tree.c_mode.c_off);
				}
			}

			{
				// 『タグ』ノードの名前です。
				auto tags_name = std::wstring(L"タグ");

				// 『タグ』ノードのIDです。
				auto tags_id = join_id(root_id, tags_name);

				// 『タグ』ノードを作成します。
				auto tags_node = agit.tree.add(root_node, root_node->vec.end(),
					tags_id, tags_name, L"", agit.c_column.c_tag, agit.tree.c_mode.c_on);

				// タグ名のコレクションを作成します。
				std::set<std::wstring> tags;
				for (const auto& node : agit.list.collection.raw.vec)
				{
					auto tag_vec = split(node->tag);
					for (const auto& tag : tag_vec)
						tags.emplace(tag);
				}

				// タグ名のコレクションを走査します。
				for (const auto& tag : tags)
				{
					// タグ名が空の場合は何もしません。
					if (tag.empty()) continue;

					// タグ名のIDです。
					auto id = join_id(tags_id, tag);

					// タグ検索用のパターンです。
					// ※L"\\b"は日本語に対応していないので使用できません。
					auto pattern = tag + L"(\\s|$)";

					// タグ名のノードを追加します。
					agit.tree.add(tags_node, tags_node->vec.end(),
						id, tag, pattern, agit.c_column.c_tag, agit.tree.c_mode.c_off);
				}
			}

			return TRUE;
		}
	} embedded_tree_builder;
}
