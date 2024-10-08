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
		// カラーセットを読み込みます。
		//
		inline static void get_color_set(const n_json& node, const std::string& name, share::ColorSet& color_set)
		{
			n_json color_set_node;
			get_child_node(node, name, color_set_node);
			get_color(color_set_node, "primary_fill", color_set.primary_fill);
			get_color(color_set_node, "primary_border", color_set.primary_border);
			get_color(color_set_node, "primary_text", color_set.primary_text);
			get_color(color_set_node, "secondary_fill", color_set.secondary_fill);
			get_color(color_set_node, "secondary_border", color_set.secondary_border);
			get_color(color_set_node, "secondary_text", color_set.secondary_text);
		}

		//
		// カラーセットを書き込みます。
		//
		inline static void set_color_set(n_json& node, const std::string& name, const share::ColorSet& color_set)
		{
			n_json color_set_node;
			set_color(color_set_node, "primary_fill", color_set.primary_fill);
			set_color(color_set_node, "primary_border", color_set.primary_border);
			set_color(color_set_node, "primary_text", color_set.primary_text);
			set_color(color_set_node, "secondary_fill", color_set.secondary_fill);
			set_color(color_set_node, "secondary_border", color_set.secondary_border);
			set_color(color_set_node, "secondary_text", color_set.secondary_text);
			set_child_node(node, name, color_set_node);
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			get_string(root, "skin_name", hive.skin_name);
			get_string(root, "scheme_name", hive.scheme_name);
			get_bool(root, "dark_mode", hive.dark_mode);
			get_int(root, "ellipse", hive.ellipse);
			get_int(root, "border_width", hive.border_width);
			get_bool(root, "draw_shadow", hive.draw_shadow);
			get_bool(root, "as_round", hive.as_round);
			get_bool(root, "draw_button_edge", hive.draw_button_edge);
			get_bool(root, "draw_border", hive.draw_border);
			get_bool(root, "as_zebra", hive.as_zebra);
			get_bool(root, "apply_file_dialog", hive.apply_file_dialog);
			get_bool(root, "fix_dpi_scaling", hive.fix_dpi_scaling);
			get_bool(root, "use_layer_color", hive.use_layer_color);
			get_bool(root, "use_layer_color_multi", hive.use_layer_color_multi);
			get_bool(root, "dont_write_bytecode", hive.dont_write_bytecode);
			get_color_set(root, "color_set_dark", hive.color_set.dark);
			get_color_set(root, "color_set_light", hive.color_set.light);
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
			set_bool(root, "dark_mode", hive.dark_mode);
			set_int(root, "ellipse", hive.ellipse);
			set_int(root, "border_width", hive.border_width);
			set_bool(root, "draw_shadow", hive.draw_shadow);
			set_bool(root, "as_round", hive.as_round);
			set_bool(root, "draw_button_edge", hive.draw_button_edge);
			set_bool(root, "draw_border", hive.draw_border);
			set_bool(root, "as_zebra", hive.as_zebra);
			set_bool(root, "apply_file_dialog", hive.apply_file_dialog);
			set_bool(root, "fix_dpi_scaling", hive.fix_dpi_scaling);
			set_bool(root, "use_layer_color", hive.use_layer_color);
			set_bool(root, "use_layer_color_multi", hive.use_layer_color_multi);
			set_bool(root, "dont_write_bytecode", hive.dont_write_bytecode);
			set_color_set(root, "color_set_dark", hive.color_set.dark);
			set_color_set(root, "color_set_light", hive.color_set.light);
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
