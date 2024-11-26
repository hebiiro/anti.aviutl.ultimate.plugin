#pragma once

namespace apn::font_tree
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// TRUEの場合は読み込みに成功しています。
		//
		BOOL is_read = FALSE;

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

			// 既存の設定を消去します。
			recent_manager.clear();
			favorite_manager.clear();

			return is_read = read_file(hive.config_file_name, hive);
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			// コンフィグを読み込めていないのに書き込みを行うと殆どの設定が初期値に戻ってしまいます。
			// これを防ぐために、コンフィグを読み込めていない場合は書き込みをしないようにします。
			if (!is_read) return FALSE;

			return write_file(hive.config_file_name, hive);
		}

		//
		// コンフィグが更新されたのでコントロールに適用します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			get_string(root, "display_name_format", hive.display_name_format);
			get_string(root, "separator_format", hive.separator_format);
			get_window(root, "addin_window", addin_window);

			preview_manager.read(root);
			recent_manager.read(root);
			favorite_manager.read(root);

			MY_TRACE("コンフィグの読み込みに成功しました\n");

			favorite_manager.expand(TVI_ROOT);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			preview_manager.write(root);
			recent_manager.write(root);
			favorite_manager.write(root);

			set_string(root, "display_name_format", hive.display_name_format);
			set_string(root, "separator_format", hive.separator_format);
			set_window(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
