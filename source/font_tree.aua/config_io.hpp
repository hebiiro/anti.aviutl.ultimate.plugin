#pragma once

namespace apn::font_tree
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
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

			// 予め既存の設定を消去します。
			recent_manager.clear();
			favorite_manager.clear();

			read_string(root, "display_name_format_v2", hive.display_name_format);
			read_string(root, "separator_format", hive.separator_format);
			read_window_pos(root, "addin_window", addin_window);

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

			write_string(root, "display_name_format_v2", hive.display_name_format);
			write_string(root, "separator_format", hive.separator_format);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
