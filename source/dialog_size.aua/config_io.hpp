#pragma once

namespace apn::dialog_size
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

			return read_file(hive.config_file_name, hive);
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

			get_child_nodes(root, "custom_template",
				[&](const n_json& custom_template_node, size_t i)
			{
				if (i >= std::size(hive.custom_templates))
					return FALSE;

				get_bool(custom_template_node, "active", hive.custom_templates[i].active);
//				get_string(custom_template_node, "name", hive.custom_templates[i].target_display_name);

				return TRUE;
			});

			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_child_nodes(root, "custom_template", hive.custom_templates,
				[&](n_json& custom_template_node, const auto& custom_template, size_t i)
			{
				set_bool(custom_template_node, "active", custom_template.active);
				set_string(custom_template_node, "name", custom_template.target_display_name);

				return TRUE;
			});

			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
