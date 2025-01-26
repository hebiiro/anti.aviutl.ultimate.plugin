#pragma once

namespace apn::dark
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
		//
		// カラーセットを読み込みます。
		//
		inline static void read_color_set(const n_json& color_set_node, share::ColorSet& color_set)
		{
			read_color(color_set_node, "fill", color_set.fill);
			read_color(color_set_node, "border", color_set.border);
			read_color(color_set_node, "text", color_set.text);
		}

		//
		// カラーセットを書き込みます。
		//
		inline static void write_color_set(n_json& color_set_node, const share::ColorSet& color_set)
		{
			write_color(color_set_node, "fill", color_set.fill);
			write_color(color_set_node, "border", color_set.border);
			write_color(color_set_node, "text", color_set.text);
		}

		//
		// カラーセット配列を読み込みます。
		//
		inline static void read_color_set_array(const n_json& node, const std::string& name, auto& color_set_array)
		{
			read_child_nodes(node, name,
				[&](const n_json& color_set_node, size_t i)
			{
				if (i >= std::size(color_set_array))
					return FALSE;

				read_color_set(color_set_node, color_set_array[i]);

				return TRUE;
			});
		}

		//
		// カラーセット配列を書き込みます。
		//
		inline static void write_color_set_array(n_json& node, const std::string& name, const auto& color_set_array)
		{
			write_child_nodes(node, name, color_set_array,
				[&](n_json& color_set_node, const auto& color_set, size_t i)
			{
				write_color_set(color_set_node, color_set);

				return TRUE;
			});
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_string(root, "skin_name", hive.skin_name);
			read_string(root, "scheme_name", hive.scheme_name);
			read_bool(root, "dark_mode", hive.dark_mode);
			read_int(root, "ellipse", hive.ellipse);
			read_int(root, "border_width", hive.border_width);
			read_int(root, "shadow_density", hive.shadow_density);
			read_int(root, "scrollbar_reduction", hive.scrollbar_reduction);
			read_bool(root, "draw_shadow", hive.draw_shadow);
			read_bool(root, "as_round", hive.as_round);
			read_bool(root, "draw_gradation", hive.draw_gradation);
			read_bool(root, "draw_button_edge", hive.draw_button_edge);
			read_bool(root, "draw_border", hive.draw_border);
			read_bool(root, "as_zebra", hive.as_zebra);
			read_bool(root, "apply_file_dialog", hive.apply_file_dialog);
			read_bool(root, "fix_dpi_scaling", hive.fix_dpi_scaling);
			read_bool(root, "use_layer_color", hive.use_layer_color);
			read_bool(root, "use_layer_color_multi", hive.use_layer_color_multi);
			read_bool(root, "dont_write_bytecode", hive.dont_write_bytecode);
			read_bool(root, "specialize_checkbox", hive.specialize_checkbox);
			read_color_set_array(root, "dark_color", hive.dark_color);
			read_color_set_array(root, "light_color", hive.light_color);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_string(root, "skin_name", hive.skin_name);
			write_string(root, "scheme_name", hive.scheme_name);
			write_bool(root, "dark_mode", hive.dark_mode);
			write_int(root, "ellipse", hive.ellipse);
			write_int(root, "border_width", hive.border_width);
			write_int(root, "shadow_density", hive.shadow_density);
			write_int(root, "scrollbar_reduction", hive.scrollbar_reduction);
			write_bool(root, "draw_shadow", hive.draw_shadow);
			write_bool(root, "as_round", hive.as_round);
			write_bool(root, "draw_gradation", hive.draw_gradation);
			write_bool(root, "draw_button_edge", hive.draw_button_edge);
			write_bool(root, "draw_border", hive.draw_border);
			write_bool(root, "as_zebra", hive.as_zebra);
			write_bool(root, "apply_file_dialog", hive.apply_file_dialog);
			write_bool(root, "fix_dpi_scaling", hive.fix_dpi_scaling);
			write_bool(root, "use_layer_color", hive.use_layer_color);
			write_bool(root, "use_layer_color_multi", hive.use_layer_color_multi);
			write_bool(root, "dont_write_bytecode", hive.dont_write_bytecode);
			write_bool(root, "specialize_checkbox", hive.specialize_checkbox);
			write_color_set_array(root, "dark_color", hive.dark_color);
			write_color_set_array(root, "light_color", hive.light_color);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
