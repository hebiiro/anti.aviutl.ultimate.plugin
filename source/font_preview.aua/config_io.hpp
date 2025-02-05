#pragma once

namespace apn::font_preview
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

			read_int(root, "item_height", hive.item_height);
			read_int(root, "font_height", hive.font_height);
			read_size(root, "window_size", hive.window_size);
			read_string(root, "sample", hive.sample);
			read_bool(root, "singleline", hive.singleline);
			read_bool(root, "use_context_menu", hive.use_context_menu);
			read_bool(root, "use_shift_key", hive.use_shift_key);
			read_bool(root, "use_ctrl_key", hive.use_ctrl_key);
			read_bool(root, "use_alt_key", hive.use_alt_key);
			read_bool(root, "use_win_key", hive.use_win_key);

			{
				n_json paint_node;
				read_child_node(root, "paint", paint_node);
				read_label(paint_node, "mode", paint.mode, paint.c_mode.labels);
				{
					n_json palette_node;
					read_child_node(paint_node, "palette", palette_node);
					{
						n_json normal_node;
						read_child_node(palette_node, "normal", normal_node);
						{
							n_json color_node;
							read_child_node(normal_node, "color", color_node);
							read_color(color_node, "background", paint.palette[paint.c_style.c_normal].color.background);
							read_color(color_node, "sample", paint.palette[paint.c_style.c_normal].color.sample);
							read_color(color_node, "font_name", paint.palette[paint.c_style.c_normal].color.font_name);
						}
					}

					{
						n_json select_node;
						read_child_node(palette_node, "select", select_node);
						{
							n_json color_node;
							read_child_node(select_node, "color", color_node);
							read_color(color_node, "background", paint.palette[paint.c_style.c_select].color.background);
							read_color(color_node, "sample", paint.palette[paint.c_style.c_select].color.sample);
							read_color(color_node, "font_name", paint.palette[paint.c_style.c_select].color.font_name);
						}
					}
				}
			}

			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_int(root, "item_height", hive.item_height);
			write_int(root, "font_height", hive.font_height);
			write_size(root, "window_size", hive.window_size);
			write_string(root, "sample", hive.sample);
			write_bool(root, "singleline", hive.singleline);
			write_bool(root, "use_context_menu", hive.use_context_menu);
			write_bool(root, "use_shift_key", hive.use_shift_key);
			write_bool(root, "use_ctrl_key", hive.use_ctrl_key);
			write_bool(root, "use_alt_key", hive.use_alt_key);
			write_bool(root, "use_win_key", hive.use_win_key);

			{
				n_json paint_node;
				write_label(paint_node, "mode", paint.mode, paint.c_mode.labels);
				{
					n_json palette_node;
					{
						n_json normal_node;
						{
							n_json color_node;
							write_color(color_node, "background", paint.palette[paint.c_style.c_normal].color.background);
							write_color(color_node, "sample", paint.palette[paint.c_style.c_normal].color.sample);
							write_color(color_node, "font_name", paint.palette[paint.c_style.c_normal].color.font_name);
							write_child_node(normal_node, "color", color_node);
						}
						write_child_node(palette_node, "normal", normal_node);
					}
					{
						n_json select_node;
						{
							n_json color_node;
							write_color(color_node, "background", paint.palette[paint.c_style.c_select].color.background);
							write_color(color_node, "sample", paint.palette[paint.c_style.c_select].color.sample);
							write_color(color_node, "font_name", paint.palette[paint.c_style.c_select].color.font_name);
							write_child_node(select_node, "color", color_node);
						}
						write_child_node(palette_node, "select", select_node);
					}
					write_child_node(paint_node, "palette", palette_node);
				}
				write_child_node(root, "paint", paint_node);
			}

			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
