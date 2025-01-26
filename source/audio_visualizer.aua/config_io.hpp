#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
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
			read_label(root, "mode", hive.mode, hive.c_mode.labels);
			read_child(root, "range").get_to(hive.option.range);
			read_child(root, "marker").get_to(hive.option.marker);
			read_child(root, "spectre_option").get_to(hive.option.spectre_option);

			// アドインウィンドウのウィンドウ位置を読み込みます。
			read_window_pos(root, "addin_window", addin_window);

			// コウィンドウを読み込みます。
			read_child_nodes(root, "co_window",
				[&](const n_json& co_window_node, size_t i)
			{
				// 名前を読み込みます。
				std::wstring name;
				read_string(co_window_node, "name", name);
				if (name.empty()) return TRUE;

				// コウィンドウを作成します。
				auto co_window = co_window_manager.create_co_window(name);

				// ウィンドウ位置を読み込みます。
				read_window_pos(co_window_node, "window", *co_window);

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
			write_label(root, "mode", hive.mode, hive.c_mode.labels);
			write_child_node(root, "range", hive.option.range);
			write_child_node(root, "marker", hive.option.marker);
			write_child_node(root, "spectre_option", hive.option.spectre_option);

			// アドインウィンドウのウィンドウ位置を書き込みます。
			write_window_pos(root, "addin_window", addin_window);

			// コウィンドウを書き込みます。
			write_child_nodes(root, "co_window", co_window_manager.collection,
				[&](n_json& co_window_node, const auto& co_window, size_t i)
			{
				// 名前を書き込みます。
				auto name = co_window->name;
				write_string(co_window_node, "name", name);

				// ウィンドウ位置を書き込みます。
				write_window_pos(co_window_node, "window", *co_window);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
