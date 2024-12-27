﻿#pragma once

namespace apn::one_for_text
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

			auto config_file_name = hive.config_file_name;
			if(!std::filesystem::exists(config_file_name))
				config_file_name = magi.get_default_config_file_name(hive.instance);
			return read_file(config_file_name, hive);
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

			get_string(root, "text_file_name", hive.text_file_name);
			get_string(root, "text_editor_path", hive.text_editor_path);
			get_bool(root, "current_scene_only", hive.current_scene_only);
			get_int(root, "layer_front", hive.layer_front);
			get_int(root, "layer_back", hive.layer_back);
			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			set_string(root, "text_file_name", hive.text_file_name);
			set_string(root, "text_editor_path", hive.text_editor_path);
			set_bool(root, "current_scene_only", hive.current_scene_only);
			set_int(root, "layer_front", hive.layer_front);
			set_int(root, "layer_back", hive.layer_back);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}