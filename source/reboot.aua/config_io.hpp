#pragma once

namespace apn::reboot
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
		// コンフィグを書き込みます。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(hive.config_file_name, hive);
		}

		//
		// コンフィグが更新されたのでコントロールに適用します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			// コンフィグが更新されたのでパートも更新します。
			for (auto& part : Part::parts) part->on_update(&addin_dialog);

			return addin_dialog.update_controls();
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// パートの読み込み処理を実行します。
			for (auto& part : Part::parts)
			{
				n_json node;
				get_child_node(root, my::wide_to_cp(part->on_get_name(), CP_UTF8), node);
				part->on_read(node);
			}

			get_window(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// パートの書き込み処理を実行します。
			for (auto& part : Part::parts)
			{
				n_json node;
				part->on_write(node);
				set_child_node(root, my::wide_to_cp(part->on_get_name(), CP_UTF8), node);
			}

			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
