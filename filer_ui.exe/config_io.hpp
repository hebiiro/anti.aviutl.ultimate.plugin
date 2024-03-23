#pragma once

namespace apn::filer_ui
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
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			std::map<std::wstring, std::shared_ptr<FilerDialog>> filers;
			for (auto& filer_dialog : FilerDialog::collection)
				filers[my::get_window_text(*filer_dialog)] = filer_dialog;

			if (auto filer_nodes_op = root.get_child_optional("filer"))
			{
				for (const auto& pair : filer_nodes_op.value())
				{
					const auto& filer_node = pair.second;

					std::wstring name;
					get_string(filer_node, "name", name);
					MY_TRACE_STR(name);

					auto it = filers.find(name);
					if (it == filers.end()) continue;

					auto& filer_dialog = it->second;

					filer_dialog->read(filer_node);
				}
			}

			return TRUE;
		}

		//
		// ノードにコンフィグを保存します。
		//
		virtual BOOL write_node(ptree& root) override
		{
			MY_TRACE_FUNC("");

			ptree filer_nodes;
			{
				for (auto& filer_dialog : FilerDialog::collection)
				{
					ptree filer_node;
					{
						auto name = my::get_window_text(*filer_dialog);
						set_string(filer_node, "name", name);
						MY_TRACE_STR(name);

						filer_dialog->write(filer_node);
					}
					filer_nodes.push_back(std::make_pair("", filer_node));
				}
			}
			root.put_child("filer", filer_nodes);

			return TRUE;
		}
	} config_io;
}
