#pragma once

namespace apn::filer
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

			// コンフィグファイルが存在しない場合は
			if (!std::filesystem::exists(hive.config_file_name))
			{
				// デフォルトのファイラを作成します。

				for (auto i = 0; i < 3; i++)
				{
					// ファイラを作成します。
					auto filer_window = filer_window_manager.create_filer_window(std::format(L"素材{}", i + 1).c_str(), TRUE);

					// ファイラを表示します。
//					::ShowWindow(*filer_window, SW_SHOW);
				}

				return TRUE;
			}
			else
			{
				return read_file(hive.config_file_name, hive);
			}
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
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// プロパティを読み込みます。
			get_bool(root, "use_common_dialog", hive.use_common_dialog);

			// アドインウィンドウのウィンドウ位置を読み込みます。
			workspace::share::get_window(root, "addin_window", addin_window);

			get_child_nodes(root, "filer",
				[&](const n_json& filer_node, size_t i)
			{
				// ファイラの名前を読み込みます。
				std::wstring name;
				get_string(filer_node, "name", name);

				// ファイラウィンドウを作成します。
				auto filer_window = filer_window_manager.create_filer_window(name.c_str(), FALSE);
				if (filer_window)
				{
					// ファイラウィンドウのウィンドウ位置を読み込みます。
					workspace::share::get_window(filer_node, "filer_window", *filer_window);
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
			set_bool(root, "use_common_dialog", hive.use_common_dialog);

			// アドインウィンドウのウィンドウ位置を書き込みます。
			workspace::share::set_window(root, "addin_window", addin_window);

			set_child_nodes(root, "filer", FilerWindow::collection,
				[&](n_json& filer_node, const auto& filer_window, size_t i)
			{
				// ファイラの名前を書き込みます。
				auto name = my::get_window_text(*filer_window);
				set_string(filer_node, "name", name);

				// ファイラウィンドウのウィンドウ位置を書き込みます。
				workspace::share::set_window(filer_node, "filer_window", *filer_window);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
