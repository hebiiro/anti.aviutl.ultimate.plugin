#pragma once

namespace apn::dialog_size
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIOUseHive<hive>
	{

		//
		// ノードからプリセットを読み込みます。
		//
		BOOL read_preset(n_json& root)
		{
			MY_TRACE_FUNC("");

			hive.video_size_collection.clear();
			hive.video_rate_collection.clear();
			hive.audio_rate_collection.clear();

			n_json new_file_node;
			read_child_node(root, "new_file", new_file_node);

			read_child_nodes(new_file_node, "video_size",
				[&](const n_json& video_size_node, size_t i)
			{
				auto& video_size = hive.video_size_collection.emplace_back();
				read_string(video_size_node, "width", video_size.width);
				read_string(video_size_node, "height", video_size.height);
				read_string(video_size_node, "name", video_size.display_name);

				return TRUE;
			});

			read_child_nodes(new_file_node, "video_rate",
				[&](const n_json& video_rate_node, size_t i)
			{
				auto& video_rate = hive.video_rate_collection.emplace_back();
				read_string(video_rate_node, "rate", video_rate.rate);
				read_string(video_rate_node, "name", video_rate.display_name);

				return TRUE;
			});

			read_child_nodes(new_file_node, "audio_rate",
				[&](const n_json& audio_rate_node, size_t i)
			{
				auto& audio_rate = hive.audio_rate_collection.emplace_back();
				read_string(audio_rate_node, "rate", audio_rate.rate);
				read_string(audio_rate_node, "name", audio_rate.display_name);

				return TRUE;
			});

			return TRUE;
		}

		//
		// ノードにプリセットを書き込みます。
		//
		BOOL write_preset(n_json& root)
		{
			MY_TRACE_FUNC("");

			n_json new_file_node;

			write_child_nodes(new_file_node, "video_size",
				hive.video_size_collection,
				[&](n_json& video_size_node, const auto& video_size, size_t i)
			{
				write_string(video_size_node, "width", video_size.width);
				write_string(video_size_node, "height", video_size.height);
				write_string(video_size_node, "name", video_size.display_name);

				return TRUE;
			});

			write_child_nodes(new_file_node, "video_rate",
				hive.video_rate_collection,
				[&](n_json& video_rate_node, const auto& video_rate, size_t i)
			{
				write_string(video_rate_node, "rate", video_rate.rate);
				write_string(video_rate_node, "name", video_rate.display_name);

				return TRUE;
			});

			write_child_nodes(new_file_node, "audio_rate",
				hive.audio_rate_collection,
				[&](n_json& audio_rate_node, const auto& audio_rate, size_t i)
			{
				write_string(audio_rate_node, "rate", audio_rate.rate);
				write_string(audio_rate_node, "name", audio_rate.display_name);

				return TRUE;
			});

			write_child_node(root, "new_file", new_file_node);

			return TRUE;
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
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			read_child_nodes(root, "custom_template",
				[&](const n_json& custom_template_node, size_t i)
			{
				if (i >= std::size(hive.custom_templates))
					return FALSE;

				read_bool(custom_template_node, "active", hive.custom_templates[i].active);
//				read_string(custom_template_node, "name", hive.custom_templates[i].target_display_name);

				return TRUE;
			});

			read_window_pos(root, "addin_window", addin_window);
			read_preset(root);

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root) override
		{
			MY_TRACE_FUNC("");

			write_child_nodes(root, "custom_template", hive.custom_templates,
				[&](n_json& custom_template_node, const auto& custom_template, size_t i)
			{
				write_bool(custom_template_node, "active", custom_template.active);
				write_string(custom_template_node, "name", custom_template.target_display_name);

				return TRUE;
			});

			write_window_pos(root, "addin_window", addin_window);
			write_preset(root);

			return TRUE;
		}
	} config_io;
}
