#pragma once

namespace apn::item_affine
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

			return addin_dialog.update_controls();
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
		// 分数を読み込みます。
		//
		inline static void read_fraction(const n_json& root, const std::string& node_name, fraction_t& fraction)
		{
			read_node(root, node_name, [&](const n_json& root) {
				read_int(root, "num", fraction.num);
				read_int(root, "den", fraction.den);
			});
		}

		//
		// 分数を書き込みます。
		//
		inline static void write_fraction(n_json& root, const std::string& node_name, const fraction_t& fraction)
		{
			write_node(root, node_name, [&](n_json& root) {
				write_int(root, "num", fraction.num);
				write_int(root, "den", fraction.den);
			});
		}

		//
		// 分数を読み込みます。
		//
		inline static void read_affine(const n_json& root, const std::string& node_name, affine_t& affine)
		{
			read_node(root, node_name, [&](const n_json& root) {
				read_fraction(root, "mul", affine.mul);
				read_fraction(root, "add", affine.add);
				read_fraction(root, "base", affine.base);
				read_fraction(root, "exp", affine.exp);
				read_label(root, "target", affine.target, affine.c_target.labels);
			});
		}

		//
		// 分数を書き込みます。
		//
		inline static void write_affine(n_json& root, const std::string& node_name, const affine_t& affine)
		{
			write_node(root, node_name, [&](n_json& root) {
				write_fraction(root, "mul", affine.mul);
				write_fraction(root, "add", affine.add);
				write_fraction(root, "base", affine.base);
				write_fraction(root, "exp", affine.exp);
				write_label(root, "target", affine.target, affine.c_target.labels);
			});
		}

		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_affine(root, "pos", hive.pos);
			read_affine(root, "len", hive.len);
			read_label(root, "orig", hive.orig, hive.c_orig.labels);
			read_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_affine(root, "pos", hive.pos);
			write_affine(root, "len", hive.len);
			write_label(root, "orig", hive.orig, hive.c_orig.labels);
			write_window_pos(root, "addin_window", addin_window);

			return TRUE;
		}
	} config_io;
}
