#pragma once

namespace apn::dialog_size
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

			read_child_nodes(root, "custom_template",
				[&](const n_json& custom_template_node, size_t i)
			{
				if (i >= std::size(hive.custom_templates))
					return FALSE;

				read_bool(custom_template_node, "active", hive.custom_templates[i].active);
//				read_string(custom_template_node, "name", hive.custom_templates[i].target_display_name);

				return TRUE;
			});

			read_window_pos(root, "addin_window", addin_window);

			new_file_dialog.read_node(root);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_child_nodes(root, "custom_template", hive.custom_templates,
				[&](n_json& custom_template_node, const auto& custom_template, size_t i)
			{
				write_bool(custom_template_node, "active", custom_template.active);
				write_string(custom_template_node, "name", custom_template.target_display_name);

				return TRUE;
			});

			write_window_pos(root, "addin_window", addin_window);

			new_file_dialog.write_node(root);

			return TRUE;
		}
	} config_io;
}
