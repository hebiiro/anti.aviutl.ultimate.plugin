#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// ファイルパスを読み込みます。
		// 相対パスだった場合は絶対パスに変換します。
		//
		inline static void get_file_name(const n_json& node, const std::string& name, std::filesystem::path& path)
		{
			my::json::get_file_name(node, name, path);
			if (path.is_relative())
				path = hive.assets_folder_name / path;
		}

		//
		// ファイルパスを書き込みます。
		// 可能ならアセットフォルダからの相対パスで書き込みます。
		//
		inline static void set_file_name(n_json& node, const std::string& name, const std::filesystem::path& path)
		{
			my::json::set_file_name(node, name,
				path.lexically_proximate(hive.assets_folder_name));
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			hive.config_file_name = magi.get_config_file_name(hive.instance);
			MY_TRACE_STR(hive.config_file_name);

			hive.assets_folder_name = magi.get_assets_file_name(hive.c_name);
			MY_TRACE_STR(hive.assets_folder_name);

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

			auto config_file_name = hive.config_file_name;
			if(!std::filesystem::exists(config_file_name))
				config_file_name = magi.get_default_config_file_name(hive.instance);
			return read_file(config_file_name, hive);
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

			return TRUE;
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			// ビジュアルのマップを作成します。
			// キーはビジュアルのウィンドウ名です。
			auto visuals = visual_manager.create_visual_map();

			// ビジュアルを読み込みます。
			get_child_nodes(root, "visual",
				[&](const n_json& visual_node, size_t i)
			{
				// ビジュアルの名前を取得します。
				std::wstring name;
				get_string(visual_node, "name", name);
				MY_TRACE_STR(name);

				// 名前からビジュアルを取得します。
				auto it = visuals.find(name);
				if (it == visuals.end()) return TRUE;

				auto& visual = it->second;

				// スキームファイルのパスを取得します。
				get_file_name(visual_node, "scheme_file_name", visual->scheme_file_name);

				// プリファレンスを取得します。
				get_child_node(visual_node, "preference", visual->editor.preference);

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

			// ビジュアルのマップを作成します。
			// キーはビジュアルのウィンドウ名です。
			auto visuals = visual_manager.create_visual_map();

			// ビジュアルを書き込みます。
			set_child_nodes(root, "visual", visuals,
				[&](n_json& visual_node, const auto& pair, size_t i)
			{
				const auto& name = pair.first;
				const auto& visual = pair.second;

				// 名前を書き込みます。
				set_string(visual_node, "name", name);

				// スキームファイルのパスを書き込みます。
				set_file_name(visual_node, "scheme_file_name", visual->scheme_file_name);

				// プリファレンスを書き込みます。
				set_child_node(visual_node, "preference", visual->editor.preference);

				return TRUE;
			});

			return TRUE;
		}
	} config_io;
}
