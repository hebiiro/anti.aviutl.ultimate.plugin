#pragma once

namespace apn::workspace
{
	//
	// このクラスはファイルからプリファレンスを保存します。
	//
	struct PreferenceSaver : StdConfigIO
	{
		//
		// 入出力オプションです。
		//
		uint32_t flags;

		//
		// コンストラクタです。
		//
		explicit PreferenceSaver(uint32_t flags = hive.c_preference_flag.c_default)
			: flags(flags)
		{
		}

		//
		// ノードにプリファレンスを保存します。
		//
		virtual BOOL write_node(n_json& node) override
		{
			MY_TRACE_FUNC("");

//			if (flags & hive.c_preference_flag.c_config)
			{
				set_int(node, "border_width", Pane::border_width);
				set_int(node, "caption_height", Pane::caption_height);
				set_int(node, "tab_height", Pane::tab_height);
				set_label(node, "tab_mode", Pane::tab_mode, Pane::c_tab_mode.labels);
				set_color(node, "fill_color", hive.fill_color);
				set_color(node, "border_color", hive.border_color);
				set_color(node, "hot_border_color", hive.hot_border_color);
				set_color(node, "active_caption_color", hive.active_caption_color);
				set_color(node, "active_caption_text_color", hive.active_caption_text_color);
				set_color(node, "inactive_caption_color", hive.inactive_caption_color);
				set_color(node, "inactive_caption_text_color", hive.inactive_caption_text_color);
				set_bool(node, "use_theme", hive.use_theme);
				set_bool(node, "scroll_force", hive.scroll_force);
				set_bool(node, "fullscreen_preview", hive.fullscreen_preview);
				set_bool(node, "show_tab_force", hive.show_tab_force);
				set_bool(node, "bypass_keyboard_message", hive.bypass_keyboard_message);
				set_bool(node, "ignore_ctrl_key_up", hive.ignore_ctrl_key_up);
				set_label(node, "layout_list_mode", hive.layout_list_mode, hive.c_layout_list_mode.labels);
			}

			// メインウィンドウを保存します。
			save_main_window(node);

			// サブウィンドウを保存します。
			save_sub_window(node);

			// フローティングシャトルを保存します。
			save_float_shuttle(node);

			return TRUE;
		}

		//
		// 指定された要素からシャトル名を読み込みます。
		//
		void set_shuttle_name(n_json& node, const std::string& name, const std::wstring& value)
		{
			set_string(node, name, Shuttle::strip_name(value));
		}

		//
		// ペインを保存します。
		//
		void save_pane(n_json& node, const std::shared_ptr<Pane>& pane)
		{
			MY_TRACE_FUNC("");

			auto current = pane->get_current_index();

			set_label(node, "split_mode", pane->split_mode, Pane::c_split_mode.labels);
			set_label(node, "origin", pane->origin, Pane::c_origin.labels);
			set_label(node, "caption_mode", pane->caption_mode, Pane::c_caption_mode.labels);
			set_int(node, "border", pane->border);
			set_bool(node, "is_border_locked", pane->is_border_locked);
			set_int(node, "current", current);

			auto c = pane->get_tab_count();
			std::vector<Shuttle*> dock_shuttles(c);
			for (decltype(c) i = 0; i < c; i++)
				dock_shuttles[i] = pane->get_shuttle(i);

			set_child_nodes(node, "dock_shuttle", dock_shuttles,
				[&](n_json& dock_shuttle_node, const auto& shuttle)
			{
				set_shuttle_name(dock_shuttle_node, "name", shuttle->name);

				return TRUE;
			});

			set_child_nodes(node, "pane", pane->children,
				[&](n_json& pane_node, const auto& child)
			{
				if (!child) return FALSE;

				save_pane(pane_node, child);

				return TRUE;
			});
		}

		//
		// ルートペインを保存します。
		//
		void save_root_pane(n_json& node, HWND hwnd)
		{
			MY_TRACE_FUNC("");

			auto root = SubWindow::get_root_pane(hwnd);

			n_json root_pane_node;
			set_bool(root_pane_node, "is_solid", root->is_solid);
			save_pane(root_pane_node, root);
			set_child_node(node, "root_pane", root_pane_node);
		}

		//
		// メインウィンドウを保存します。
		//
		void save_main_window(n_json& node)
		{
			MY_TRACE_FUNC("");

			n_json main_window_node;
			set_window(main_window_node, "placement", hive.main_window);
			save_root_pane(main_window_node, hive.main_window);
			set_child_node(node, "main_window", main_window_node);
		}

		//
		// サブウィンドウを保存します。
		//
		void save_sub_window(n_json& node)
		{
			MY_TRACE_FUNC("");

			set_child_nodes(node, "sub_window", SubWindow::collection,
				[&](n_json& sub_window_node, const auto& sub_window)
			{
				set_shuttle_name(sub_window_node, "name", sub_window->name);
				save_root_pane(sub_window_node, *sub_window);

				return TRUE;
			});
		}

		//
		// フローティングシャトルを保存します。
		//
		void save_float_shuttle(n_json& node)
		{
			MY_TRACE_FUNC("");

			set_child_nodes(node, "float_shuttle", shuttle_manager.collection,
				[&](n_json& float_shuttle_node, const auto& x)
			{
				const auto& shuttle = x.second;

				set_shuttle_name(float_shuttle_node, "name", shuttle->name);
				set_window(float_shuttle_node, "placement", *shuttle->float_container);

				return TRUE;
			});
		}
	};
}
