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

			sight.update_context();

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			get_bool(root, "use_shift", keyboard_hook.use_shift);
			get_bool(root, "use_ctrl", keyboard_hook.use_ctrl);
			get_bool(root, "use_alt", keyboard_hook.use_alt);
			get_bool(root, "use_win", keyboard_hook.use_win);

			get_bool(root, "aim_src.available", aim_src.available);
			get_int(root, "aim_src.alpha", aim_src.alpha);
			get_color(root, "aim_src.color", aim_src.color);

			get_bool(root, "aim_dst.available", aim_dst.available);
			get_int(root, "aim_dst.alpha", aim_dst.alpha);
			get_color(root, "aim_dst.color", aim_dst.color);

			get_bool(root, "sight.available", sight.config.available);
			get_size(root, "sight.canvas_size", sight.config.canvas_size);
			get_int(root, "sight.alpha", sight.config.alpha);
			get_color(root, "sight.pen_color", sight.config.pen_color);
			get_float(root, "sight.pen_width", sight.config.pen_width);
			get_color(root, "sight.brush_color", sight.config.brush_color);
			get_int(root, "sight.base", sight.config.base);
			get_int(root, "sight.width", sight.config.width);
			get_string(root, "sight.font_name", sight.config.font_name);
			get_float(root, "sight.font_size", sight.config.font_size);
			get_float(root, "sight.rotate", sight.config.rotate);
			get_point(root, "sight.start_offset", sight.config.start_offset);

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			set_bool(root, "use_shift", keyboard_hook.use_shift);
			set_bool(root, "use_ctrl", keyboard_hook.use_ctrl);
			set_bool(root, "use_alt", keyboard_hook.use_alt);
			set_bool(root, "use_win", keyboard_hook.use_win);

			set_bool(root, "aim_src.available", aim_src.available);
			set_int(root, "aim_src.alpha", aim_src.alpha);
			set_color(root, "aim_src.color", aim_src.color);

			set_bool(root, "aim_dst.available", aim_dst.available);
			set_int(root, "aim_dst.alpha", aim_dst.alpha);
			set_color(root, "aim_dst.color", aim_dst.color);

			set_bool(root, "sight.available", sight.config.available);
			set_size(root, "sight.canvas_size", sight.config.canvas_size);
			set_int(root, "sight.alpha", sight.config.alpha);
			set_color(root, "sight.pen_color", sight.config.pen_color);
			set_float(root, "sight.pen_width", sight.config.pen_width);
			set_color(root, "sight.brush_color", sight.config.brush_color);
			set_int(root, "sight.base", sight.config.base);
			set_int(root, "sight.width", sight.config.width);
			set_string(root, "sight.font_name", sight.config.font_name);
			set_float(root, "sight.font_size", sight.config.font_size);
			set_float(root, "sight.rotate", sight.config.rotate);
			set_point(root, "sight.start_offset", sight.config.start_offset);

			return TRUE;
		}
	} config_io;
}
