#pragma once

namespace apn::item_copy
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

			return addin_dialog.update_controls();
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_int(root, "insert_layer", hive.insert_layer);
			read_string(root, "prefix", hive.prefix);
			read_bool(root, "use_empty_text", hive.use_empty_text);
			read_label(root, "file_split_mode", hive.file_split_mode, hive.c_file_split_mode.labels);
			read_string(root, "exo_file_name", hive.exo_file_name);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_int(root, "insert_layer", hive.insert_layer);
			write_string(root, "prefix", hive.prefix);
			write_bool(root, "use_empty_text", hive.use_empty_text);
			write_label(root, "file_split_mode", hive.file_split_mode, hive.c_file_split_mode.labels);
			write_string(root, "exo_file_name", hive.exo_file_name);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
