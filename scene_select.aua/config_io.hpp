#pragma once

namespace apn::scene_select
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

			addin_window.calc_layout();
			addin_window.redraw();

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			get_label(root, "size_mode", hive.size_mode, hive.c_size_mode.labels);
			get_label(root, "layout_mode", hive.layout_mode, hive.c_layout_mode.labels);
			get_int(root, "wrap", hive.wrap);
			get_int(root, "button_count", hive.button_count);
			get_size(root, "button_size", hive.button_size);
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

			set_label(root, "size_mode", hive.size_mode, hive.c_size_mode.labels);
			set_label(root, "layout_mode", hive.layout_mode, hive.c_layout_mode.labels);
			set_int(root, "wrap", hive.wrap);
			set_int(root, "button_count", hive.button_count);
			set_size(root, "button_size", hive.button_size);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
