#pragma once

namespace apn::font_select
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

			return addin_dialog.update_controls();
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			get_size(root, "item_size", hive.item_size);
			get_size(root, "font_size", hive.font_size);
			get_string(root, "sample", hive.sample);
			get_bool(root, "single_line", hive.single_line);
			get_label(root, "paint.mode", paint.mode, paint.c_mode.labels);
			get_color(root, "paint.palette.normal.color.background", paint.palette[paint.c_style.c_normal].color.background);
			get_color(root, "paint.palette.normal.color.sample", paint.palette[paint.c_style.c_normal].color.sample);
			get_color(root, "paint.palette.normal.color.font_name", paint.palette[paint.c_style.c_normal].color.font_name);
			get_color(root, "paint.palette.select.color.background", paint.palette[paint.c_style.c_select].color.background);
			get_color(root, "paint.palette.select.color.sample", paint.palette[paint.c_style.c_select].color.sample);
			get_color(root, "paint.palette.select.color.font_name", paint.palette[paint.c_style.c_select].color.font_name);
			get_bool(root, "use_context_menu", hive.use_context_menu);
			get_bool(root, "use_backward", hive.use_backward);
			get_bool(root, "use_forward", hive.use_forward);
			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			set_size(root, "item_size", hive.item_size);
			set_size(root, "font_size", hive.font_size);
			set_string(root, "sample", hive.sample);
			set_bool(root, "single_line", hive.single_line);
			set_label(root, "paint.mode", paint.mode, paint.c_mode.labels);
			set_color(root, "paint.palette.normal.color.background", paint.palette[paint.c_style.c_normal].color.background);
			set_color(root, "paint.palette.normal.color.sample", paint.palette[paint.c_style.c_normal].color.sample);
			set_color(root, "paint.palette.normal.color.font_name", paint.palette[paint.c_style.c_normal].color.font_name);
			set_color(root, "paint.palette.select.color.background", paint.palette[paint.c_style.c_select].color.background);
			set_color(root, "paint.palette.select.color.sample", paint.palette[paint.c_style.c_select].color.sample);
			set_color(root, "paint.palette.select.color.font_name", paint.palette[paint.c_style.c_select].color.font_name);
			set_bool(root, "use_context_menu", hive.use_context_menu);
			set_bool(root, "use_backward", hive.use_backward);
			set_bool(root, "use_forward", hive.use_forward);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
