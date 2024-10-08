#pragma once

namespace apn::exedit_tweaker
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
				n_json gradient_fill_node;
				get_child_node(root, "gradient_fill", gradient_fill_node);
				get_bool(gradient_fill_node, "enabled", hive.gradient_fill.enabled);
				{
					n_json edge_node;
					get_child_node(gradient_fill_node, "edge", edge_node);
					{
						n_json inner_node;
						get_child_node(edge_node, "inner", inner_node);
						get_color(inner_node, "color", hive.gradient_fill.edge.inner.color);
						get_size(inner_node, "size", hive.gradient_fill.edge.inner.size);
					}
					{
						n_json outer_node;
						get_child_node(edge_node, "outer", outer_node);
						get_color(outer_node, "color", hive.gradient_fill.edge.outer.color);
						get_size(outer_node, "size", hive.gradient_fill.edge.outer.size);
					}
				}
			}

			{
				n_json selection_node;
				get_child_node(root, "selection", selection_node);
				{
					n_json fill_node;
					get_child_node(selection_node, "fill", fill_node);
					get_color(fill_node, "color", hive.selection.fill.color);
				}
				{
					n_json stroke_node;
					get_child_node(selection_node, "stroke", stroke_node);
					get_color(stroke_node, "color", hive.selection.stroke.color);
				}
				{
					n_json background_node;
					get_child_node(selection_node, "background", background_node);
					get_color(background_node, "color", hive.selection.background.color);
				}
			}

			{
				n_json layer_node;
				get_child_node(root, "layer", layer_node);
				{
					n_json bound_node;
					get_child_node(layer_node, "bound", bound_node);
					{
						n_json left_node;
						get_child_node(bound_node, "left", left_node);
						get_color(left_node, "color", hive.layer.bound.left.color);
					}
					{
						n_json top_node;
						get_child_node(bound_node, "top", top_node);
						get_color(top_node, "color", hive.layer.bound.top.color);
					}
					{
						n_json right_node;
						get_child_node(bound_node, "right", right_node);
						get_color(right_node, "color", hive.layer.bound.right.color);
					}
					{
						n_json bottom_node;
						get_child_node(bound_node, "bottom", bottom_node);
						get_color(bottom_node, "color", hive.layer.bound.bottom.color);
					}
				}
				{
					n_json separator_node;
					get_child_node(layer_node, "separator", separator_node);
					get_color(separator_node, "color", hive.layer.separator.color);
				}
			}

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			{
				n_json gradient_fill_node;
				set_bool(gradient_fill_node, "enabled", hive.gradient_fill.enabled);
				{
					n_json edge_node;
					{
						n_json inner_node;
						set_color(inner_node, "color", hive.gradient_fill.edge.inner.color);
						set_size(inner_node, "size", hive.gradient_fill.edge.inner.size);
						set_child_node(edge_node, "inner", inner_node);
					}
					{
						n_json outer_node;
						set_color(outer_node, "color", hive.gradient_fill.edge.outer.color);
						set_size(outer_node, "size", hive.gradient_fill.edge.outer.size);
						set_child_node(edge_node, "outer", outer_node);
					}
					set_child_node(gradient_fill_node, "edge", edge_node);
				}
				set_child_node(root, "gradient_fill", gradient_fill_node);
			}

			{
				n_json selection_node;
				{
					n_json fill_node;
					set_color(fill_node, "color", hive.selection.fill.color);
					set_child_node(selection_node, "fill", fill_node);
				}
				{
					n_json stroke_node;
					set_color(stroke_node, "color", hive.selection.stroke.color);
					set_child_node(selection_node, "stroke", stroke_node);
				}
				{
					n_json background_node;
					set_color(background_node, "color", hive.selection.background.color);
					set_child_node(selection_node, "background", background_node);
				}
				set_child_node(root, "selection", selection_node);
			}

			{
				n_json layer_node;
				{
					n_json bound_node;
					{
						n_json left_node;
						set_color(left_node, "color", hive.layer.bound.left.color);
						set_child_node(bound_node, "left", left_node);
					}
					{
						n_json top_node;
						set_color(top_node, "color", hive.layer.bound.top.color);
						set_child_node(bound_node, "top", top_node);
					}
					{
						n_json right_node;
						set_color(right_node, "color", hive.layer.bound.right.color);
						set_child_node(bound_node, "right", right_node);
					}
					{
						n_json bottom_node;
						set_color(bottom_node, "color", hive.layer.bound.bottom.color);
						set_child_node(bound_node, "bottom", bottom_node);
					}
					set_child_node(layer_node, "bound", bound_node);
				}
				{
					n_json separator_node;
					set_color(separator_node, "color", hive.layer.separator.color);
					set_child_node(layer_node, "separator", separator_node);
				}
				set_child_node(root, "layer", layer_node);
			}

			return TRUE;
		}
	} config_io;
}
