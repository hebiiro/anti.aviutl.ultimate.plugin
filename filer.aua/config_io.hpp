#pragma once

namespace apn::filer
{
	using namespace my::json;

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

				// デフォルトのウィンドウ矩形です。
				auto rc = RECT { 0, 0, 800, 600 };

				for (auto i = 0; i < 3; i++)
				{
					// 矩形を少しずらします。
					::OffsetRect(&rc, 24, 24);

					// ファイラを作成します。
					auto filer_window = filer_window_manager.create_filer_window(std::format(L"素材{}", i + 1).c_str(), TRUE, rc);

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

			// プロパティを読み込みます。
			get_bool(root, "use_common_dialog", hive.use_common_dialog);

			// アドインウィンドウのウィンドウ位置を読み込みます。
			workspace::share::get_window(root, "addin_window", addin_window);

			// アドインウィンドウのウィンドウ矩形を取得します。
			auto rc = my::get_window_rect(addin_window);

			if (auto filer_nodes_op = root.get_child_optional("filer"))
			{
				for (const auto& pair : filer_nodes_op.value())
				{
					const auto& filer_node = pair.second;

					// ファイラの名前を読み込みます。
					std::wstring name;
					get_string(filer_node, "name", name);

					// ファイラウィンドウを作成します。
					auto filer_window = filer_window_manager.create_filer_window(name.c_str(), FALSE, rc);
					if (filer_window)
					{
						// ファイラウィンドウのウィンドウ位置を読み込みます。
						workspace::share::get_window(filer_node, "filer_window", *filer_window);
					}
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

			// プロパティを書き込みます。
			set_bool(root, "use_common_dialog", hive.use_common_dialog);

			// アドインウィンドウのウィンドウ位置を書き込みます。
			workspace::share::set_window(root, "addin_window", addin_window);

			ptree filer_nodes;
			{
				for (auto& filer_window : FilerWindow::collection)
				{
					ptree filer_node;
					{
						// ファイラの名前を書き込みます。
						auto name = my::get_window_text(*filer_window);
						set_string(filer_node, "name", name);

						// ファイラウィンドウのウィンドウ位置を書き込みます。
						workspace::share::set_window(filer_node, "filer_window", *filer_window);
					}
					filer_nodes.push_back(std::make_pair("", filer_node));
				}
			}
			root.put_child("filer", filer_nodes);

			return TRUE;
		}
	} config_io;
}
