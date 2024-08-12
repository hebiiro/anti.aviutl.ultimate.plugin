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

			get_bool(root, "use_current_frame", hive.use_current_frame);
			get_string(root, "align_time", hive.align_time);
			get_string(root, "stretch_time", hive.stretch_time);
			get_string(root, "shift_time", hive.shift_time);
			get_string(root, "relative_space_time", hive.relative_space_time);
			get_string(root, "absolute_space_time", hive.absolute_space_time);
			get_int(root, "move_vert_layer", hive.move_vert_layer);
			get_int(root, "sub_time_mode", hive.sub_time_mode);
			get_int(root, "align_sub_time", hive.align_sub_time);
			get_int(root, "stretch_sub_time", hive.stretch_sub_time);
			get_int(root, "shift_sub_time", hive.shift_sub_time);
			get_int(root, "relative_space_sub_time", hive.relative_space_sub_time);
			get_int(root, "absolute_space_sub_time", hive.absolute_space_sub_time);
			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_bool(root, "use_current_frame", hive.use_current_frame);
			set_string(root, "align_time", hive.align_time);
			set_string(root, "stretch_time", hive.stretch_time);
			set_string(root, "shift_time", hive.shift_time);
			set_string(root, "relative_space_time", hive.relative_space_time);
			set_string(root, "absolute_space_time", hive.absolute_space_time);
			set_int(root, "move_vert_layer", hive.move_vert_layer);
			set_int(root, "sub_time_mode", hive.sub_time_mode);
			set_int(root, "align_sub_time", hive.align_sub_time);
			set_int(root, "stretch_sub_time", hive.stretch_sub_time);
			set_int(root, "shift_sub_time", hive.shift_sub_time);
			set_int(root, "relative_space_sub_time", hive.relative_space_sub_time);
			set_int(root, "absolute_space_sub_time", hive.absolute_space_sub_time);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
