#pragma once

namespace apn::volume_meter
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

			// コンフィグをサブスレッドにも適用します。
			return sub_thread::controller.set_paint_option(main_thread::model.paint_option);
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_label(root, "mode", hive.mode, hive.c_mode.labels);

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

			write_label(root, "mode", hive.mode, hive.c_mode.labels);

			{
				n_json main_thread_node;
				main_thread::controller.write_node(main_thread_node);
				write_child_node(root, "main_thread", main_thread_node);
			}

			return TRUE;
		}
	} config_io;
}
