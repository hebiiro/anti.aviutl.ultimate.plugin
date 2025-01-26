#pragma once

namespace apn::settings_browser
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
		// この仮想関数はコンフィグが更新されたときに呼び出されます。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			// コンフィグをダイアログコントロールに適用します。
			addin_dialog.update_controls();

			// ブラウザで新しいURLをブラウズします。
			return app->navigate();
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_bool(root, "auto_get", hive.auto_get);
			read_string(root, "url", hive.url);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_bool(root, "auto_get", hive.auto_get);
			write_string(root, "url", hive.url);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
