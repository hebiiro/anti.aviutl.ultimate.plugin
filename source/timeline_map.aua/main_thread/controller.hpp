#pragma once

namespace apn::timeline_map::main_thread
{
	//
	// このクラスはメインスレッドのコントローラーです。
	// ビューの初期化/後始末と、プロパティの入出力を担当します。
	//
	inline struct controller_t
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			if (!view.init())
			{
				hive.message_box(L"メインスレッドのビューの初期化に失敗しました\n");

				return FALSE;
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			view.exit();

			return TRUE;
		}

		//
		// ラムダ式を使用してコンフィグを読み込みます。
		//
		void read_node(const n_json& root, const std::string& node_name, auto func)
		{
			n_json node;
			read_child_node(root, node_name, node);
			return func(node);
		}

		//
		// ラムダ式を使用してコンフィグを書き込みます。
		//
		void write_node(n_json& root, const std::string& node_name, auto func)
		{
			n_json node;
			my::scope_exit scope_exit([&]() { write_child_node(root, node_name, node); });
			return func(node);
		}

		//
		// RGBAを読み込みます。
		//
		void read_rgba(const n_json& root, const std::string& node_name, rgba_t& rgba)
		{
			read_node(root, node_name, [&](const n_json& root) {
				read_int(root, "r", rgba.r);
				read_int(root, "g", rgba.g);
				read_int(root, "b", rgba.b);
				read_int(root, "a", rgba.a);
			});
		}

		//
		// RGBAを書き込みます。
		//
		void write_rgba(n_json& root, const std::string& node_name, const rgba_t& rgba)
		{
			write_node(root, node_name, [&](n_json& root) {
				write_int(root, "r", rgba.r);
				write_int(root, "g", rgba.g);
				write_int(root, "b", rgba.b);
				write_int(root, "a", rgba.a);
			});
		}

		//
		// アイテムノードを読み込みます。
		//
		void read_item_node(const n_json& root, const std::string& node_name, property_t::item_t::node_t& item_node)
		{
			read_node(root, node_name, [&](const n_json& root) {
				read_rgba(root, "start_color", item_node.start_color);
				read_rgba(root, "end_color", item_node.end_color);
			});
		}

		//
		// アイテムノードを書き込みます。
		//
		void write_item_node(n_json& root, const std::string& node_name, const property_t::item_t::node_t& item_node)
		{
			write_node(root, node_name, [&](n_json& root) {
				write_rgba(root, "start_color", item_node.start_color);
				write_rgba(root, "end_color", item_node.end_color);
			});
		}

		//
		// コンフィグを読み込みます。
		//
		BOOL read_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_node(root, "text", [&](const n_json& root) {
				read_string(root, "font_name", property.text.font_name);
				read_int(root, "font_size", property.text.font_size);
				read_rgba(root, "color", property.text.color);
				read_rgba(root, "shadow_color", property.text.shadow_color);
				read_point(root, "padding", property.text.padding);
				read_point(root, "shadow_offset", property.text.shadow_offset);
				read_label(root, "horz_align", property.text.horz_align, property.text.c_horz_align.labels);
				read_label(root, "vert_align", property.text.vert_align, property.text.c_vert_align.labels);
			});

			read_node(root, "layer", [&](const n_json& root) {
				read_int(root, "top_space", property.layer.top_space);
				read_rgba(root, "odd_color", property.layer.odd_color);
				read_rgba(root, "even_color", property.layer.even_color);
			});

			read_node(root, "item", [&](const n_json& root) {
				read_int(root, "round_size", property.item.round_size);
				read_label(root, "round_mode", property.item.round_mode, property.c_round_mode.labels);
				read_rgba(root, "stroke_color", property.item.stroke_color);
				read_int(root, "stroke_width", property.item.stroke_width);

				for (size_t i = 0; i < property.item.c_item.c_max_size; i++)
					read_item_node(root, property.item.c_item.labels[i], property.item.nodes[i]);
			});

			read_node(root, "midpt", [&](const n_json& root) {
				read_rgba(root, "color", property.midpt.color);
				read_rgba(root, "line_color", property.midpt.line_color);
				read_int(root, "line_width", property.midpt.line_width);
			});

			read_node(root, "current_frame", [&](const n_json& root) {
				read_rgba(root, "line_color", property.current_frame.line_color);
				read_int(root, "line_width", property.current_frame.line_width);
			});

			read_node(root, "visible_area", [&](const n_json& root) {
				read_int(root, "round_size", property.visible_area.round_size);
				read_label(root, "round_mode", property.visible_area.round_mode, property.c_round_mode.labels);
				read_rgba(root, "color", property.visible_area.color);
				read_rgba(root, "stroke_color", property.visible_area.stroke_color);
				read_int(root, "stroke_width", property.visible_area.stroke_width);
			});

			read_window_pos(root, "view", view);

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_node(root, "text", [&](n_json& root) {
				write_string(root, "font_name", property.text.font_name);
				write_int(root, "font_size", property.text.font_size);
				write_rgba(root, "color", property.text.color);
				write_rgba(root, "shadow_color", property.text.shadow_color);
				write_point(root, "padding", property.text.padding);
				write_point(root, "shadow_offset", property.text.shadow_offset);
				write_label(root, "horz_align", property.text.horz_align, property.text.c_horz_align.labels);
				write_label(root, "vert_align", property.text.vert_align, property.text.c_vert_align.labels);
			});

			write_node(root, "layer", [&](n_json& root) {
				write_int(root, "top_space", property.layer.top_space);
				write_rgba(root, "odd_color", property.layer.odd_color);
				write_rgba(root, "even_color", property.layer.even_color);
			});

			write_node(root, "item", [&](n_json& root) {
				write_int(root, "round_size", property.item.round_size);
				write_label(root, "round_mode", property.item.round_mode, property.c_round_mode.labels);
				write_rgba(root, "stroke_color", property.item.stroke_color);
				write_int(root, "stroke_width", property.item.stroke_width);

				for (size_t i = 0; i < property.item.c_item.c_max_size; i++)
					write_item_node(root, property.item.c_item.labels[i], property.item.nodes[i]);
			});

			write_node(root, "midpt", [&](n_json& root) {
				write_rgba(root, "color", property.midpt.color);
				write_rgba(root, "line_color", property.midpt.line_color);
				write_int(root, "line_width", property.midpt.line_width);
			});

			write_node(root, "current_frame", [&](n_json& root) {
				write_rgba(root, "line_color", property.current_frame.line_color);
				write_int(root, "line_width", property.current_frame.line_width);
			});

			write_node(root, "visible_area", [&](n_json& root) {
				write_int(root, "round_size", property.visible_area.round_size);
				write_label(root, "round_mode", property.visible_area.round_mode, property.c_round_mode.labels);
				write_rgba(root, "color", property.visible_area.color);
				write_rgba(root, "stroke_color", property.visible_area.stroke_color);
				write_int(root, "stroke_width", property.visible_area.stroke_width);
			});

			write_window_pos(root, "view", view);

			return TRUE;
		}
	} controller;
}
