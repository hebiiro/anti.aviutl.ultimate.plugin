#pragma once

namespace apn::filter_relocate::controller
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct config_io_t : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// コンフィグファイルのファイルパスを決定しておきます。
			model::property.config_file_name =
				magi.get_config_file_name(model::property.instance);
			MY_TRACE_STR(model::property.config_file_name);

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

			return read_file(model::property.config_file_name, view::main_window);
		}

		//
		// コンフィグを書き込みます。
		//
		BOOL write()
		{
			MY_TRACE_FUNC("");

			return write_file(model::property.config_file_name, view::main_window);
		}

		//
		// コンフィグが更新されたのでコントロールに適用します。
		//
		virtual BOOL update() override
		{
			MY_TRACE_FUNC("");

			return view::main_dialog.update_controls();
		}

		//
		// ラムダ式を使用してノードを読み込みます。
		//
		inline static void read_node(const n_json& root, const std::string& node_name, auto func)
		{
			n_json node;
			read_child_node(root, node_name, node);
			return func(node);
		}

		//
		// ラムダ式を使用してノードを書き込みます。
		//
		inline static void write_node(n_json& root, const std::string& node_name, auto func)
		{
			n_json node;
			my::scope_exit scope_exit([&]() { write_child_node(root, node_name, node); });
			return func(node);
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_window_pos(root, "main_window", view::main_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_window_pos(root, "main_window", view::main_window);

			return TRUE;
		}
	} config_io;
}
