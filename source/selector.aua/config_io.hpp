#pragma once

namespace apn::selector
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
		//
		// コンフィグが更新されたのでコントロールに適用します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			// コンフィグが更新されたのでパートも更新します。
			for (auto& part : Part::parts) part->on_update(&addin_dialog);

			return addin_dialog.update_controls();
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// パートの読み込み処理を実行します。
			for (auto& part : Part::parts)
			{
				n_json node;
				read_child_node(root, my::wide_to_cp(part->on_get_name(), CP_UTF8), node);
				part->on_read(node);
			}

			read_int(root, "item_length", hive.item_length);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// パートの書き込み処理を実行します。
			for (auto& part : Part::parts)
			{
				n_json node;
				part->on_write(node);
				write_child_node(root, my::wide_to_cp(part->on_get_name(), CP_UTF8), node);
			}

			write_int(root, "item_length", hive.item_length);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
