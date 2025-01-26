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

			read_bool(root, "use_shift", keyboard_hook.use_shift);
			read_bool(root, "use_ctrl", keyboard_hook.use_ctrl);
			read_bool(root, "use_alt", keyboard_hook.use_alt);
			read_bool(root, "use_win", keyboard_hook.use_win);

			{
				n_json aim_src_node;
				read_child_node(root, "aim_src", aim_src_node);
				read_bool(aim_src_node, "available", aim_src.available);
				read_int(aim_src_node, "alpha", aim_src.alpha);
				read_color(aim_src_node, "color", aim_src.color);
			}

			{
				n_json aim_dst_node;
				read_child_node(root, "aim_dst", aim_dst_node);
				read_bool(aim_dst_node, "available", aim_dst.available);
				read_int(aim_dst_node, "alpha", aim_dst.alpha);
				read_color(aim_dst_node, "color", aim_dst.color);
			}

			{
				n_json sight_node;
				read_child_node(root, "sight", sight_node);
				read_bool(sight_node, "available", sight.config.available);
				read_size(sight_node, "canvas_size", sight.config.canvas_size);
				read_int(sight_node, "alpha", sight.config.alpha);
				read_color(sight_node, "pen_color", sight.config.pen_color);
				read_real(sight_node, "pen_width", sight.config.pen_width);
				read_color(sight_node, "brush_color", sight.config.brush_color);
				read_int(sight_node, "base", sight.config.base);
				read_int(sight_node, "width", sight.config.width);
				read_string(sight_node, "font_name", sight.config.font_name);
				read_real(sight_node, "font_size", sight.config.font_size);
				read_real(sight_node, "rotate", sight.config.rotate);
				read_point(sight_node, "start_offset", sight.config.start_offset);
			}

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_bool(root, "use_shift", keyboard_hook.use_shift);
			write_bool(root, "use_ctrl", keyboard_hook.use_ctrl);
			write_bool(root, "use_alt", keyboard_hook.use_alt);
			write_bool(root, "use_win", keyboard_hook.use_win);

			{
				n_json aim_src_node;
				write_bool(aim_src_node, "available", aim_src.available);
				write_int(aim_src_node, "alpha", aim_src.alpha);
				write_color(aim_src_node, "color", aim_src.color);
				write_child_node(root, "aim_src", aim_src_node);
			}

			{
				n_json aim_dst_node;
				write_bool(aim_dst_node, "available", aim_dst.available);
				write_int(aim_dst_node, "alpha", aim_dst.alpha);
				write_color(aim_dst_node, "color", aim_dst.color);
				write_child_node(root, "aim_dst", aim_dst_node);
			}

			{
				n_json sight_node;
				write_bool(sight_node, "available", sight.config.available);
				write_size(sight_node, "canvas_size", sight.config.canvas_size);
				write_int(sight_node, "alpha", sight.config.alpha);
				write_color(sight_node, "pen_color", sight.config.pen_color);
				write_real(sight_node, "pen_width", sight.config.pen_width);
				write_color(sight_node, "brush_color", sight.config.brush_color);
				write_int(sight_node, "base", sight.config.base);
				write_int(sight_node, "width", sight.config.width);
				write_string(sight_node, "font_name", sight.config.font_name);
				write_real(sight_node, "font_size", sight.config.font_size);
				write_real(sight_node, "rotate", sight.config.rotate);
				write_point(sight_node, "start_offset", sight.config.start_offset);
				write_child_node(root, "sight", sight_node);
			}

			return TRUE;
		}
	} config_io;
}
