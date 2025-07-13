#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはカスタムオブジェクト関連の変数を保持します。
	//
	inline struct Agit
	{
		inline static constexpr struct Column {
			inline static constexpr int32_t c_script_name = 0;
			inline static constexpr int32_t c_folder_name = 1;
			inline static constexpr int32_t c_file_name = 2;
			inline static constexpr int32_t c_file_time = 3;
			inline static constexpr int32_t c_author = 4;
			inline static constexpr int32_t c_tag = 5;
			inline static constexpr int32_t c_desc = 6;
			inline static constexpr int32_t c_note = 7;
			inline static constexpr int32_t c_max_size = 8;
			inline static constexpr my::Label labels[] = {
				{ c_script_name, L"script_name" },
				{ c_folder_name, L"folder_name" },
				{ c_file_name, L"file_name" },
				{ c_file_time, L"file_time" },
				{ c_author, L"author" },
				{ c_tag, L"tag" },
				{ c_desc, L"desc" },
				{ c_note, L"note" },
			};
			struct Data { LPCWSTR text; int32_t width; };
			inline static constexpr Data c_datas[] = {
				{ L"スクリプト名", 100 },
				{ L"フォルダ名", 100 },
				{ L"ファイル名", 100 },
				{ L"ファイルタイム", 100 },
				{ L"作者", 100 },
				{ L"タグ", 100 },
				{ L"説明", 200 },
				{ L"ノート", 200 },
			};
		} c_column;

		//
		// このクラスはカスタムオブジェクトのリストです。
		//
		struct List {
			//
			// このクラスはリストノードです。
			//
			struct Node {
				int32_t orig_index = -1;
				std::wstring script_name;
				std::wstring folder_name;
				std::wstring file_name;
				std::wstring file_time;
				std::wstring author;
				std::wstring tag;
				std::wstring desc;
				std::wstring note;
				std::wstring distributor;

				//
				// 指定された列に対応する文字列を返します。
				//
				inline std::wstring& get_text(int32_t col_index)
				{
					switch (col_index)
					{
					default:
					case c_column.c_script_name: return script_name;
					case c_column.c_folder_name: return folder_name;
					case c_column.c_file_name: return file_name;
					case c_column.c_file_time: return file_time;
					case c_column.c_author: return author;
					case c_column.c_tag: return tag;
					case c_column.c_desc: return desc;
					case c_column.c_note: return note;
					}
				}

				//
				// このカスタムオブジェクトが導入済みの場合はTRUEを返します。
				//
				inline BOOL is_installed()
				{
					return file_time.length() != 0;
				}
			};

			//
			// リストノードのコレクションです。
			//
			struct {
				struct {
					std::vector<std::shared_ptr<Node>> vec;
					std::unordered_map<std::wstring, std::shared_ptr<Node>> map;
				} raw;
				struct {
					std::vector<std::shared_ptr<Node>> vec;
				} filtered;
			} collection;

			//
			// カスタムオブジェクトをコレクションに追加します。
			//
			auto add(const auto& custom_object)
			{
				// カスタムオブジェクトの識別名を取得します。
				auto id = get_script_id(custom_object);
				MY_TRACE_STR(id);

				// 同じカスタムオブジェクトが存在する場合は
				auto it = collection.raw.map.find(id);
				if (it != collection.raw.map.end())
				{
					// 既存のカスタムオブジェクトを返します。
					return it->second;
				}
				// 同じカスタムオブジェクトが存在しない場合は
				else
				{
					// カスタムオブジェクトをコレクションに追加します。
					custom_object->orig_index = (int32_t)collection.raw.vec.size();
					collection.raw.vec.emplace_back(custom_object);
					collection.raw.map[id] = custom_object;

					// コレクションに追加されたカスタムオブジェクトを返します。
					return custom_object;
				}
			}
		} list;

		//
		// このクラスはカスタムオブジェクトのツリーです。
		//
		struct Tree {
			inline static constexpr struct Mode {
				inline static constexpr int32_t c_off = 0;
				inline static constexpr int32_t c_on = 1;
				inline static constexpr int32_t c_exclusion = 2;
				inline static constexpr my::Label labels[] = {
					{ c_off, L"off" },
					{ c_on, L"on" },
					{ c_exclusion, L"exclusion" },
				};
			} c_mode;

			//
			// このクラスはツリーノードです。
			//
			struct Node {
				std::wstring id;
				std::wstring name;
				std::wstring pattern;
				int32_t target = c_column.c_tag;
				int32_t mode = c_mode.c_off;
				BOOL expanded = FALSE;

				//
				// 親ノードです。
				//
				std::weak_ptr<Node> parent;

				//
				// 子ノードのコレクションです。
				//
				std::vector<std::shared_ptr<Node>> vec;
			};

			//
			// ツリーノードのルートです。
			//
			std::shared_ptr<Node> root;

			//
			// ツリーノードのマップです。
			// IDからツリーノードを取得できるようにします。
			//
			std::unordered_map<std::wstring, std::shared_ptr<Node>> map;

			//
			// コンストラクタです。
			//
			Tree()
			{
				// ルートノードを作成します。
				root = std::make_shared<Node>(L"ルート", L"", L"", c_column.c_tag, c_mode.c_on, TRUE);
			}

			//
			// 指定されたツリーノードを追加します。
			//
			std::shared_ptr<Node> add(
				const std::shared_ptr<Node>& parent,
				std::vector<std::shared_ptr<Node>>::iterator insert_pos,
				const std::shared_ptr<Node>& node)
			{
				// 同一IDのノードが存在すればそれを返します。
				auto it = map.find(node->id);
				if (it != map.end()) return it->second;

				// ノードのIDが有効の場合はマップにも追加します。
				if (node->id.length()) map[node->id] = node;

				// 親ノードが有効の場合は
				if (parent)
				{
					// 親ノードをセットします。
					node->parent = parent;

					// 親ノードのコレクションの指定された位置に追加します。
					return *parent->vec.insert(insert_pos, node);
				}
				// 親ノードが無効の場合は
				else
				{
					// ルートノードに設定します。
					return agit.tree.root = node;
				}
			}

			//
			// ツリーノードを新規作成して返します。
			// 親ノードのコレクションにも追加します。
			//
			std::shared_ptr<Node> add(
				const std::shared_ptr<Node>& parent,
				std::vector<std::shared_ptr<Node>>::iterator insert_pos,
				const std::wstring& id,
				const std::wstring& name,
				const std::wstring& pattern,
				int32_t target,
				int32_t mode = c_mode.c_off,
				BOOL expanded = FALSE)
			{
				return add(parent, insert_pos, std::make_shared<Node>(id, name, pattern, target, mode, expanded));
			}

			//
			// ツリーノードを親ノードから切り離します。
			//
			BOOL erase(const std::shared_ptr<Node>& node)
			{
				if (auto parent = node->parent.lock())
				{
					auto it = std::find(parent->vec.begin(), parent->vec.end(), node);
					if (it != parent->vec.end())
						return parent->vec.erase(it), TRUE;
				}

				return FALSE;
			}

			//
			// 指定されたIDのツリーノードを返します。
			//
			std::shared_ptr<Node> get(const std::wstring& id)
			{
				auto it = map.find(id);
				if (it == map.end()) return {};
				return it->second;
			}
		} tree;

		//
		// このクラスは組み込みスクリプトを管理します。
		//
		struct Embedded {
			//
			// このクラスは組み込みスクリプトのノードです。
			//
			struct Node {
				int32_t index;
				std::wstring name;
			};

			//
			// 組み込みスクリプトのノードのコレクションです。
			// キーはスクリプト名です。
			//
			std::unordered_map<std::wstring, Node> map;
		} embedded;

		//
		// このクラスはカスタムオブジェクトをソートします。
		//
		struct Sort {
			//
			// このクラスはソート用ノードです。
			//
			struct Node {
				BOOL ascendant = FALSE;
				int32_t col_index = -1;
			};

			//
			// ソート用ノードのデックです。
			//
			std::deque<std::shared_ptr<Node>> nodes;

			//
			// ソート用ノードを追加します。
			//
			void add(int32_t col_index)
			{
				// ソート順です。
				BOOL ascendant = FALSE;

				// 既存のノードを走査します。
				for (auto it = nodes.begin(); it != nodes.end(); it++)
				{
					// ノードを取得します。
					const auto& node = *it;

					// 列インデックスが一致する場合は
					if (node->col_index == col_index)
					{
						// ソート順を反転します。
						ascendant = !node->ascendant;

						// 一旦デックから削除します。
						nodes.erase(it);

						break;
					}
				}

				// ソート用ノードが多すぎる場合は末尾のノードを削除します。
				if (nodes.size() >= 3) nodes.pop_back();

				// ノードを新規作成してデックに追加します。
				nodes.emplace_front(std::make_shared<Node>(ascendant, col_index));
			}
		} sort;

		//
		// このクラスはカスタムオブジェクトを絞り込みます。
		//
		struct Filter {
			std::wstring texts[c_column.c_max_size];
		} filter;

		//
		// 指定されたカスタムオブジェクトが組み込みスクリプトの場合はTRUEを返します。
		//
		inline BOOL is_embedded(const auto& custom_object)
		{
			return embedded.map.contains(custom_object->script_name);
		}

		//
		// すべてのカスタムオブジェクトのファイルタイムを更新します。
		//
		BOOL update_file_time()
		{
			// カスタムオブジェクトのリストを取得します。
			auto& vec = list.collection.raw.vec;

			// カスタムオブジェクトを走査します。
			for (auto& custom_object : vec)
			{
				// カスタムオブジェクトが組み込みスクリプトの場合は
				if (is_embedded(custom_object))
				{
					// カスタムオブジェクトを組み込みスクリプトとしてマークします。
					custom_object->file_time = L"組み込み";
				}
				// カスタムオブジェクトが通常のカスタムオブジェクトの場合は
				else
				{
					// ファイルタイムを取得します。
					custom_object->file_time = get_file_time_string(
						get_script_path(custom_object, L".obj"));
				}
			}

			return TRUE;
		}

		//
		// カスタムオブジェクトを比較します。
		//
		inline static int32_t compare(const auto& a, const auto& b, int32_t col_index)
		{
			return ::lstrcmpW(a->get_text(col_index).c_str(), b->get_text(col_index).c_str());
		}

		//
		// カスタムオブジェクトのリストを元の並びに戻します。
		//
		BOOL unsort_list()
		{
			// カスタムオブジェクトのリストを取得します。
			auto& vec = list.collection.raw.vec;

			// カスタムオブジェクトのリストをソートします。
			std::sort(vec.begin(), vec.end(),
				[&](const auto& a, const auto& b) -> bool
			{
				return a->orig_index < b->orig_index;
			});

			return TRUE;
		}

		//
		// カスタムオブジェクトのリストをソートします。
		//
		BOOL sort_list()
		{
			// カスタムオブジェクトのリストを取得します。
			auto& vec = list.collection.raw.vec;

			// カスタムオブジェクトのリストをソートします。
			std::sort(vec.begin(), vec.end(),
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
		// 指定されたカスタムオブジェクトが絞り込み対象の場合はTRUEを返します。
		//
		BOOL is_filter_target(const std::shared_ptr<List::Node>& custom_object, const std::shared_ptr<Tree::Node>& filter_node)
		{
			// フィルタノードが無効の場合は絞り込み対象外です。
			if (!filter_node) return FALSE;

			// モードがオフの場合は絞り込み対象外です。
			if (filter_node->mode == tree.c_mode.c_off) return FALSE;

			// フィルタノードのパターンが有効の場合は
			if (filter_node->pattern.length())
			{
				// 正規表現を準備します。
				std::wregex reg(filter_node->pattern);
				std::wsmatch results;

				// マッチ対象の文字列を取得します。
				const auto& target = custom_object->get_text(filter_node->target);

				// 除外モードの場合は
				if (filter_node->mode == tree.c_mode.c_exclusion)
				{
					// パターンにマッチした場合はTRUEを返します。
					if (std::regex_search(target, results, reg)) return TRUE;
				}
				// 通常モードの場合は
				else
				{
					// パターンにマッチしなかった場合はTRUEを返します。
					if (!std::regex_search(target, results, reg)) return TRUE;
				}
			}

			// フィルタノードの子ノードを走査します。
			for (const auto& child_node : filter_node->vec)
			{
				// 指定されたカスタムオブジェクトが絞り込み対象の場合は
				if (is_filter_target(custom_object, child_node))
					return TRUE; // TRUEを返します。
			}

			// 指定されたカスタムオブジェクトは絞り込み対象外なので
			return FALSE; // FALSEを返します。
		}

		//
		// 指定されたカスタムオブジェクトが絞り込み対象の場合はTRUEを返します。
		//
		BOOL is_filter_target(const std::shared_ptr<List::Node>& custom_object)
		{
			// フィルタ文字列を走査します。
			for (size_t i = 0; i < std::size(filter.texts); i++)
			{
				// フィルタ文字列を取得します。
				const auto& text = filter.texts[i];
				if (text.empty()) continue;

				// 指定されたカスタムオブジェクト内に
				// フィルタ文字列が見つからなかった場合は
				if (custom_object->get_text((int32_t)i).find(text) == text.npos)
					return TRUE; // TRUEを返します。
			}

			// 指定されたカスタムオブジェクトは絞り込み対象外なので
			return FALSE; // FALSEを返します。
		}

		//
		// カスタムオブジェクトのリストを絞り込みます。
		// (フィルタリングされたコレクションを別途構築します)
		//
		BOOL filter_list()
		{
			// 一旦フィルタ済みコレクションをリセットします。
			list.collection.filtered.vec.clear();

			// すべてのカスタムオブジェクトを走査します。
			for (const auto& custom_object : list.collection.raw.vec)
			{
				// カスタムオブジェクトが絞り込み対象の場合は何もしません。
				if (is_filter_target(custom_object)) continue;

				// カスタムオブジェクトが絞り込み対象の場合は何もしません。
				if (is_filter_target(custom_object, tree.root)) continue;

				// カスタムオブジェクトをフィルタ済みコレクションに追加します。
				list.collection.filtered.vec.emplace_back(custom_object);
			}

			return TRUE;
		}
	} agit;
}
