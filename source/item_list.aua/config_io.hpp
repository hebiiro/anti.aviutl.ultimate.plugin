#pragma once

namespace apn::item_list
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct config_io_t : StdConfigIOUseHive<hive>
	{
		//
		// コンフィグが更新されたのでコントロールに適用します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			// ダイアログのコントロールを更新します。
			return addin_dialog.update_controls();
		}

		//
		// リストの列を読み込みます。
		//
		BOOL read_list_column(const n_json& root)
		{
			MY_TRACE_FUNC("");

			// リストビューを取得します。
			auto list_view = addin_dialog.ctrl(idc_list);

			// 列を読み込みます。
			read_child_nodes(root, "column",
				[&](const n_json& column_node, size_t i)
			{
				// インデックスが無効の場合はループを終了します。
				if (i >= list.k_column.k_max_size) return FALSE;

				// 名前を読み込みます。
				std::wstring name;
				read_string(column_node, "name", name);

				// 幅を読み込みます。
				int32_t width = 0;
				read_int(column_node, "width", width);

				// 順番を読み込みます。
				int32_t order = 0;
				read_int(column_node, "order", order);

				// インデックスを読み込みます。
				int32_t sub_item = 0;
				read_int(column_node, "sub_item", sub_item);

				// 列情報をセットします。
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
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_list_column(root);
			read_bool(root, "flag_as_time", hive.flag_as_time);
			read_int(root, "refresh_delay", hive.refresh_delay);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// リストの列を書き込みます。
		//
		BOOL write_list_column(n_json& root)
		{
			MY_TRACE_FUNC("");

			// リストビューを取得します。
			auto list_view = addin_dialog.ctrl(idc_list);

			n_json column_nodes;

			for (auto i = 0; i < list.k_column.k_max_size; i++)
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
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_list_column(root);
			write_bool(root, "flag_as_time", hive.flag_as_time);
			write_int(root, "refresh_delay", hive.refresh_delay);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
