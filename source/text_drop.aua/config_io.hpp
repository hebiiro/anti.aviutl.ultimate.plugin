#pragma once

namespace apn::text_drop
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.config_file_name = magi.get_config_file_name(hive.instance);
			MY_TRACE_STR(hive.config_file_name);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		BOOL read()
		{
			MY_TRACE_FUNC("");

			auto config_file_name = hive.config_file_name;
			if (!std::filesystem::exists(config_file_name))
				config_file_name = magi.get_default_config_file_name(hive.instance);
			return read_file(config_file_name, hive);
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

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

			read_int(root, "item_range", hive.item_range);
			read_int(root, "space_range", hive.space_range);
			read_int(root, "char_range", hive.char_range);
			read_label(root, "item_direction", hive.item_direction, hive.c_direction.labels);
			read_string(root, "exo_file_name", hive.exo_file_name);
			read_string(root, "drop_text", hive.drop_text);
			read_int(root, "insert_layer", hive.insert_layer);
			read_bool(root, "selected_text_only", hive.selected_text_only);
			read_string(root, "text_file_name", hive.text_file_name);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_int(root, "item_range", hive.item_range);
			write_int(root, "space_range", hive.space_range);
			write_int(root, "char_range", hive.char_range);
			write_label(root, "item_direction", hive.item_direction, hive.c_direction.labels);
			write_string(root, "exo_file_name", hive.exo_file_name);
			write_string(root, "drop_text", hive.drop_text);
			write_int(root, "insert_layer", hive.insert_layer);
			write_bool(root, "selected_text_only", hive.selected_text_only);
			write_string(root, "text_file_name", hive.text_file_name);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
