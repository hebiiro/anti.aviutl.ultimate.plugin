#pragma once

namespace apn::item_align
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct config_io : StdConfigIO
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

			read_bool(root, "flag_use_current_frame", hive.flag_use_current_frame);
			read_int(root, "sub_time_mode", hive.time.mode);
			read_string(root, "align_time", hive.time.elements[hive.time.c_index.c_align].sec);
			read_string(root, "stretch_time", hive.time.elements[hive.time.c_index.c_stretch].sec);
			read_string(root, "relative_space_time", hive.time.elements[hive.time.c_index.c_relative_space].sec);
			read_string(root, "absolute_space_time", hive.time.elements[hive.time.c_index.c_absolute_space].sec);
			read_string(root, "shift_time", hive.time.elements[hive.time.c_index.c_shift].sec);
			read_int(root, "align_time", hive.time.elements[hive.time.c_index.c_align].frame);
			read_int(root, "stretch_time", hive.time.elements[hive.time.c_index.c_stretch].frame);
			read_int(root, "relative_space_time", hive.time.elements[hive.time.c_index.c_relative_space].frame);
			read_int(root, "absolute_space_time", hive.time.elements[hive.time.c_index.c_absolute_space].frame);
			read_int(root, "shift_time", hive.time.elements[hive.time.c_index.c_shift].frame);
			read_int(root, "move_vert_layer", hive.move_vert_layer);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_bool(root, "flag_use_current_frame", hive.flag_use_current_frame);
			write_int(root, "sub_time_mode", hive.time.mode);
			write_string(root, "align_time", hive.time.elements[hive.time.c_index.c_align].sec);
			write_string(root, "stretch_time", hive.time.elements[hive.time.c_index.c_stretch].sec);
			write_string(root, "relative_space_time", hive.time.elements[hive.time.c_index.c_relative_space].sec);
			write_string(root, "absolute_space_time", hive.time.elements[hive.time.c_index.c_absolute_space].sec);
			write_string(root, "shift_time", hive.time.elements[hive.time.c_index.c_shift].sec);
			write_int(root, "align_time", hive.time.elements[hive.time.c_index.c_align].frame);
			write_int(root, "stretch_time", hive.time.elements[hive.time.c_index.c_stretch].frame);
			write_int(root, "relative_space_time", hive.time.elements[hive.time.c_index.c_relative_space].frame);
			write_int(root, "absolute_space_time", hive.time.elements[hive.time.c_index.c_absolute_space].frame);
			write_int(root, "shift_time", hive.time.elements[hive.time.c_index.c_shift].frame);
			write_int(root, "move_vert_layer", hive.move_vert_layer);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
