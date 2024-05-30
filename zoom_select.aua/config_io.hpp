#pragma once

namespace apn::zoom_select
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
			if (auto zoom_value_nodes_op = root.get_child_optional("zoom_value"))
			{
				for (const auto& pair : zoom_value_nodes_op.value())
				{
					const auto& zoom_value_node = pair.second;

					if (i < std::size(hive.zoom_values))
					{
						get_int(zoom_value_node, "", hive.zoom_values[i]);

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

			ptree zoom_value_nodes;
			{
				for (size_t i = 0; i < std::size(hive.zoom_values); i++)
				{
					ptree zoom_value_node;
					{
						set_int(zoom_value_node, "", hive.zoom_values[i]);
					}
					zoom_value_nodes.push_back(std::make_pair("", zoom_value_node));
				}
			}
			root.put_child("zoom_value", zoom_value_nodes);

			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
