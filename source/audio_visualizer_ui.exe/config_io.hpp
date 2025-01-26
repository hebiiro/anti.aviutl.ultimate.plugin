#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{
		//
		// ファイルパスを読み込みます。
		// 相対パスだった場合は絶対パスに変換します。
		//
		inline static void read_file_name(const n_json& node, const std::string& name,
			std::filesystem::path& path, const std::filesystem::path& origin_path)
		{
			my::json::read_file_name(node, name, path);

			if (path.is_relative()) path = origin_path / path;
		}

		//
		// ファイルパスを書き込みます。
		// 可能なら原点からの相対パスで書き込みます。
		//
		inline static void write_file_name(n_json& node, const std::string& name,
			const std::filesystem::path& path, const std::filesystem::path& origin_path)
		{
			my::json::write_file_name(node, name, path.lexically_proximate(origin_path));
		}

		//
		// パスの原点を返します。
		//
		inline static std::filesystem::path get_origin_path()
		{
			return magi.get_assets_file_name(hive.c_name);
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
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// パスの原点を取得します。
			auto origin_path = get_origin_path();

			// ビジュアルのマップを作成します。
			// キーはビジュアルのウィンドウ名です。
			auto visuals = visual_manager.create_visual_map();

			// ビジュアルを読み込みます。
			read_child_nodes(root, "visual",
				[&](const n_json& visual_node, size_t i)
			{
				// ビジュアルの名前を取得します。
				std::wstring name;
				read_string(visual_node, "name", name);
				MY_TRACE_STR(name);

				// 名前からビジュアルを検索します。
				auto it = visuals.find(name);
				if (it == visuals.end()) return TRUE;

				// ビジュアルを取得します。
				auto& visual = it->second;

				// スキームファイルのパスを読み込みます。
				read_file_name(visual_node, "scheme_file_name", visual->scheme_file_name, origin_path);

				// プリファレンスを取得します。
				read_child_node(visual_node, "preference", visual->editor.preference);

				// スキームを読み込みます。
				visual->read_scheme();

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

			// パスの原点を取得します。
			auto origin_path = get_origin_path();

			// ビジュアルのマップを作成します。
			// キーはビジュアルのウィンドウ名です。
			auto visuals = visual_manager.create_visual_map();

			// ビジュアルを書き込みます。
			write_child_nodes(root, "visual", visuals,
				[&](n_json& visual_node, const auto& pair, size_t i)
			{
				const auto& name = pair.first;
				const auto& visual = pair.second;

				// 名前を書き込みます。
				write_string(visual_node, "name", name);

				// スキームファイルのパスを書き込みます。
				write_file_name(visual_node, "scheme_file_name", visual->scheme_file_name, origin_path);

				// プリファレンスを書き込みます。
				write_child_node(visual_node, "preference", visual->editor.preference);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
