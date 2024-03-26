#pragma once

namespace apn::exedit_tweaker
{
	using namespace my::json;

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
			return __super::init(hive.instance);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			return __super::exit(hive.instance);
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_load(ptree& root) override
		{
			get_bool(root, "gradient_fill.enable", hive.gradient_fill.enable);
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
		// コンフィグをファイルに保存します。
		//
		virtual BOOL on_save(std::ofstream& ofs) override
		{
			ptree root;

			set_bool(root, "gradient_fill.enable", hive.gradient_fill.enable);
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

			write_json(ofs, root);

			return TRUE;
		}

		virtual int message_box(const std::wstring& text) override
		{
			return hive.message_box(text);
		}
	} config_io;
}
