#pragma once

namespace apn::workspace
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
		//
		// 指定されたファイルからサブプロセスの設定を読み込みます。
		//
		void get_sub_process(const n_json& node, std::shared_ptr<Hive::SubProcess::Node>& sub_process)
		{
			read_bool(node, "active", sub_process->active);
			read_string(node, "type", sub_process->type);
			read_string(node, "name", sub_process->name);
			read_string(node, "class_name", sub_process->class_name);
			read_string(node, "window_name", sub_process->window_name);
			read_child_nodes(node, "exit_mode",
				[&](const n_json& exit_mode_node, size_t i)
				{
					std::wstring exit_mode;
					read_string(exit_mode_node, exit_mode);

					if (!exit_mode.empty())
						sub_process->exit_mode.emplace(exit_mode);

					return TRUE;
				});
			read_string(node, "path", sub_process->path);
		}

		//
		// 指定されたファイルにサブプロセスの設定を保存します。
		//
		void set_sub_process(n_json& node, const std::shared_ptr<Hive::SubProcess::Node>& sub_process)
		{
			write_bool(node, "active", sub_process->active);
			if (!sub_process->type.empty()) write_string(node, "type", sub_process->type);
			if (!sub_process->name.empty()) write_string(node, "name", sub_process->name);
			if (!sub_process->class_name.empty()) write_string(node, "class_name", sub_process->class_name);
			if (!sub_process->window_name.empty()) write_string(node, "window_name", sub_process->window_name);
			if (!sub_process->exit_mode.empty()) write_child_nodes(node, "exit_mode", sub_process->exit_mode,
				[&](n_json& exit_mode_node, const auto& exit_mode, size_t i)
				{
					write_string(exit_mode_node, exit_mode);

					return TRUE;
				});
			if (!sub_process->path.empty()) write_string(node, "path", sub_process->path);
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_bool(root, "use_fullscreen_preview", hive.use_fullscreen_preview);
			read_bool(root, "omit_window_initialize", hive.omit_window_initialize);
			read_shortcut_key(root, "shortcut_key.show_caption", hive.shortcut_key.show_caption);
			read_child_nodes(root, "sub_process",
				[&](const n_json& sub_process_node, size_t i)
				{
					auto sub_process = std::make_shared<Hive::SubProcess::Node>();
					hive.sub_process.collection.emplace_back(sub_process);
					get_sub_process(sub_process_node, sub_process);

					return TRUE;
				});

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_bool(root, "use_fullscreen_preview", hive.use_fullscreen_preview);
			write_bool(root, "omit_window_initialize", hive.omit_window_initialize);
			write_shortcut_key(root, "shortcut_key.show_caption", hive.shortcut_key.show_caption);
			write_child_nodes(root, "sub_process", hive.sub_process.collection,
				[&](n_json& sub_process_node, const auto& sub_process, size_t i)
				{
					set_sub_process(sub_process_node, sub_process);

					return TRUE;
				});

			return TRUE;
		}
	} config_io;
}
