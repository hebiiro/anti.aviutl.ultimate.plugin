#pragma once

namespace apn::filer
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

			// プロパティを読み込みます。
			read_bool(root, "use_common_dialog", hive.use_common_dialog);

			// アドインウィンドウのウィンドウ位置を読み込みます。
			read_window_pos(root, "addin_window", addin_window);

			read_child_nodes(root, "filer",
				[&](const n_json& filer_node, size_t i)
			{
				// ファイラの名前を読み込みます。
				std::wstring name;
				read_string(filer_node, "name", name);

				// ファイラウィンドウを作成します。
				auto filer_window = filer_window_manager.create_filer_window(name.c_str(), FALSE);
				if (filer_window)
				{
					// ファイラウィンドウのウィンドウ位置を読み込みます。
					read_window_pos(filer_node, "filer_window", *filer_window);
				}

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

			// プロパティを書き込みます。
			write_bool(root, "use_common_dialog", hive.use_common_dialog);

			// アドインウィンドウのウィンドウ位置を書き込みます。
			write_window_pos(root, "addin_window", addin_window);

			write_child_nodes(root, "filer", FilerWindow::collection,
				[&](n_json& filer_node, const auto& filer_window, size_t i)
			{
				// ファイラの名前を書き込みます。
				auto name = my::get_window_text(*filer_window);
				write_string(filer_node, "name", name);

				// ファイラウィンドウのウィンドウ位置を書き込みます。
				write_window_pos(filer_node, "filer_window", *filer_window);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
