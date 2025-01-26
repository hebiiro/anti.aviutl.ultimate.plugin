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

			read_int(root, "quality", hive.quality);
			read_label(root, "mode", hive.mode, hive.c_mode.labels);
			read_string(root, "file_name", hive.file_name);
			read_int(root, "index", hive.index);
			read_int(root, "number_width", hive.number_width);
			read_bool(root, "use_pargb", hive.use_pargb);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_int(root, "quality", hive.quality);
			write_label(root, "mode", hive.mode, hive.c_mode.labels);
			write_string(root, "file_name", hive.file_name);
			write_int(root, "index", hive.index);
			write_int(root, "number_width", hive.number_width);
			write_bool(root, "use_pargb", hive.use_pargb);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
