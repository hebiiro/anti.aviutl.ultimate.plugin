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
		// コンフィグを書き込みます。
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
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			get_int(root, "quality", hive.quality);
			get_label(root, "mode", hive.mode, hive.c_mode.labels);
			get_string(root, "file_name", hive.file_name);
			get_int(root, "index", hive.index);
			get_int(root, "number_width", hive.number_width);
			get_bool(root, "use_pargb", hive.use_pargb);
			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_int(root, "quality", hive.quality);
			set_label(root, "mode", hive.mode, hive.c_mode.labels);
			set_string(root, "file_name", hive.file_name);
			set_int(root, "index", hive.index);
			set_int(root, "number_width", hive.number_width);
			set_bool(root, "use_pargb", hive.use_pargb);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
