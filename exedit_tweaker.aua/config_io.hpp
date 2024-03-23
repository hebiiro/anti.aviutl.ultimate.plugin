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
		// コンフィグを保存します。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			get_bool(root, "gradient_fill.enabled", hive.gradient_fill.enabled);
			get_color(root, "gradient_fill.edge.inner.color", hive.gradient_fill.edge.inner.color);
			get_int(root, "gradient_fill.edge.inner.size.cx", hive.gradient_fill.edge.inner.size.cx);
			get_int(root, "gradient_fill.edge.inner.size.cy", hive.gradient_fill.edge.inner.size.cy);
			get_color(root, "gradient_fill.edge.outer.color", hive.gradient_fill.edge.outer.color);
			get_int(root, "gradient_fill.edge.outer.size.cx", hive.gradient_fill.edge.outer.size.cx);
			get_int(root, "gradient_fill.edge.outer.size.cy", hive.gradient_fill.edge.outer.size.cy);

			get_color(root, "selection.fill.color", hive.selection.fill.color);
			get_color(root, "selection.stroke.color", hive.selection.stroke.color);
			get_color(root, "selection.background.color", hive.selection.background.color);

			get_color(root, "layer.bound.left.color", hive.layer.bound.left.color);
			get_color(root, "layer.bound.top.color", hive.layer.bound.top.color);
			get_color(root, "layer.bound.right.color", hive.layer.bound.right.color);
			get_color(root, "layer.bound.bottom.color", hive.layer.bound.bottom.color);
			get_color(root, "layer.separator.color", hive.layer.separator.color);

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			set_bool(root, "gradient_fill.enabled", hive.gradient_fill.enabled);
			set_color(root, "gradient_fill.edge.inner.color", hive.gradient_fill.edge.inner.color);
			set_int(root, "gradient_fill.edge.inner.size.cx", hive.gradient_fill.edge.inner.size.cx);
			set_int(root, "gradient_fill.edge.inner.size.cy", hive.gradient_fill.edge.inner.size.cy);
			set_color(root, "gradient_fill.edge.outer.color", hive.gradient_fill.edge.outer.color);
			set_int(root, "gradient_fill.edge.outer.size.cx", hive.gradient_fill.edge.outer.size.cx);
			set_int(root, "gradient_fill.edge.outer.size.cy", hive.gradient_fill.edge.outer.size.cy);

			set_color(root, "selection.fill.color", hive.selection.fill.color);
			set_color(root, "selection.stroke.color", hive.selection.stroke.color);
			set_color(root, "selection.background.color", hive.selection.background.color);

			set_color(root, "layer.bound.left.color", hive.layer.bound.left.color);
			set_color(root, "layer.bound.top.color", hive.layer.bound.top.color);
			set_color(root, "layer.bound.right.color", hive.layer.bound.right.color);
			set_color(root, "layer.bound.bottom.color", hive.layer.bound.bottom.color);
			set_color(root, "layer.separator.color", hive.layer.separator.color);

			return TRUE;
		}
	} config_io;
}
