#pragma once

namespace apn::text_split
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

			get_label(root, "shift_mode", hive.shift_mode, hive.c_shift_mode.labels);
			get_int(root, "shift_offset", hive.shift_offset);
			get_int(root, "shift_range", hive.shift_range);
			get_label(root, "each_mode", hive.each_mode, hive.c_each_mode.labels);
			get_label(root, "fit_mode", hive.fit_mode, hive.c_fit_mode.labels);
			get_int(root, "fit_width", hive.fit_width);
			get_int(root, "erase_orig_item", hive.erase_orig_item);
			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_label(root, "shift_mode", hive.shift_mode, hive.c_shift_mode.labels);
			set_int(root, "shift_offset", hive.shift_offset);
			set_int(root, "shift_range", hive.shift_range);
			set_label(root, "each_mode", hive.each_mode, hive.c_each_mode.labels);
			set_label(root, "fit_mode", hive.fit_mode, hive.c_fit_mode.labels);
			set_int(root, "fit_width", hive.fit_width);
			set_int(root, "erase_orig_item", hive.erase_orig_item);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
