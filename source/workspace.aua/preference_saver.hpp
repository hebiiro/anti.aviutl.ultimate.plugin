#pragma once

namespace apn::workspace
{
	//
	// このクラスはファイルからプリファレンスを書き込みます。
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
		// ノードにプリファレンスを書き込みます。
		//
		virtual BOOL write_node(n_json& node) override
		{
			MY_TRACE_FUNC("");

//			if (flags & hive.c_preference_flag.c_config)
			{
				write_tav(node, "tav", hive.tav);
				set_int(node, "tab_height", hive.tab_height);
				set_int(node, "tab_space", hive.tab_space);
				set_int(node, "caption_height", hive.caption_height);
				set_int(node, "border_width", hive.border_width);
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

			// メインウィンドウを書き込みます。
			write_main_window(node);

			// サブウィンドウを書き込みます。
			write_sub_window(node);

			// フローティングシャトルを書き込みます。
			write_float_shuttle(node);

			return TRUE;
		}

		//
		// シャトル名を書き込みます。
		//
		void write_shuttle_name(n_json& node, const std::string& name, const std::wstring& value)
		{
			set_string(node, name, Shuttle::strip_name(value));
		}

		//
		// タブの属性を書き込みます。
		//
		void write_tav(n_json& tav_node, Hive::Tav& tav)
		{
			MY_TRACE_FUNC("");

			set_label(tav_node, "display_mode", tav.display_mode, tav.c_display_mode.labels);
			set_label(tav_node, "select_mode", tav.select_mode, tav.c_select_mode.labels);
			set_label(tav_node, "stretch_mode", tav.stretch_mode, tav.c_stretch_mode.labels);
			set_label(tav_node, "location", tav.location, tav.c_location.labels);
			set_label(tav_node, "node_align", tav.node_align, tav.c_node_align.labels);
		}

		//
		// タブの属性を書き込みます。
		//
		void write_tav(n_json& node, const std::string& name, Hive::Tav& tav)
		{
			MY_TRACE_FUNC("");

			n_json tav_node;
			write_tav(tav_node, tav);
			set_child_node(node, name, tav_node);
		}

		//
		// ドロワーを読み込みます。
		//
		void write_drawer(n_json& drawer_node, Drawer& drawer)
		{
			MY_TRACE_FUNC("");

			// ドロワーの属性を書き込みます。
			set_bool(drawer_node, "inside", drawer.inside);

			// タブの属性を書き込みます。
			return write_tav(drawer_node, drawer);
		}

		//
		// ペインを書き込みます。
		//
		void write_pane(n_json& node, const std::shared_ptr<Pane>& pane)
		{
			MY_TRACE_FUNC("");

			// タブの属性を書き込みます。
			write_tav(node, "tav", pane->tav);

			// ドロワーを書き込みます。
			{
				set_child_nodes(node, "drawer", pane->drawers,
					[&](n_json& drawer_node, const auto& drawer, size_t i)
				{
					// ドロワーが無効の場合は何もしません。
					if (!drawer) return TRUE;

					// ドロワーの属性を書き込みます。
					write_drawer(drawer_node, *drawer);

					// タブ項目を書き込みます。
					set_child_nodes(drawer_node, "shuttle", drawer->nodes,
						[&](n_json& shuttle_node, const auto& node, size_t i)
					{
						// シャトルが取得できる場合は
						if (auto shuttle = Shuttle::get_pointer(node->hwnd))
						{
							// シャトル名を書き込みます。
							write_shuttle_name(shuttle_node, "name", shuttle->name);
						}

						return TRUE;
					});

					return TRUE;
				});
			}

			// ペインの属性を書き込みます。
			auto current = pane->get_current_index();

			set_label(node, "split_mode", pane->split_mode, hive.pane.c_split_mode.labels);
			set_label(node, "origin", pane->origin, hive.pane.c_origin.labels);
			set_label(node, "caption_mode", pane->caption_mode, hive.pane.c_caption_mode.labels);
			set_label(node, "caption_location", pane->caption_location, hive.pane.c_caption_location.labels);
			set_int(node, "border", pane->border);
			set_bool(node, "is_border_locked", pane->is_border_locked);
			set_int(node, "current", current);

			// 一旦ドッキングシャトルのコレクションを作成します。
			auto c = pane->get_tab_count();
			std::vector<Shuttle*> dock_shuttles(c);
			for (decltype(c) i = 0; i < c; i++)
			{
				if (auto shuttle = pane->get_shuttle(i))
					dock_shuttles[i] = shuttle;
			}

			// ドッキングシャトルを書き込みます。
			set_child_nodes(node, "dock_shuttle", dock_shuttles,
				[&](n_json& dock_shuttle_node, const auto& shuttle, size_t i)
			{
				write_shuttle_name(dock_shuttle_node, "name", shuttle->name);

				return TRUE;
			});

			// 子ペインを書き込みます。
			set_child_nodes(node, "pane", pane->children,
				[&](n_json& pane_node, const auto& child, size_t i)
			{
				if (!child) return FALSE;

				write_pane(pane_node, child);

				return TRUE;
			});
		}

		//
		// ルートペインを書き込みます。
		//
		void write_root_pane(n_json& node, HWND hwnd)
		{
			MY_TRACE_FUNC("");

			auto root = SubWindow::get_root_pane(hwnd);

			n_json root_pane_node;
			set_bool(root_pane_node, "is_solid", root->is_solid);
			write_pane(root_pane_node, root);
			set_child_node(node, "root_pane", root_pane_node);
		}

		//
		// メインウィンドウを書き込みます。
		//
		void write_main_window(n_json& node)
		{
			MY_TRACE_FUNC("");

			n_json main_window_node;
			set_window(main_window_node, "placement", hive.main_window);
			write_root_pane(main_window_node, hive.main_window);
			set_child_node(node, "main_window", main_window_node);
		}

		//
		// サブウィンドウを書き込みます。
		//
		void write_sub_window(n_json& node)
		{
			MY_TRACE_FUNC("");

			set_child_nodes(node, "sub_window", SubWindow::collection,
				[&](n_json& sub_window_node, const auto& sub_window, size_t i)
			{
				write_shuttle_name(sub_window_node, "name", sub_window->name);
				write_root_pane(sub_window_node, *sub_window);

				return TRUE;
			});
		}

		//
		// フローティングシャトルを書き込みます。
		//
		void write_float_shuttle(n_json& node)
		{
			MY_TRACE_FUNC("");

			set_child_nodes(node, "float_shuttle", shuttle_manager.collection,
				[&](n_json& float_shuttle_node, const auto& pair, size_t i)
			{
				const auto& shuttle = pair.second;

				write_shuttle_name(float_shuttle_node, "name", shuttle->name);
				set_window(float_shuttle_node, "placement", *shuttle->float_container);

				return TRUE;
			});
		}
	};
}
