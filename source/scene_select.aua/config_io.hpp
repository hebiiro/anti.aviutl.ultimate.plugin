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

			addin_window.calc_layout();
			addin_window.redraw();

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_label(root, "size_mode", hive.size_mode, hive.c_size_mode.labels);
			read_label(root, "layout_mode", hive.layout_mode, hive.c_layout_mode.labels);
			read_int(root, "wrap", hive.wrap);
			read_int(root, "button_count", hive.button_count);
			read_size(root, "button_size", hive.button_size);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_label(root, "size_mode", hive.size_mode, hive.c_size_mode.labels);
			write_label(root, "layout_mode", hive.layout_mode, hive.c_layout_mode.labels);
			write_int(root, "wrap", hive.wrap);
			write_int(root, "button_count", hive.button_count);
			write_size(root, "button_size", hive.button_size);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
