#pragma once

namespace apn::item_align
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

			read_bool(root, "use_current_frame", hive.use_current_frame);
			read_string(root, "align_time", hive.align_time);
			read_string(root, "stretch_time", hive.stretch_time);
			read_string(root, "shift_time", hive.shift_time);
			read_string(root, "relative_space_time", hive.relative_space_time);
			read_string(root, "absolute_space_time", hive.absolute_space_time);
			read_int(root, "move_vert_layer", hive.move_vert_layer);
			read_int(root, "sub_time_mode", hive.sub_time_mode);
			read_int(root, "align_sub_time", hive.align_sub_time);
			read_int(root, "stretch_sub_time", hive.stretch_sub_time);
			read_int(root, "shift_sub_time", hive.shift_sub_time);
			read_int(root, "relative_space_sub_time", hive.relative_space_sub_time);
			read_int(root, "absolute_space_sub_time", hive.absolute_space_sub_time);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_bool(root, "use_current_frame", hive.use_current_frame);
			write_string(root, "align_time", hive.align_time);
			write_string(root, "stretch_time", hive.stretch_time);
			write_string(root, "shift_time", hive.shift_time);
			write_string(root, "relative_space_time", hive.relative_space_time);
			write_string(root, "absolute_space_time", hive.absolute_space_time);
			write_int(root, "move_vert_layer", hive.move_vert_layer);
			write_int(root, "sub_time_mode", hive.sub_time_mode);
			write_int(root, "align_sub_time", hive.align_sub_time);
			write_int(root, "stretch_sub_time", hive.stretch_sub_time);
			write_int(root, "shift_sub_time", hive.shift_sub_time);
			write_int(root, "relative_space_sub_time", hive.relative_space_sub_time);
			write_int(root, "absolute_space_sub_time", hive.absolute_space_sub_time);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
