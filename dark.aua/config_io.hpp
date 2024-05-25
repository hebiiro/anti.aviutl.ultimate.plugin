#pragma once

namespace apn::dark
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
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			get_string(root, "skin_name", hive.skin_name);
			get_string(root, "scheme_name", hive.scheme_name);
			get_int(root, "ellipse", hive.ellipse);
			get_int(root, "border_width", hive.border_width);
			get_label(root, "shadow_mode", hive.shadow_mode, hive.c_shadow_mode.labels);
			get_label(root, "round_mode", hive.round_mode, hive.c_round_mode.labels);
			get_label(root, "static_edge_mode", hive.static_edge_mode, hive.c_static_edge_mode.labels);
			get_label(root, "file_dialog_mode", hive.file_dialog_mode, hive.c_file_dialog_mode.labels);
			get_label(root, "dpi_scaling_mode", hive.dpi_scaling_mode, hive.c_dpi_scaling_mode.labels);
			get_bool(root, "use_layer_color", hive.use_layer_color);
			get_bool(root, "use_layer_color_multi", hive.use_layer_color_multi);
			get_color(root, "main_background_color.dark", hive.main_background_color.dark);
			get_color(root, "main_background_color.light", hive.main_background_color.light);
			get_color(root, "main_text_color.dark", hive.main_text_color.dark);
			get_color(root, "main_text_color.light", hive.main_text_color.light);
			get_color(root, "sub_background_color.dark", hive.sub_background_color.dark);
			get_color(root, "sub_background_color.light", hive.sub_background_color.light);
			get_color(root, "sub_text_color.dark", hive.sub_text_color.dark);
			get_color(root, "sub_text_color.light", hive.sub_text_color.light);
			get_window(root, "addin_window", addin_window);

			size_t i = 0;
			if (auto custom_color_nodes_op = root.get_child_optional("custom_color"))
			{
				for (const auto& pair : custom_color_nodes_op.value())
				{
					const auto& custom_color_node = pair.second;

					if (i < std::size(hive.custom_colors))
					{
						get_int(custom_color_node, "", hive.custom_colors[i]);

						i++;
					}
				}
			}

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			set_string(root, "skin_name", hive.skin_name);
			set_string(root, "scheme_name", hive.scheme_name);
			set_int(root, "ellipse", hive.ellipse);
			set_int(root, "border_width", hive.border_width);
			set_label(root, "shadow_mode", hive.shadow_mode, hive.c_shadow_mode.labels);
			set_label(root, "round_mode", hive.round_mode, hive.c_round_mode.labels);
			set_label(root, "static_edge_mode", hive.static_edge_mode, hive.c_static_edge_mode.labels);
			set_label(root, "file_dialog_mode", hive.file_dialog_mode, hive.c_file_dialog_mode.labels);
			set_label(root, "dpi_scaling_mode", hive.dpi_scaling_mode, hive.c_dpi_scaling_mode.labels);
			set_bool(root, "use_layer_color", hive.use_layer_color);
			set_bool(root, "use_layer_color_multi", hive.use_layer_color_multi);
			set_color(root, "main_background_color.dark", hive.main_background_color.dark);
			set_color(root, "main_background_color.light", hive.main_background_color.light);
			set_color(root, "main_text_color.dark", hive.main_text_color.dark);
			set_color(root, "main_text_color.light", hive.main_text_color.light);
			set_color(root, "sub_background_color.dark", hive.sub_background_color.dark);
			set_color(root, "sub_background_color.light", hive.sub_background_color.light);
			set_color(root, "sub_text_color.dark", hive.sub_text_color.dark);
			set_color(root, "sub_text_color.light", hive.sub_text_color.light);
			set_window(root, "addin_window", addin_window);

			ptree custom_color_nodes;
			{
				for (size_t i = 0; i < std::size(hive.custom_colors); i++)
				{
					ptree custom_color_node;
					{
						set_color(custom_color_node, "", hive.custom_colors[i]);
					}
					custom_color_nodes.push_back(std::make_pair("", custom_color_node));
				}
			}
			root.put_child("custom_color", custom_color_nodes);

			return TRUE;
		}
	} config_io;
}
