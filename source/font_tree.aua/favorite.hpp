#pragma once

namespace apn::font_tree
{
	//
	// このクラスはお気に入りフォントを管理するツリーコントロールです。
	//
	inline struct FavoriteManager : my::Window
	{
		struct Node {
			std::wstring name;
			std::wstring alias;
			std::wstring display_name;
			BOOL expanded = FALSE;

			void update_display_name()
			{
				// 別名が有効の場合は
				if (alias.length())
				{
					// 実名が有効の場合は
					if (name.length())
					{
						// 別名と実名の組み合わせを表示名にします。
						display_name = my::format(hive.display_name_format, alias, name);
					}
					// 実名が無効の場合は
					else
					{
						// 別名を表示名にします。
						display_name = alias;
					}
				}
				// 別名が無効の場合は
				else
				{
					// 実名が有効の場合は
					if (name.length())
					{
						// 実名を表示名にします。
						display_name = name;
					}
					// 実名が無効の場合は
					else
					{
						// セパレータを表示名にします。
						display_name = hive.separator_format;
					}
				}

				MY_TRACE_STR(name);
				MY_TRACE_STR(alias);
				MY_TRACE_STR(display_name);
			}
		};

		//
		// ノードのコレクションです。
		//
		std::unordered_map<HTREEITEM, std::shared_ptr<Node>> nodes;

		//
		// ノードを新規作成します。
		//
		std::shared_ptr<Node> create_node()
		{
			return std::make_shared<Node>();
		}

		//
		// すべての要素を消去します。
		//
		void clear()
		{
			TreeView_DeleteAllItems(*this);
			nodes.clear();
		}

		//
		// 指定された要素に該当するノードを返します。
		//
		std::shared_ptr<Node> get_node(HTREEITEM item_handle)
		{
			auto it = nodes.find(item_handle);
			if (it == nodes.end()) return nullptr;
			return it->second;
		}

		//
		// ノードを追加します。
		//
		HTREEITEM insert(HTREEITEM parent_handle, HTREEITEM insert_after, const std::shared_ptr<Node>& node)
		{
			// ノードの表示名を更新します。
			node->update_display_name();

			// ツリービューに追加します。同時にコレクションにも追加します。
			TV_INSERTSTRUCT tv = {};
			tv.hParent = parent_handle;
			tv.hInsertAfter = insert_after;
			tv.item.mask = TVIF_TEXT;
			tv.item.pszText = node->display_name.data();
			auto item_handle = TreeView_InsertItem(*this, &tv);
			nodes[item_handle] = node;
			return item_handle;
		}

		//
		// ノードを削除します。
		//
		void erase(HTREEITEM item_handle)
		{
			TreeView_DeleteItem(*this, item_handle);
			nodes.erase(item_handle);
		}

		//
		// ノードを更新します。
		//
		void update(HTREEITEM item_handle, const std::shared_ptr<Node>& node)
		{
			// ノードの表示名を更新します。
			node->update_display_name();

			// ツリービューのアイテム名も更新します。
			TV_ITEM tv = {};
			tv.mask = TVIF_TEXT;
			tv.pszText = node->display_name.data();
			tv.hItem = item_handle;
			TreeView_SetItem(*this, &tv);
		}

		//
		// ノードを展開します。
		//
		void expand(HTREEITEM parent_handle)
		{
			// ツリービュー内の親アイテムから最初の子アイテムのハンドルを取得します。
			auto item_handle = TreeView_GetChild(*this, parent_handle);

			// 子アイテムを走査します。
			while (item_handle)
			{
				// 子アイテムに該当するノードが存在する場合は
				auto it = nodes.find(item_handle);
				if (it != nodes.end())
				{
					// ノードを取得します。
					auto& node = it->second;
					MY_TRACE("{/}, {/}, {/}, {/}\n", node->name, node->alias, node->display_name, node->expanded);

					// ノードが展開中の場合は
					if (node->expanded)
					{
						// ツリービュー内のアイテムを展開します。
						auto result = TreeView_Expand(*this, item_handle, TVE_EXPAND);
						MY_TRACE_INT(result);
					}

					// 再帰的に子アイテムを展開します。
					expand(item_handle);
				}

				// ツリービュー内の次の子アイテムのハンドルを取得します。
				item_handle = TreeView_GetNextSibling(*this, item_handle);
			}
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("{/hex}", parent);

			// ツリーコントロールを作成します。
			return create(
				0,
//				WS_EX_CLIENTEDGE,
				WC_TREEVIEW,
				_T("favorite"),
				WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | WS_BORDER |
				TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_FULLROWSELECT,
//				TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS | TVS_HASLINES,
				0, 0, 100, 100,
				parent, nullptr, hive.instance, nullptr);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// コンテキストメニューを表示します。
		//
		BOOL show_context_menu(const POINT& point)
		{
			MY_TRACE_FUNC("");

			// 選択要素を取得します。
			auto item_handle = TreeView_GetSelection(*this);
			MY_TRACE_HEX(item_handle);

			// 選択要素に該当するノードを取得します。
			auto node = get_node(item_handle);
			if (!node) return FALSE;

			// フォントコンボボックスを取得します。
			auto font_combobox = magi.exin.get_font_combobox();

			// フォントコンボボックスからフォント名を取得します。
			auto font_name = cb::get_text(font_combobox, -1);

			// メニューアイテムのIDです。
			constexpr uint32_t c_insert = 1;
			constexpr uint32_t c_erase = 2;
			constexpr uint32_t c_replace_name = 3;
			constexpr uint32_t c_erase_name = 4;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			::AppendMenu(menu.get(), MF_STRING, c_insert,
				my::format(_T("選択中の\"{/}\"の子要素として\"{/}\"を追加"), node->display_name, font_name).c_str());
			::AppendMenu(menu.get(), MF_STRING, c_erase,
				my::format(_T("選択中の\"{/}\"を削除"), node->display_name).c_str());
			::AppendMenu(menu.get(), MF_STRING, c_replace_name,
				my::format(_T("選択中の\"{/}\"のフォント名を\"{/}\"で置き換える"), node->display_name, font_name).c_str());
			::AppendMenu(menu.get(), MF_STRING, c_erase_name,
				my::format(_T("選択中の\"{/}\"のフォント名を消去する"), node->display_name).c_str());

			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, hwnd, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_insert:
				{
					auto node = std::make_shared<Node>();
					node->name = font_name.c_str();
					insert(item_handle, nullptr, node);
					TreeView_Expand(*this, item_handle, TVE_EXPAND);

					my::invalidate(*this, nullptr, TRUE);

					break;
				}
			case c_erase:
				{
					erase(item_handle);

					my::invalidate(*this, nullptr, TRUE);

					break;
				}
			case c_replace_name:
				{
					node->name = font_name.c_str();
					update(item_handle, node);

					my::invalidate(*this, nullptr, TRUE);

					break;
				}
			case c_erase_name:
				{
					node->name = L"";
					update(item_handle, node);

					my::invalidate(*this, nullptr, TRUE);

					break;
				}
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// お気に入りを読み込みます。
		//
		void read(const n_json& node)
		{
			MY_TRACE_FUNC("");

			n_json favorite_node;
			read_child_node(node, "favorite", favorite_node);
			read_favorites(favorite_node, TVI_ROOT);
		}

		//
		// 再帰的にお気に入りを読み込みます。
		//
		void read_favorites(const n_json& node, HTREEITEM parent_handle)
		{
			MY_TRACE_FUNC("");

			// お気に入りを読み込みます。
			read_child_nodes(node, "favorite",
				[&](const n_json& favorite_node, size_t i)
			{
				// ノードを新規作成します。
				auto node = create_node();

				// ノードの属性を読み込みます。
				read_string(favorite_node, "name", node->name);
				read_string(favorite_node, "alias", node->alias);
				read_bool(favorite_node, "expanded", node->expanded);

				// 子要素を追加します。
				auto item_handle = insert(parent_handle, TVI_LAST, node);

				// 再帰的に子要素を読み込みます。
				read_favorites(favorite_node, item_handle);

				return TRUE;
			});
		}

		//
		// お気に入りを書き込みます。
		//
		void write(n_json& node)
		{
			MY_TRACE_FUNC("");

			n_json favorite_node;
			write_favorites(favorite_node, TVI_ROOT);
			write_child_node(node, "favorite", favorite_node);
		}

		//
		// 再帰的にお気に入りを書き込みます。
		//
		void write_favorites(n_json& node, HTREEITEM parent_handle)
		{
			MY_TRACE_FUNC("");

			// まずツリー要素のコレクションを作成します。
			std::vector<HTREEITEM> favorites;
			{
				// 最初の子要素を取得します。
				auto item_handle = TreeView_GetChild(*this, parent_handle);
				while (item_handle)
				{
					// コレクションに追加します。
					favorites.emplace_back(item_handle);

					// 次の子要素を取得します。
					item_handle = TreeView_GetNextSibling(*this, item_handle);
				}
			}

			// お気に入りを書き込みます。
			write_child_nodes(node, "favorite", favorites,
				[&](n_json& favorite_node, const auto& favorite, size_t i)
			{
				// ツリー要素に該当するノードを取得します。
				const auto& node = get_node(favorite);
				if (!node) return TRUE;

				// ノードの属性を書き込みます。
				write_string(favorite_node, "name", node->name);
				write_string(favorite_node, "alias", node->alias);
				write_bool(favorite_node, "expanded", node->expanded);

				// 再帰的にお気に入りを書き込みます。
				write_favorites(favorite_node, favorite);

				return TRUE;
			});
		}
	} favorite_manager;
}
