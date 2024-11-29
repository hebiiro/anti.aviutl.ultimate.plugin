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
		inline static void get_color_set(const n_json& color_set_node, share::ColorSet& color_set)
		{
			get_color(color_set_node, "fill", color_set.fill);
			get_color(color_set_node, "border", color_set.border);
			get_color(color_set_node, "text", color_set.text);
		}

		//
		// カラーセットを書き込みます。
		//
		inline static void set_color_set(n_json& color_set_node, const share::ColorSet& color_set)
		{
			set_color(color_set_node, "fill", color_set.fill);
			set_color(color_set_node, "border", color_set.border);
			set_color(color_set_node, "text", color_set.text);
		}

		//
		// カラーセット配列を読み込みます。
		//
		inline static void get_color_set_array(const n_json& node, const std::string& name, auto& color_set_array)
		{
			get_child_nodes(node, name,
				[&](const n_json& color_set_node, size_t i)
			{
				if (i >= std::size(color_set_array))
					return FALSE;

				get_color_set(color_set_node, color_set_array[i]);

				return TRUE;
			});
		}

		//
		// カラーセット配列を書き込みます。
		//
		inline static void set_color_set_array(n_json& node, const std::string& name, const auto& color_set_array)
		{
			set_child_nodes(node, name, color_set_array,
				[&](n_json& color_set_node, const auto& color_set, size_t i)
			{
				set_color_set(color_set_node, color_set);

				return TRUE;
			});
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
			get_int(root, "shadow_density", hive.shadow_density);
			get_int(root, "scrollbar_reduction", hive.scrollbar_reduction);
			get_bool(root, "draw_shadow", hive.draw_shadow);
			get_bool(root, "as_round", hive.as_round);
			get_bool(root, "draw_gradation", hive.draw_gradation);
			get_bool(root, "draw_button_edge", hive.draw_button_edge);
			get_bool(root, "draw_border", hive.draw_border);
			get_bool(root, "as_zebra", hive.as_zebra);
			get_bool(root, "apply_file_dialog", hive.apply_file_dialog);
			get_bool(root, "fix_dpi_scaling", hive.fix_dpi_scaling);
			get_bool(root, "use_layer_color", hive.use_layer_color);
			get_bool(root, "use_layer_color_multi", hive.use_layer_color_multi);
			get_bool(root, "dont_write_bytecode", hive.dont_write_bytecode);
			get_color_set_array(root, "dark_color", hive.dark_color);
			get_color_set_array(root, "light_color", hive.light_color);
			get_window(root, "addin_window", addin_window);

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
			set_int(root, "shadow_density", hive.shadow_density);
			set_int(root, "scrollbar_reduction", hive.scrollbar_reduction);
			set_bool(root, "draw_shadow", hive.draw_shadow);
			set_bool(root, "as_round", hive.as_round);
			set_bool(root, "draw_gradation", hive.draw_gradation);
			set_bool(root, "draw_button_edge", hive.draw_button_edge);
			set_bool(root, "draw_border", hive.draw_border);
			set_bool(root, "as_zebra", hive.as_zebra);
			set_bool(root, "apply_file_dialog", hive.apply_file_dialog);
			set_bool(root, "fix_dpi_scaling", hive.fix_dpi_scaling);
			set_bool(root, "use_layer_color", hive.use_layer_color);
			set_bool(root, "use_layer_color_multi", hive.use_layer_color_multi);
			set_bool(root, "dont_write_bytecode", hive.dont_write_bytecode);
			set_color_set_array(root, "dark_color", hive.dark_color);
			set_color_set_array(root, "light_color", hive.light_color);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
