#pragma once

namespace apn::item_copy
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

			get_int(root, "insert_layer", hive.insert_layer);
			get_string(root, "prefix", hive.prefix);
			get_bool(root, "use_empty_text", hive.use_empty_text);
			get_label(root, "file_split_mode", hive.file_split_mode, hive.c_file_split_mode.labels);
			get_string(root, "exo_file_name", hive.exo_file_name);
			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_int(root, "insert_layer", hive.insert_layer);
			set_string(root, "prefix", hive.prefix);
			set_bool(root, "use_empty_text", hive.use_empty_text);
			set_label(root, "file_split_mode", hive.file_split_mode, hive.c_file_split_mode.labels);
			set_string(root, "exo_file_name", hive.exo_file_name);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
