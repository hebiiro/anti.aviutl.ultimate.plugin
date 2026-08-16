#pragma once

namespace apn::dark
{
	using config_io_base2_t = StdConfigIOUseHive<hive>;

	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct config_io_t : config_io_base2_t, entry_t::node_t
	{
		virtual BOOL on_init() { return config_io_base2_t::init(); }
		virtual BOOL on_exit() { return config_io_base2_t::exit(); }

		//
		// ノードからダークモード化の設定を読み込みます。
		//
		BOOL read_dark(n_json& root)
		{
			MY_TRACE_FUNC("");

//			read_string(root, "dark.custom_color_file_name", hive.dark.custom_color_file_name);
			read_bool(root, "dark.flag_exclude_comdlg32", hive.dark.flag_exclude_comdlg32);

			return TRUE;
		}

		//
		// ノードにダークモード化の設定を書き込みます。
		//
		BOOL write_dark(n_json& root)
		{
			MY_TRACE_FUNC("");

//			write_string(root, "dark.custom_color_file_name", hive.dark.custom_color_file_name);
			write_bool(root, "dark.flag_exclude_comdlg32", hive.dark.flag_exclude_comdlg32);

			return TRUE;
		}

		//
		// ノードからDirect2Dの設定を読み込みます。
		//
		BOOL read_d2d(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "d2d.flag_figure", hive.d2d.flag_figure);
			read_bool(root, "d2d.flag_text", hive.d2d.flag_text);

			return TRUE;
		}

		//
		// ノードにDirect2Dの設定を書き込みます。
		//
		BOOL write_d2d(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "d2d.flag_figure", hive.d2d.flag_figure);
			write_bool(root, "d2d.flag_text", hive.d2d.flag_text);

