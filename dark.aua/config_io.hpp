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
		// コンフィグを書き込みます。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// ペアカラーを読み込みます。
		//
		inline static void get_pair_color(const n_json& node, const std::string& name, share::PairColor& pair_color)
		{
			n_json pair_color_node;
			get_child_node(node, name, pair_color_node);
			get_color(pair_color_node, "dark", pair_color.dark);
			get_color(pair_color_node, "light", pair_color.light);
		}

		//
		// ペアカラーを書き込みます。
		//
		inline static void set_pair_color(n_json& node, const std::string& name, const share::PairColor& pair_color)
		{
			n_json pair_color_node;
			set_color(pair_color_node, "dark", pair_color.dark);
			set_color(pair_color_node, "light", pair_color.light);
			set_child_node(node, name, pair_color_node);
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
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
			get_bool(root, "dont_write_bytecode", hive.dont_write_bytecode);
			get_pair_color(root, "main_background_color", hive.main_background_color);
			get_pair_color(root, "main_text_color", hive.main_text_color);
			get_pair_color(root, "sub_background_color", hive.sub_background_color);
			get_pair_color(root, "sub_text_color", hive.sub_text_color);
			get_window(root, "addin_window", addin_window);

			size_t i = 0;
			get_child_nodes(root, "custom_color",
				[&](const n_json& custom_color_node)
			{
				if (i >= std::size(hive.custom_colors))
					return FALSE;

				get_int(custom_color_node, hive.custom_colors[i]);

				i++;

				return TRUE;
			});

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
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
			set_bool(root, "dont_write_bytecode", hive.dont_write_bytecode);
			set_pair_color(root, "main_background_color", hive.main_background_color);
			set_pair_color(root, "main_text_color", hive.main_text_color);
			set_pair_color(root, "sub_background_color", hive.sub_background_color);
			set_pair_color(root, "sub_text_color", hive.sub_text_color);
			set_window(root, "addin_window", addin_window);

			set_child_nodes(root, "custom_color", hive.custom_colors,
				[&](n_json& custom_color_node, const auto& custom_color)
			{
				set_color(custom_color_node, custom_color);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
