#pragma once

namespace apn::timeline_map
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct config_io_t : StdConfigIOUseHive<hive>
	{
		//
		// コンフィグが更新されたのでコントロールを更新します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			if (main_thread::paint_option_dialog)
				main_thread::paint_option_dialog.update_controls();

			// リソースを再作成します。
			return main_thread::state.recreate_resources();
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			{
				n_json main_thread_node;
				read_child_node(root, "main_thread", main_thread_node);
				main_thread::controller.read_node(main_thread_node);
			}

			return TRUE;
		}

		//
		// コンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			{
				n_json main_thread_node;
				main_thread::controller.write_node(main_thread_node);
				write_child_node(root, "main_thread", main_thread_node);
			}

			return TRUE;
		}
	} config_io;
}
