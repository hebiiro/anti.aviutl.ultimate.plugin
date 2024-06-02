#pragma once

namespace apn::workspace
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
		// 指定されたファイルからサブプロセスの設定を読み込みます。
		//
		void get_sub_process(const ptree& node, const std::string& name, Hive::SubProcess& sub_process)
		{
			get_bool(node, name + ".dock", sub_process.dock);
			get_string(node, name + ".run", sub_process.run);

			if (!sub_process.run.empty())
			{
				std::filesystem::path path(sub_process.run);

				::ShellExecute(hive.main_window, 0, path.c_str(), 0, path.parent_path().c_str(), SW_SHOW);
			}
		}

		//
		// 指定されたファイルにサブプロセスの設定を保存します。
		//
		void set_sub_process(ptree& node, const std::string& name, const Hive::SubProcess& sub_process)
		{
			set_bool(node, name + ".dock", sub_process.dock);
			set_string(node, name + ".run", sub_process.run);
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			get_bool(root, "use_fullscreen_player", hive.use_fullscreen_player);
			get_shortcut_key(root, "shortcut_key.show_caption", hive.shortcut_key.show_caption);
			get_sub_process(root, "psdtoolkit", hive.psdtoolkit);
			get_sub_process(root, "bouyomisan", hive.bouyomisan);
			get_sub_process(root, "console", hive.console);

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			set_bool(root, "use_fullscreen_player", hive.use_fullscreen_player);
			set_shortcut_key(root, "shortcut_key.show_caption", hive.shortcut_key.show_caption);
			set_sub_process(root, "psdtoolkit", hive.psdtoolkit);
			set_sub_process(root, "bouyomisan", hive.bouyomisan);
			set_sub_process(root, "console", hive.console);

			return TRUE;
		}
	} config_io;
}