			return TRUE;
		}

		//
		// ノードからスクロールバーの設定を読み込みます。
		//
		BOOL read_scrollbar(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_int(root, "scrollbar.reduction", hive.scrollbar.reduction);
			read_bool(root, "scrollbar.flag_arrow_as_button", hive.scrollbar.flag_arrow_as_button);
			read_bool(root, "scrollbar.flag_has_gripper", hive.scrollbar.flag_has_gripper);

			return TRUE;
		}

		//
		// ノードにスクロールバーの設定を書き込みます。
		//
		BOOL write_scrollbar(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_int(root, "scrollbar.reduction", hive.scrollbar.reduction);
			write_bool(root, "scrollbar.flag_arrow_as_button", hive.scrollbar.flag_arrow_as_button);
			write_bool(root, "scrollbar.flag_has_gripper", hive.scrollbar.flag_has_gripper);

			return TRUE;
		}

		//
		// ノードからチェックボックスの設定を読み込みます。
		//
		BOOL read_checkbox(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "checkbox.flag_reversed", hive.checkbox.flag_reversed);

			return TRUE;
		}

		//
		// ノードにチェックボックスの設定を書き込みます。
		//
		BOOL write_checkbox(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "checkbox.flag_reversed", hive.checkbox.flag_reversed);

			return TRUE;
		}

		//
		// ノードから丸みの設定を読み込みます。
		//
		BOOL read_round(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "round.flag_use", hive.round.flag_use);
			read_int(root, "round.size", hive.round.size);

			return TRUE;
		}

		//
		// ノードに丸みの設定を書き込みます。
		//
		BOOL write_round(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "round.flag_use", hive.round.flag_use);
			write_int(root, "round.size", hive.round.size);

			return TRUE;
		}

		//
		// ノードから縁の設定を読み込みます。
		//
		BOOL read_border(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "border.flag_3d_edge", hive.border.flag_3d_edge);
			read_int(root, "border.width", hive.border.width);

			return TRUE;
		}

		//
		// ノードに縁の設定を書き込みます。
		//
		BOOL write_border(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "border.flag_3d_edge", hive.border.flag_3d_edge);
			write_int(root, "border.width", hive.border.width);

			return TRUE;
		}

		//
		// ノードからグラデーションの設定を読み込みます。
		//
		BOOL read_gradient(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "gradient.flag_use", hive.gradient.flag_use);
			read_bool(root, "gradient.flag_end_alpha", hive.gradient.flag_end_alpha);
			read_int(root, "gradient.end_alpha", hive.gradient.end_alpha);

			return TRUE;
		}

		//
		// ノードにグラデーションの設定を書き込みます。
		//
		BOOL write_gradient(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "gradient.flag_use", hive.gradient.flag_use);
			write_bool(root, "gradient.flag_end_alpha", hive.gradient.flag_end_alpha);
			write_int(root, "gradient.end_alpha", hive.gradient.end_alpha);

			return TRUE;
		}

		//
		// ノードから影の設定を読み込みます。
		//
		BOOL read_shadow(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "shadow.flag_use", hive.shadow.flag_use);
			read_bool(root, "shadow.flag_blur", hive.shadow.flag_blur);
			read_point(root, "shadow.offset", hive.shadow.offset);
			read_int(root, "shadow.size", hive.shadow.size);

			return TRUE;
		}

		//
		// ノードに影の設定を書き込みます。
		//
		BOOL write_shadow(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "shadow.flag_use", hive.shadow.flag_use);
			write_bool(root, "shadow.flag_blur", hive.shadow.flag_blur);
			write_point(root, "shadow.offset", hive.shadow.offset);
			write_int(root, "shadow.size", hive.shadow.size);

			return TRUE;
		}

		//
		// ノードから拡張編集の設定を読み込みます。
		//
		BOOL read_exedit(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "exedit.flag_draw_button_edge", hive.exedit.flag_draw_button_edge);
			read_int(root, "exedit.layer_border", hive.exedit.layer_border);
			read_bool(root, "exedit.flag_as_zebra", hive.exedit.flag_as_zebra);
			read_bool(root, "exedit.flag_draw_selected_layer", hive.exedit.flag_draw_selected_layer);
			read_bool(root, "exedit.flag_use_layer_color", hive.exedit.flag_use_layer_color);
			read_bool(root, "exedit.flag_use_layer_color_multi", hive.exedit.flag_use_layer_color_multi);
			read_int(root, "exedit.cursor.style", hive.exedit.cursor.style);
			read_int(root, "exedit.cursor.width", hive.exedit.cursor.width);
			read_int(root, "exedit.group.alpha", hive.exedit.group.alpha);

			return TRUE;
		}

		//
		// ノードに拡張編集の設定を書き込みます。
		//
		BOOL write_exedit(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "exedit.flag_draw_button_edge", hive.exedit.flag_draw_button_edge);
			write_int(root, "exedit.layer_border", hive.exedit.layer_border);
			write_bool(root, "exedit.flag_as_zebra", hive.exedit.flag_as_zebra);
			write_bool(root, "exedit.flag_draw_selected_layer", hive.exedit.flag_draw_selected_layer);
			write_bool(root, "exedit.flag_use_layer_color", hive.exedit.flag_use_layer_color);
			write_bool(root, "exedit.flag_use_layer_color_multi", hive.exedit.flag_use_layer_color_multi);
			write_int(root, "exedit.cursor.style", hive.exedit.cursor.style);
			write_int(root, "exedit.cursor.width", hive.exedit.cursor.width);
			write_int(root, "exedit.group.alpha", hive.exedit.group.alpha);

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_dark(root);
			read_d2d(root);
			read_scrollbar(root);
			read_checkbox(root);
			read_round(root);
			read_border(root);
			read_gradient(root);
			read_shadow(root);
			read_exedit(root);

			return __super::read_node(root);
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_dark(root);
			write_d2d(root);
			write_scrollbar(root);
			write_checkbox(root);
			write_round(root);
			write_border(root);
			write_gradient(root);
			write_shadow(root);
			write_exedit(root);

			return __super::write_node(root);
		}
	} config_io;
}
