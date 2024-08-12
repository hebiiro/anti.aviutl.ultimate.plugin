#pragma once

namespace apn::filter_drag
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

			sight.update_context();

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			get_bool(root, "use_shift", keyboard_hook.use_shift);
			get_bool(root, "use_ctrl", keyboard_hook.use_ctrl);
			get_bool(root, "use_alt", keyboard_hook.use_alt);
			get_bool(root, "use_win", keyboard_hook.use_win);

			{
				n_json aim_src_node;
				get_child_node(root, "aim_src", aim_src_node);
				get_bool(aim_src_node, "available", aim_src.available);
				get_int(aim_src_node, "alpha", aim_src.alpha);
				get_color(aim_src_node, "color", aim_src.color);
			}

			{
				n_json aim_dst_node;
				get_child_node(root, "aim_dst", aim_dst_node);
				get_bool(aim_dst_node, "available", aim_dst.available);
				get_int(aim_dst_node, "alpha", aim_dst.alpha);
				get_color(aim_dst_node, "color", aim_dst.color);
			}

			{
				n_json sight_node;
				get_child_node(root, "sight", sight_node);
				get_bool(sight_node, "available", sight.config.available);
				get_size(sight_node, "canvas_size", sight.config.canvas_size);
				get_int(sight_node, "alpha", sight.config.alpha);
				get_color(sight_node, "pen_color", sight.config.pen_color);
				get_float(sight_node, "pen_width", sight.config.pen_width);
				get_color(sight_node, "brush_color", sight.config.brush_color);
				get_int(sight_node, "base", sight.config.base);
				get_int(sight_node, "width", sight.config.width);
				get_string(sight_node, "font_name", sight.config.font_name);
				get_float(sight_node, "font_size", sight.config.font_size);
				get_float(sight_node, "rotate", sight.config.rotate);
				get_point(sight_node, "start_offset", sight.config.start_offset);
			}

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_bool(root, "use_shift", keyboard_hook.use_shift);
			set_bool(root, "use_ctrl", keyboard_hook.use_ctrl);
			set_bool(root, "use_alt", keyboard_hook.use_alt);
			set_bool(root, "use_win", keyboard_hook.use_win);

			{
				n_json aim_src_node;
				set_bool(aim_src_node, "available", aim_src.available);
				set_int(aim_src_node, "alpha", aim_src.alpha);
				set_color(aim_src_node, "color", aim_src.color);
				set_child_node(root, "aim_src", aim_src_node);
			}

			{
				n_json aim_dst_node;
				set_bool(aim_dst_node, "available", aim_dst.available);
				set_int(aim_dst_node, "alpha", aim_dst.alpha);
				set_color(aim_dst_node, "color", aim_dst.color);
				set_child_node(root, "aim_dst", aim_dst_node);
			}

			{
				n_json sight_node;
				set_bool(sight_node, "available", sight.config.available);
				set_size(sight_node, "canvas_size", sight.config.canvas_size);
				set_int(sight_node, "alpha", sight.config.alpha);
				set_color(sight_node, "pen_color", sight.config.pen_color);
				set_float(sight_node, "pen_width", sight.config.pen_width);
				set_color(sight_node, "brush_color", sight.config.brush_color);
				set_int(sight_node, "base", sight.config.base);
				set_int(sight_node, "width", sight.config.width);
				set_string(sight_node, "font_name", sight.config.font_name);
				set_float(sight_node, "font_size", sight.config.font_size);
				set_float(sight_node, "rotate", sight.config.rotate);
				set_point(sight_node, "start_offset", sight.config.start_offset);
				set_child_node(root, "sight", sight_node);
			}

			return TRUE;
		}
	} config_io;
}
