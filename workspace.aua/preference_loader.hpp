#pragma once

namespace apn::workspace
{
	//
	// このクラスはファイルからプリファレンスを読み込みます。
	//
	inline struct PreferenceLoader
	{
		//
		// 指定された要素からプリファレンスを読み込みます。
		//
		BOOL load(const ptree& node, BOOL layout_only)
		{
			MY_TRACE_FUNC("{}", layout_only);

			if (!layout_only)
			{
				get_int(node, "border_width", Pane::border_width);
				get_int(node, "caption_height", Pane::caption_height);
				get_int(node, "tab_height", Pane::tab_height);
				get_label(node, "tab_mode", Pane::tab_mode, Pane::c_tab_mode.labels);
				get_color(node, "fill_color", hive.fill_color);
				get_color(node, "border_color", hive.border_color);
				get_color(node, "hot_border_color", hive.hot_border_color);
				get_color(node, "active_caption_color", hive.active_caption_color);
				get_color(node, "active_caption_text_color", hive.active_caption_text_color);
				get_color(node, "inactive_caption_color", hive.inactive_caption_color);
				get_color(node, "inactive_caption_text_color", hive.inactive_caption_text_color);
				get_bool(node, "use_theme", hive.use_theme);
				get_bool(node, "scroll_force", hive.scroll_force);
				get_bool(node, "maximum_play", hive.maximum_play);
				get_bool(node, "show_tab_force", hive.show_tab_force);
			}

			// 事前にサブウィンドウを読み込みます。
			pre_load_sub_window(node);

			// メインウィンドウを読み込みます。
			load_main_window(node);

			// サブウィンドウを読み込みます。
			load_sub_window(node);

			// フローティングシャトルを読み込みます。
			load_float_shuttle(node);

			return TRUE;
		}

		//
		// 指定された要素からシャトル名を読み込みます。
		//
		void get_shuttle_name(const ptree& node, const std::string& name, std::wstring& value)
		{
			get_string(node, name, value);

			value = Shuttle::decorate_name(value);
		}

		//
		// ペインを読み込みます。
		//
		void load_pane(const ptree& node, const std::shared_ptr<Pane>& pane)
		{
			MY_TRACE_FUNC("");

			// ペインの属性を読み込みます。
			auto current = -1;

			get_label(node, "split_mode", pane->split_mode, Pane::c_split_mode.labels);
			get_label(node, "origin", pane->origin, Pane::c_origin.labels);
			get_label(node, "caption_mode", pane->caption_mode, Pane::c_caption_mode.labels);
			get_bool(node, "is_border_locked", pane->is_border_locked);
			get_int(node, "border", pane->border);
			get_int(node, "current", current);

			MY_TRACE_INT(pane->split_mode);
			MY_TRACE_INT(pane->origin);
			MY_TRACE_INT(pane->caption_mode);
			MY_TRACE_INT(pane->is_border_locked);
			MY_TRACE_INT(pane->border);
			MY_TRACE_INT(current);

			// ドッキングシャトルを読み込みます。
			if (auto dock_shuttle_nodes_op = node.get_child_optional("dock_shuttle"))
			{
				for (const auto& pair : dock_shuttle_nodes_op.value())
				{
					const auto& dock_node = pair.second;

					// シャトル名を読み込みます。
					std::wstring name;
					get_shuttle_name(dock_node, "name", name);
					MY_TRACE_STR(name);

					// シャトルを取得します。
					auto shuttle = shuttle_manager.get(name);
					if (!shuttle) continue;

					//  ペインにシャトルを追加します。
					pane->insert_shuttle(shuttle.get());
				}
			}

			// ペインのカレントシャトルを設定します。
			pane->set_current_index(current);

			// 子ペインを読み込みます。
			if (auto pane_nodes_op = node.get_child_optional("pane"))
			{
				size_t i = 0;
				for (const auto& pair : pane_nodes_op.value())
				{
					if (i >= 2) break; // 子ペインは最大2個までです。

					const auto& pane_node = pair.second;

					pane->children[i] = std::make_shared<Pane>(pane->owner);

					load_pane(pane_node, pane->children[i]);

					i++;
				}
			}
		}

		//
		// ルートペインを読み込みます。
		//
		void load_root_pane(const ptree& node, HWND hwnd)
		{
			MY_TRACE_FUNC("");

			auto root = SubWindow::get_root_pane(hwnd);

			// 一旦すべてのペインをリセットします。
			root->reset_pane();

			// ルートペインを読み込みます。
			if (auto root_pane_nodes_op = node.get_child_optional("root_pane"))
			{
				load_pane(root_pane_nodes_op.value(), root);
			}
		}

		//
		// 事前にサブウィンドウを読み込みます。
		// これにより、レイアウトを読み込む前に必要なすべてのサブウィンドウが存在する状態になります。
		//
		void pre_load_sub_window(const ptree& node)
		{
			MY_TRACE_FUNC("");

			// 一旦すべてのサブウィンドウを削除します。
			sub_window_manager.clear();

			// サブウィンドウを読み込みます。
			if (auto sub_window_nodes_op = node.get_child_optional("sub_window"))
			{
				for (const auto& pair : sub_window_nodes_op.value())
				{
					const auto& sub_window_node = pair.second;

					// シャトル名を読み込みます。
					std::wstring name;
					get_shuttle_name(sub_window_node, "name", name);
					MY_TRACE_STR(name);

					// サブウィンドウを作成します。
					auto sub_window = std::make_shared<SubWindow>();
					if (sub_window->create(name, hive.main_window))
						sub_window->init(name, *sub_window);
				}
			}
		}

		//
		// メインウィンドウを読み込みます。
		//
		void load_main_window(const ptree& node)
		{
			MY_TRACE_FUNC("");

			// メインウィンドウを読み込みます。
			if (auto main_window_node_op = node.get_child_optional("main_window"))
			{
				const auto& main_window_node = main_window_node_op.value();

				// ウィンドウ位置を読み込みます。
				get_window(main_window_node, "placement", hive.main_window);

				// ルートペインを読み込みます。
				load_root_pane(main_window_node, hive.main_window);
			}
		}

		//
		// サブウィンドウを読み込みます。
		//
		void load_sub_window(const ptree& node)
		{
			MY_TRACE_FUNC("");

			// サブウィンドウを読み込みます。
			if (auto sub_window_nodes_op = node.get_child_optional("sub_window"))
			{
				for (const auto& pair : sub_window_nodes_op.value())
				{
					const auto& sub_window_node = pair.second;

					// シャトル名を読み込みます。
					std::wstring name;
					get_shuttle_name(sub_window_node, "name", name);
					MY_TRACE_STR(name);

					// サブウィンドウを取得します。
					auto shuttle = shuttle_manager.get(name);
					if (!shuttle) continue;

					// ルートペインを読み込みます。
					load_root_pane(sub_window_node, *shuttle);
				}
			}
		}

		//
		// フローティングシャトルを読み込みます。
		//
		void load_float_shuttle(const ptree& node)
		{
			MY_TRACE_FUNC("");

			// フローティングシャトルを読み込みます。
			if (auto float_shuttle_nodes_op = node.get_child_optional("float_shuttle"))
			{
				for (const auto& pair : float_shuttle_nodes_op.value())
				{
					const auto& float_node = pair.second;

					// シャトル名を読み込みます。
					std::wstring name;
					get_shuttle_name(float_node, "name", name);
					MY_TRACE_STR(name);

					// シャトルを取得します。
					auto shuttle = shuttle_manager.get(name);
					if (!shuttle) continue;

					get_window(float_node, "placement", *shuttle->float_container);

					// フローティングコンテナが表示状態の場合は
					if (::IsWindowVisible(*shuttle->float_container))
					{
						// シャトルがドッキング中かもしれないので、ドッキングを解除します。
						shuttle->fire_release_shuttle();

						// ターゲットウィンドウを表示状態にします。
						shuttle->show_target_window();
					}
				}
			}
		}
	} preference_loader;
}
