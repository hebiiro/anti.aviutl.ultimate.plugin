#pragma once

namespace apn::audio_visualizer
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
				// デフォルトのコウィンドウを作成します。

				std::wstring names[] = {
					L"左右Lv",
					L"左Lv",
					L"右Lv",
					L"パワースペクトル",
				};

				for (const auto& name : names)
				{
					// コウィンドウを作成します。
					auto co_window = co_window_manager.create_co_window(name);

					::ShowWindow(*co_window, SW_SHOW);
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
		// コンフィグが更新されたのでコントロールを更新します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			return addin_dialog.update_controls();
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// グローバル設定を読み込みます。
			get_label(root, "mode", hive.mode, hive.c_mode.labels);
			get_child(root, "range").get_to(hive.option.range);
			get_child(root, "marker").get_to(hive.option.marker);
			get_child(root, "spectre_option").get_to(hive.option.spectre_option);

			// アドインウィンドウのウィンドウ位置を読み込みます。
			get_window(root, "addin_window", addin_window);

			// コウィンドウを読み込みます。
			get_child_nodes(root, "co_window",
				[&](const n_json& co_window_node)
			{
				// 名前を読み込みます。
				std::wstring name;
				get_string(co_window_node, "name", name);
				if (name.empty()) return TRUE;

				// コウィンドウを作成します。
				auto co_window = co_window_manager.create_co_window(name);

				// ウィンドウ位置を読み込みます。
				get_window(co_window_node, "window", *co_window);

				return TRUE;
			});

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// グローバル設定を書き込みます。
			set_label(root, "mode", hive.mode, hive.c_mode.labels);
			set_child_node(root, "range", hive.option.range);
			set_child_node(root, "marker", hive.option.marker);
			set_child_node(root, "spectre_option", hive.option.spectre_option);

			// アドインウィンドウのウィンドウ位置を書き込みます。
			set_window(root, "addin_window", addin_window);

			// コウィンドウを書き込みます。
			set_child_nodes(root, "co_window", co_window_manager.collection,
				[&](n_json& co_window_node, const auto& co_window)
			{
				// 名前を書き込みます。
				auto name = co_window->name;
				set_string(co_window_node, "name", name);

				// ウィンドウ位置を書き込みます。
				set_window(co_window_node, "window", *co_window);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
