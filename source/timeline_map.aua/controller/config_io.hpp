#pragma once

namespace apn::timeline_map::controller
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct config_io_t : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// コンフィグファイルのファイルパスを決定しておきます。
			model::property.config_file_name =
				magi.get_config_file_name(model::property.instance);
			MY_TRACE_STR(model::property.config_file_name);

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

			return read_file(model::property.config_file_name, view::overview);
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(model::property.config_file_name, view::overview);
		}

		//
		// コンフィグが更新されたのでコントロールを更新します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			if (view::paint_option_dialog)
				view::paint_option_dialog.update_controls();

			// リソースを再作成します。
			return model::state.recreate_resources();
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_child_node(root, "text", [&](const n_json& root) {
				read_string(root, "font_name", model::property.text.font_name);
				read_int(root, "font_size", model::property.text.font_size);
				read_rgba(root, "color", model::property.text.color);
				read_rgba(root, "shadow_color", model::property.text.shadow_color);
				read_point(root, "padding", model::property.text.padding);
				read_point(root, "shadow_offset", model::property.text.shadow_offset);
				read_label(root, "horz_align", model::property.text.horz_align, model::property.text.c_horz_align.labels);
				read_label(root, "vert_align", model::property.text.vert_align, model::property.text.c_vert_align.labels);
				read_bool(root, "flag_draw_text", model::property.text.flag_draw_text);
				read_bool(root, "flag_draw_shadow", model::property.text.flag_draw_shadow);
				read_int(root, "min_draw_width", model::property.text.min_draw_width);

			});

			read_child_node(root, "layer", [&](const n_json& root) {
				read_int(root, "top_space", model::property.layer.top_space);
				read_rgba(root, "odd_color", model::property.layer.odd_color);
				read_rgba(root, "even_color", model::property.layer.even_color);
				read_rgba(root, "undisp_color", model::property.layer.undisp_color);
				read_rgba(root, "undisp_stroke_color", model::property.layer.undisp_stroke_color);
				read_int(root, "undisp_stroke_width", model::property.layer.undisp_stroke_width);
				read_rgba(root, "locked_stroke_color", model::property.layer.locked_stroke_color);
				read_int(root, "locked_stroke_width", model::property.layer.locked_stroke_width);
				read_rgba(root, "coordlink_stroke_color", model::property.layer.coordlink_stroke_color);
				read_int(root, "coordlink_stroke_width", model::property.layer.coordlink_stroke_width);
				read_rgba(root, "clip_stroke_color", model::property.layer.clip_stroke_color);
				read_int(root, "clip_stroke_width", model::property.layer.clip_stroke_width);
			});

			read_child_node(root, "item", [&](const n_json& root) {
				read_int(root, "round_size", model::property.item.round_size);
				read_label(root, "round_mode", model::property.item.round_mode, model::property.c_round_mode.labels);
				read_rgba(root, "stroke_color", model::property.item.stroke_color);
				read_int(root, "stroke_width", model::property.item.stroke_width);

				for (size_t i = 0; i < model::property.item.c_item.c_max_size; i++)
					read_item_node(root, model::property.item.c_item.labels[i], model::property.item.nodes[i]);
			});

			read_child_node(root, "midpt", [&](const n_json& root) {
				read_rgba(root, "color", model::property.midpt.color);
				read_rgba(root, "line_color", model::property.midpt.line_color);
				read_int(root, "line_width", model::property.midpt.line_width);
			});

			read_child_node(root, "current_frame", [&](const n_json& root) {
				read_rgba(root, "line_color", model::property.current_frame.line_color);
				read_int(root, "line_width", model::property.current_frame.line_width);
			});

			read_child_node(root, "visible_area", [&](const n_json& root) {
				read_int(root, "round_size", model::property.visible_area.round_size);
				read_label(root, "round_mode", model::property.visible_area.round_mode, model::property.c_round_mode.labels);
				read_rgba(root, "color", model::property.visible_area.color);
				read_rgba(root, "stroke_color", model::property.visible_area.stroke_color);
				read_int(root, "stroke_width", model::property.visible_area.stroke_width);
			});

			read_child_node(root, "control_range", [&](const n_json& root) {
				read_rgba(root, "color", model::property.control_range.color);
				read_rgba(root, "stroke_color", model::property.control_range.stroke_color);
				read_int(root, "stroke_width", model::property.control_range.stroke_width);
			});

			read_child_node(root, "etc", [&](const n_json& root) {
				read_bool(root, "flag_immediate", model::property.etc.flag_immediate);
			});

			read_window_pos(root, "overview", view::overview);

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_child_node(root, "text", [&](n_json& root) {
				write_string(root, "font_name", model::property.text.font_name);
				write_int(root, "font_size", model::property.text.font_size);
				write_rgba(root, "color", model::property.text.color);
				write_rgba(root, "shadow_color", model::property.text.shadow_color);
				write_point(root, "padding", model::property.text.padding);
				write_point(root, "shadow_offset", model::property.text.shadow_offset);
				write_label(root, "horz_align", model::property.text.horz_align, model::property.text.c_horz_align.labels);
				write_label(root, "vert_align", model::property.text.vert_align, model::property.text.c_vert_align.labels);
				write_bool(root, "flag_draw_text", model::property.text.flag_draw_text);
				write_bool(root, "flag_draw_shadow", model::property.text.flag_draw_shadow);
				write_int(root, "min_draw_width", model::property.text.min_draw_width);
			});

			write_child_node(root, "layer", [&](n_json& root) {
				write_int(root, "top_space", model::property.layer.top_space);
				write_rgba(root, "odd_color", model::property.layer.odd_color);
				write_rgba(root, "even_color", model::property.layer.even_color);
				write_rgba(root, "undisp_color", model::property.layer.undisp_color);
				write_rgba(root, "undisp_stroke_color", model::property.layer.undisp_stroke_color);
				write_int(root, "undisp_stroke_width", model::property.layer.undisp_stroke_width);
				write_rgba(root, "locked_stroke_color", model::property.layer.locked_stroke_color);
				write_int(root, "locked_stroke_width", model::property.layer.locked_stroke_width);
				write_rgba(root, "coordlink_stroke_color", model::property.layer.coordlink_stroke_color);
				write_int(root, "coordlink_stroke_width", model::property.layer.coordlink_stroke_width);
				write_rgba(root, "clip_stroke_color", model::property.layer.clip_stroke_color);
				write_int(root, "clip_stroke_width", model::property.layer.clip_stroke_width);
			});

			write_child_node(root, "item", [&](n_json& root) {
				write_int(root, "round_size", model::property.item.round_size);
				write_label(root, "round_mode", model::property.item.round_mode, model::property.c_round_mode.labels);
				write_rgba(root, "stroke_color", model::property.item.stroke_color);
				write_int(root, "stroke_width", model::property.item.stroke_width);

				for (size_t i = 0; i < model::property.item.c_item.c_max_size; i++)
					write_item_node(root, model::property.item.c_item.labels[i], model::property.item.nodes[i]);
			});

			write_child_node(root, "midpt", [&](n_json& root) {
				write_rgba(root, "color", model::property.midpt.color);
				write_rgba(root, "line_color", model::property.midpt.line_color);
				write_int(root, "line_width", model::property.midpt.line_width);
			});

			write_child_node(root, "current_frame", [&](n_json& root) {
				write_rgba(root, "line_color", model::property.current_frame.line_color);
				write_int(root, "line_width", model::property.current_frame.line_width);
			});

			write_child_node(root, "visible_area", [&](n_json& root) {
				write_int(root, "round_size", model::property.visible_area.round_size);
				write_label(root, "round_mode", model::property.visible_area.round_mode, model::property.c_round_mode.labels);
				write_rgba(root, "color", model::property.visible_area.color);
				write_rgba(root, "stroke_color", model::property.visible_area.stroke_color);
				write_int(root, "stroke_width", model::property.visible_area.stroke_width);
			});

			write_child_node(root, "control_range", [&](n_json& root) {
				write_rgba(root, "color", model::property.control_range.color);
				write_rgba(root, "stroke_color", model::property.control_range.stroke_color);
				write_int(root, "stroke_width", model::property.control_range.stroke_width);
			});

			write_child_node(root, "etc", [&](n_json& root) {
				write_bool(root, "flag_immediate", model::property.etc.flag_immediate);
			});

			write_window_pos(root, "overview", view::overview);

			return TRUE;
		}

		//
		// RGBAを読み込みます。
		//
		void read_rgba(const n_json& root, const std::string& node_name, rgba_t& rgba)
		{
			read_child_node(root, node_name, [&](const n_json& root) {
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
			write_child_node(root, node_name, [&](n_json& root) {
				write_int(root, "r", rgba.r);
				write_int(root, "g", rgba.g);
				write_int(root, "b", rgba.b);
				write_int(root, "a", rgba.a);
			});
		}

		//
		// アイテムノードを読み込みます。
		//
		void read_item_node(const n_json& root, const std::string& node_name, model::property_t::item_t::node_t& item_node)
		{
			read_child_node(root, node_name, [&](const n_json& root) {
				read_rgba(root, "start_color", item_node.start_color);
				read_rgba(root, "end_color", item_node.end_color);
			});
		}

		//
		// アイテムノードを書き込みます。
		//
		void write_item_node(n_json& root, const std::string& node_name, const model::property_t::item_t::node_t& item_node)
		{
			write_child_node(root, node_name, [&](n_json& root) {
				write_rgba(root, "start_color", item_node.start_color);
				write_rgba(root, "end_color", item_node.end_color);
			});
		}
	} config_io;
}
