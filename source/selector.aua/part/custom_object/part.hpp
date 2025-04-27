#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはカスタムオブジェクトのパートです。
	//
	inline struct Part : selector::Part
	{
		//
		// このパートの名前を返します。
		//
		virtual std::wstring on_get_name()
		{
			return L"custom_object";
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ウィンドウ初期化処理を実行します。
		//
		virtual BOOL on_window_init() override
		{
			MY_TRACE_FUNC("");

			if (!list_window.init()) return FALSE;
			if (!tree_window.init()) return FALSE;

			// 拡張編集内のカスタムオブジェクトデータを読み込みます。
			exedit_reader.read();

			// カスタムオブジェクトのスペックを読み込みます。
			spec_reader.read();

			return TRUE;
		}

		//
		// ウィンドウ後始末処理を実行します。
		//
		virtual BOOL on_window_exit() override
		{
			MY_TRACE_FUNC("");

			tree_window.exit();
			list_window.exit();

			return TRUE;
		}

		//
		// リストの列を読み込みます。
		//
		BOOL read_list_column(const n_json& root)
		{
			MY_TRACE_FUNC("");

			auto list_view = list_dialog.ctrl(IDC_LIST);

			read_child_nodes(root, "column",
				[&](const n_json& column_node, size_t i)
			{
				if (i >= agit.c_column.c_max_size) return FALSE;

				std::wstring name;
				read_string(column_node, "name", name);

				int32_t width = 0;
				read_int(column_node, "width", width);

				int32_t order = 0;
				read_int(column_node, "order", order);

				int32_t sub_item = 0;
				read_int(column_node, "sub_item", sub_item);

				LVCOLUMN column = {};
				column.mask = LVCF_WIDTH | LVCF_ORDER | LVCF_SUBITEM;
//				column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_ORDER | LVCF_SUBITEM;
				column.pszText = name.data();
				column.cx = width;
				column.iOrder = order;
				column.iSubItem = sub_item;
				ListView_SetColumn(list_view, i, &column);

				return TRUE;
			});

			return TRUE;
		}

		//
		// リストの行を読み込みます。
		//
		BOOL read_list_row(const n_json& root)
		{
			MY_TRACE_FUNC("");

			read_child_nodes(root, "row",
				[&](const n_json& custom_object_node, size_t i)
			{
				// まず、スクリプトIDを読み込みます。
				std::wstring id;
				read_string(custom_object_node, "id", id);

				// スクリプトIDからカスタムオブジェクトを取得します。
				auto it = agit.list.collection.raw.map.find(id);
				if (it == agit.list.collection.raw.map.end()) return TRUE;
				auto& custom_object = it->second;

				// 残りのプロパティを読み込みます。
				read_string(custom_object_node, "note", custom_object->note);

				return TRUE;
			});

			return TRUE;
		}

		//
		// ツリーノードを読み込みます。
		//
		BOOL read_tree_node(const n_json& root, const std::shared_ptr<Agit::Tree::Node>& node)
		{
			if (!node) return FALSE;

			MY_TRACE_FUNC("{/}", node->name);

			read_child_nodes(root, "tree",
				[&](const n_json& child_node, size_t i)
			{
				// まずIDだけを取得します。
				std::wstring id;
				read_string(child_node, "id", id);

				// IDから既存のノードを取得します。
				auto child = agit.tree.get(id);

				// 既存のノードが取得できた場合は
				if (child)
				{
					// モードを読み込みます。
					read_label(child_node, "mode", child->mode, agit.tree.c_mode.labels);

					// 展開状態を読み込みます。
					read_bool(child_node, "expanded", child->expanded);
				}
				// 既存のノードが取得できなかった場合は
				else
				{
					// 残りの属性を読み込みます。

					std::wstring name;
					read_string(child_node, "name", name);

					std::wstring pattern;
					read_string(child_node, "pattern", pattern);

					int32_t target = {};
					read_label(child_node, "target", target, agit.c_column.labels);

					int32_t mode = {};
					read_label(child_node, "mode", mode, agit.tree.c_mode.labels);

					BOOL expanded = FALSE;
					read_bool(child_node, "expanded", expanded);

					// ノードを新規作成します。
					child = agit.tree.add(node, node->vec.end(),
						id, name, pattern, target, mode, expanded);
				}

				// 子ノードを再帰的に読み込みます。
				read_tree_node(child_node, child);

				return TRUE;
			});

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_read(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_list_column(root);
			read_list_row(root);
			read_tree_node(root, agit.tree.root);
			read_window_pos(root, "list_window", list_window);
			read_window_pos(root, "tree_window", tree_window);

			return TRUE;
		}

		//
		// リストの列を書き込みます。
		//
		BOOL write_list_column(n_json& root)
		{
			MY_TRACE_FUNC("");

			auto list_view = list_dialog.ctrl(IDC_LIST);

			n_json column_nodes;

			for (auto i = 0; i < agit.c_column.c_max_size; i++)
			{
				n_json column_node;

				TCHAR name[MAX_PATH] = {};
				LVCOLUMN column = {};
				column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_ORDER | LVCF_SUBITEM;
				column.cchTextMax = std::size(name);
				column.pszText = name;
				ListView_GetColumn(list_view, i, &column);

				write_string(column_node, "name", column.pszText);
				write_int(column_node, "width", column.cx);
				write_int(column_node, "order", column.iOrder);
				write_int(column_node, "sub_item", column.iSubItem);

				column_nodes.emplace_back(column_node);
			}

			write_child_node(root, "column", column_nodes);

			return TRUE;
		}

		//
		// リストの行を書き込みます。
		//
		BOOL write_list_row(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_child_nodes(root, "row", agit.list.collection.raw.vec,
				[&](n_json& custom_object_node, const auto& custom_object, size_t i)
			{
				write_string(custom_object_node, "id", get_script_id(custom_object));
				write_string(custom_object_node, "note", custom_object->note);

				return TRUE;
			});

			return TRUE;
		}

		//
		// ツリーノードを書き込みます。
		//
		BOOL write_tree_node(n_json& root, const std::shared_ptr<Agit::Tree::Node>& node)
		{
			if (!node) return FALSE;

			MY_TRACE_FUNC("{/}", node->name);

			write_child_nodes(root, "tree", node->vec,
				[&](n_json& child_node, const auto& child, size_t i)
			{
				write_string(child_node, "id", child->id);
				write_string(child_node, "name", child->name);
				write_string(child_node, "pattern", child->pattern);
				write_label(child_node, "target", child->target, agit.c_column.labels);
				write_label(child_node, "mode", child->mode, agit.tree.c_mode.labels);
				write_bool(child_node, "expanded", child->expanded);

				write_tree_node(child_node, child);

				return TRUE;
			});

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL on_write(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_list_column(root);
			write_list_row(root);
			write_tree_node(root, agit.tree.root);
			write_window_pos(root, "list_window", list_window);
			write_window_pos(root, "tree_window", tree_window);

			return TRUE;
		}

		//
		// パートの状態を更新します。
		//
		virtual BOOL on_update(AddinDialogInterface* addin_dialog) override
		{
			MY_TRACE_FUNC("");

			// 組み込みのツリーノードを構築します。
			embedded_tree_builder.build();

			// カスタムオブジェクトのファイルタイムを更新します。
			agit.update_file_time();

			// カスタムオブジェクトのリストを絞り込みます。
			agit.filter_list();

			// ダイアログの絞り込み用コンボボックスを更新します。
			list_dialog.update_filter_comboboxs();

			// ダイアログのリストビューを更新します。
			list_dialog.update_list_view();

			// ダイアログのツリービューを更新します。
			tree_dialog.update_tree_view();

			return TRUE;
		}

		//
		// コマンドを実行します。
		//
		virtual BOOL on_command(AddinDialogInterface* addin_dialog, uint32_t code, uint32_t control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			switch (control_id)
			{
			case IDC_SHOW_CUSTOM_OBJECT_LIST: ::ShowWindow(list_window, SW_SHOW); break;
			case IDC_SHOW_CUSTOM_OBJECT_TREE: ::ShowWindow(tree_window, SW_SHOW); break;
			}

			return TRUE;
		}

		//
		// オプションを変更します。
		//
		virtual BOOL on_option(AddinDialogInterface* addin_dialog, uint32_t control_id, HWND control, const POINT& point) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}", control_id, control);

			return TRUE;
		}
	} part;
}
