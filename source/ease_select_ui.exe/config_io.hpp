#pragma once

namespace apn::ease_select_ui
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
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			{
				n_json easing_node; read_child_node(root, "easing", easing_node);
				read_int(easing_node, "image_version", easing_window.image_version);
				read_bool(easing_node, "clamp", easing_window.clamp);
				read_string(easing_node, "horz", easing_window.horz);
				read_string(easing_node, "vert", easing_window.vert);
				read_int(easing_node, "alpha", easing_window.alpha);
				read_int(easing_node, "scale", easing_window.scale);
				read_color(easing_node, "sel_color", easing_window.sel_color);
				read_color(easing_node, "hot_color", easing_window.hot_color);
			}

			{
				n_json ease_node; read_child_node(root, "ease", ease_node);
				read_bool(ease_node, "enabled", ease_window.enabled);
				read_string(ease_node, "origin", ease_window.origin);
				read_bool(ease_node, "clamp", ease_window.clamp);
				read_string(ease_node, "horz", ease_window.horz);
				read_string(ease_node, "vert", ease_window.vert);
				read_int(ease_node, "alpha", ease_window.alpha);
				read_int(ease_node, "margin", ease_window.margin);
				read_int(ease_node, "hit_distance", ease_window.hit_distance);
				read_int(ease_node, "segment_count", ease_window.segment_count);
				read_bool(ease_node, "hide_cursor", ease_window.hide_cursor);
				read_bool(ease_node, "immediately", ease_window.immediately);
				read_size(ease_node, "window_size", ease_window.window_size);
				read_color(ease_node, "background_color", ease_window.background_color);
				read_color(ease_node, "border_color", ease_window.border_color);
				read_int(ease_node, "border_width", ease_window.border_width);
				read_color(ease_node, "curve_color", ease_window.curve_color);
				read_int(ease_node, "curve_width", ease_window.curve_width);
				read_color(ease_node, "invalid_curve_color", ease_window.invalid_curve_color);
				read_int(ease_node, "invalid_curve_width", ease_window.invalid_curve_width);
				read_color(ease_node, "handle_color", ease_window.handle_color);
				read_int(ease_node, "handle_width", ease_window.handle_width);
				read_color(ease_node, "point_color", ease_window.point_color);
				read_int(ease_node, "point_radius", ease_window.point_radius);
				read_color(ease_node, "hot_point_color", ease_window.hot_point_color);
			}

			hive.config_loaded = TRUE;

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			if (!hive.config_loaded) return FALSE;

			{
				n_json easing_node;
				write_int(easing_node, "image_version", easing_window.image_version);
				write_bool(easing_node, "clamp", easing_window.clamp);
				write_string(easing_node, "horz", easing_window.horz);
				write_string(easing_node, "vert", easing_window.vert);
				write_int(easing_node, "alpha", easing_window.alpha);
				write_int(easing_node, "scale", easing_window.scale);
				write_color(easing_node, "sel_color", easing_window.sel_color);
				write_color(easing_node, "hot_color", easing_window.hot_color);
				write_child_node(root, "easing", easing_node);
			}

			{
				n_json ease_node;
				write_bool(ease_node, "enabled", ease_window.enabled);
				write_string(ease_node, "origin", ease_window.origin);
				write_bool(ease_node, "clamp", ease_window.clamp);
				write_string(ease_node, "horz", ease_window.horz);
				write_string(ease_node, "vert", ease_window.vert);
				write_int(ease_node, "alpha", ease_window.alpha);
				write_int(ease_node, "margin", ease_window.margin);
				write_int(ease_node, "hit_distance", ease_window.hit_distance);
				write_int(ease_node, "segment_count", ease_window.segment_count);
				write_bool(ease_node, "hide_cursor", ease_window.hide_cursor);
				write_bool(ease_node, "immediately", ease_window.immediately);
				write_size(ease_node, "window_size", ease_window.window_size);
				write_color(ease_node, "background_color", ease_window.background_color);
				write_color(ease_node, "border_color", ease_window.border_color);
				write_int(ease_node, "border_width", ease_window.border_width);
				write_color(ease_node, "curve_color", ease_window.curve_color);
				write_int(ease_node, "curve_width", ease_window.curve_width);
				write_color(ease_node, "invalid_curve_color", ease_window.invalid_curve_color);
				write_int(ease_node, "invalid_curve_width", ease_window.invalid_curve_width);
				write_color(ease_node, "handle_color", ease_window.handle_color);
				write_int(ease_node, "handle_width", ease_window.handle_width);
				write_color(ease_node, "point_color", ease_window.point_color);
				write_int(ease_node, "point_radius", ease_window.point_radius);
				write_color(ease_node, "hot_point_color", ease_window.hot_point_color);
				write_child_node(root, "ease", ease_node);
			}

			return TRUE;
		}
	} config_io;
}
