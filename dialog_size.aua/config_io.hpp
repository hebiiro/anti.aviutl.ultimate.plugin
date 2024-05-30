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
		// コンフィグを保存します。
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
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			size_t i = 0;
			if (auto custom_template_nodes_op = root.get_child_optional("custom_template"))
			{
				for (const auto& pair : custom_template_nodes_op.value())
				{
					const auto& custom_template_node = pair.second;

					if (i < std::size(hive.custom_templates))
					{
						get_bool(custom_template_node, "active", hive.custom_templates[i].active);
//						get_string(custom_template_node, "name", hive.custom_templates[i].target_display_name);

						i++;
					}
				}
			}

			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			ptree custom_template_nodes;
			{
				for (size_t i = 0; i < std::size(hive.custom_templates); i++)
				{
					ptree custom_template_node;
					{
						set_bool(custom_template_node, "active", hive.custom_templates[i].active);
						set_string(custom_template_node, "name", hive.custom_templates[i].target_display_name);
					}
					custom_template_nodes.push_back(std::make_pair("", custom_template_node));
				}
			}
			root.put_child("custom_template", custom_template_nodes);

			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
