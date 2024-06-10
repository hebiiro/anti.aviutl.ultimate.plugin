#pragma once

namespace apn::workspace
{
	void get_child_nodes(const ptree& node, const std::string& name, auto func)
	{
		if (auto child_nodes_op = node.get_child_optional(name))
		{
			for (const auto& pair : child_nodes_op.value())
			{
				const auto& child_node = pair.second;

				func(child_node);
			}
		}
	}

	void set_child_nodes(ptree& node, const std::string& name, const auto& collection, auto func)
	{
		ptree child_nodes;
		{
			for (const auto& value : collection)
			{
				ptree child_node;
				{
					func(child_node, value);
				}
				child_nodes.push_back(std::make_pair("", child_node));
			}
		}
		node.put_child(name, child_nodes);
	}

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
		void get_sub_process(const ptree& node, std::shared_ptr<Hive::SubProcess::Node>& sub_process)
		{
			get_bool(node, "active", sub_process->active);
			get_string(node, "type", sub_process->type);
			get_string(node, "name", sub_process->name);
			get_string(node, "class_name", sub_process->class_name);
			get_string(node, "window_name", sub_process->window_name);
			get_child_nodes(node, "exit_mode",
				[&](const ptree& exit_mode_node)
				{
					std::wstring exit_mode;
					get_string(exit_mode_node, "", exit_mode);
					if (!exit_mode.empty()) sub_process->exit_mode.emplace(exit_mode);
				});
			get_string(node, "path", sub_process->path);
		}

		//
		// 指定されたファイルにサブプロセスの設定を保存します。
		//
		void set_sub_process(ptree& node, const std::shared_ptr<Hive::SubProcess::Node>& sub_process)
		{
			set_bool(node, "active", sub_process->active);
			if (!sub_process->type.empty()) set_string(node, "type", sub_process->type);
			if (!sub_process->name.empty()) set_string(node, "name", sub_process->name);
			if (!sub_process->class_name.empty()) set_string(node, "class_name", sub_process->class_name);
			if (!sub_process->window_name.empty()) set_string(node, "window_name", sub_process->window_name);
			if (!sub_process->exit_mode.empty()) set_child_nodes(node, "exit_mode", sub_process->exit_mode,
				[&](ptree& exit_mode_node, const auto& exit_mode)
				{
					set_string(exit_mode_node, "", exit_mode);
				});
			if (!sub_process->path.empty()) set_string(node, "path", sub_process->path);
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			get_bool(root, "use_fullscreen_preview", hive.use_fullscreen_preview);
			get_shortcut_key(root, "shortcut_key.show_caption", hive.shortcut_key.show_caption);
			get_child_nodes(root, "sub_process",
				[&](const ptree& sub_process_node)
				{
					auto sub_process = std::make_shared<Hive::SubProcess::Node>();
					hive.sub_process.collection.emplace_back(sub_process);
					get_sub_process(sub_process_node, sub_process);
				});

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			set_bool(root, "use_fullscreen_preview", hive.use_fullscreen_preview);
			set_shortcut_key(root, "shortcut_key.show_caption", hive.shortcut_key.show_caption);
			set_child_nodes(root, "sub_process", hive.sub_process.collection,
				[&](ptree& sub_process_node, const auto& sub_process)
				{
					set_sub_process(sub_process_node, sub_process);
				});

			return TRUE;
		}
	} config_io;
}
