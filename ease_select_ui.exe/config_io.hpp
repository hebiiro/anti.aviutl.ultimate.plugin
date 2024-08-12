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
				n_json easing_node; get_child_node(root, "easing", easing_node);
				get_int(easing_node, "image_version", easing_window.image_version);
				get_bool(easing_node, "clamp", easing_window.clamp);
				get_string(easing_node, "horz", easing_window.horz);
				get_string(easing_node, "vert", easing_window.vert);
				get_int(easing_node, "alpha", easing_window.alpha);
				get_int(easing_node, "scale", easing_window.scale);
				get_color(easing_node, "sel_color", easing_window.sel_color);
				get_color(easing_node, "hot_color", easing_window.hot_color);
			}

			{
				n_json ease_node; get_child_node(root, "ease", ease_node);
				get_bool(ease_node, "enabled", ease_window.enabled);
				get_string(ease_node, "origin", ease_window.origin);
				get_bool(ease_node, "clamp", ease_window.clamp);
				get_string(ease_node, "horz", ease_window.horz);
				get_string(ease_node, "vert", ease_window.vert);
				get_int(ease_node, "alpha", ease_window.alpha);
				get_int(ease_node, "margin", ease_window.margin);
				get_int(ease_node, "hit_distance", ease_window.hit_distance);
				get_int(ease_node, "segment_count", ease_window.segment_count);
				get_bool(ease_node, "hide_cursor", ease_window.hide_cursor);
				get_bool(ease_node, "immediately", ease_window.immediately);
				get_size(ease_node, "window_size", ease_window.window_size);
				get_color(ease_node, "background_color", ease_window.background_color);
				get_color(ease_node, "border_color", ease_window.border_color);
				get_int(ease_node, "border_width", ease_window.border_width);
				get_color(ease_node, "curve_color", ease_window.curve_color);
				get_int(ease_node, "curve_width", ease_window.curve_width);
				get_color(ease_node, "invalid_curve_color", ease_window.invalid_curve_color);
				get_int(ease_node, "invalid_curve_width", ease_window.invalid_curve_width);
				get_color(ease_node, "handle_color", ease_window.handle_color);
				get_int(ease_node, "handle_width", ease_window.handle_width);
				get_color(ease_node, "point_color", ease_window.point_color);
				get_int(ease_node, "point_radius", ease_window.point_radius);
				get_color(ease_node, "hot_point_color", ease_window.hot_point_color);
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
				set_int(easing_node, "image_version", easing_window.image_version);
				set_bool(easing_node, "clamp", easing_window.clamp);
				set_string(easing_node, "horz", easing_window.horz);
				set_string(easing_node, "vert", easing_window.vert);
				set_int(easing_node, "alpha", easing_window.alpha);
				set_int(easing_node, "scale", easing_window.scale);
				set_color(easing_node, "sel_color", easing_window.sel_color);
				set_color(easing_node, "hot_color", easing_window.hot_color);
				set_child_node(root, "easing", easing_node);
			}

			{
				n_json ease_node;
				set_bool(ease_node, "enabled", ease_window.enabled);
				set_string(ease_node, "origin", ease_window.origin);
				set_bool(ease_node, "clamp", ease_window.clamp);
				set_string(ease_node, "horz", ease_window.horz);
				set_string(ease_node, "vert", ease_window.vert);
				set_int(ease_node, "alpha", ease_window.alpha);
				set_int(ease_node, "margin", ease_window.margin);
				set_int(ease_node, "hit_distance", ease_window.hit_distance);
				set_int(ease_node, "segment_count", ease_window.segment_count);
				set_bool(ease_node, "hide_cursor", ease_window.hide_cursor);
				set_bool(ease_node, "immediately", ease_window.immediately);
				set_size(ease_node, "window_size", ease_window.window_size);
				set_color(ease_node, "background_color", ease_window.background_color);
				set_color(ease_node, "border_color", ease_window.border_color);
				set_int(ease_node, "border_width", ease_window.border_width);
				set_color(ease_node, "curve_color", ease_window.curve_color);
				set_int(ease_node, "curve_width", ease_window.curve_width);
				set_color(ease_node, "invalid_curve_color", ease_window.invalid_curve_color);
				set_int(ease_node, "invalid_curve_width", ease_window.invalid_curve_width);
				set_color(ease_node, "handle_color", ease_window.handle_color);
				set_int(ease_node, "handle_width", ease_window.handle_width);
				set_color(ease_node, "point_color", ease_window.point_color);
				set_int(ease_node, "point_radius", ease_window.point_radius);
				set_color(ease_node, "hot_point_color", ease_window.hot_point_color);
				set_child_node(root, "ease", ease_node);
			}

			return TRUE;
		}
	} config_io;
}
