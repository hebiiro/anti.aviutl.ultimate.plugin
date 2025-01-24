#pragma once

namespace apn::filer_ui
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			std::map<std::wstring, std::shared_ptr<FilerDialog>> filers;
			for (auto& filer_dialog : FilerDialog::collection)
				filers[my::get_window_text(*filer_dialog)] = filer_dialog;

			read_child_nodes(root, "filer",
				[&](const n_json& filer_node, size_t i)
			{
				std::wstring name;
				read_string(filer_node, "name", name);
				MY_TRACE_STR(name);

				auto it = filers.find(name);
				if (it == filers.end()) return TRUE;

				auto& filer_dialog = it->second;

				filer_dialog->read(filer_node);

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

			write_child_nodes(root, "filer", FilerDialog::collection,
				[&](n_json& filer_node, const auto& filer_dialog, size_t i)
			{
				auto name = my::get_window_text(*filer_dialog);
				write_string(filer_node, "name", name);
				MY_TRACE_STR(name);

				filer_dialog->write(filer_node);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
