#pragma once

namespace apn::image_export
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

			get_int(root, "quality", hive.quality);
			get_label(root, "mode", hive.mode, hive.c_mode.labels);
			get_string(root, "file_name", hive.file_name);
			get_int(root, "index", hive.index);
			get_int(root, "number_width", hive.number_width);
			get_window(root, "addin_window", addin_window);

			addin_dialog.update_controls();

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			using namespace my::json;

			set_int(root, "quality", hive.quality);
			set_label(root, "mode", hive.mode, hive.c_mode.labels);
			set_string(root, "file_name", hive.file_name);
			set_int(root, "index", hive.index);
			set_int(root, "number_width", hive.number_width);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
