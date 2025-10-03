#pragma once

namespace apn::workspace
{
	//
	// このクラスはファイルからプリファレンスを読み込みます。
	//
	struct PreferenceLoader : StdConfigIO
	{
		//
		// 入出力オプションです。
		//
		uint32_t flags;

		//
		// コンストラクタです。
		//
		explicit PreferenceLoader(uint32_t flags = hive.c_preference_flag.c_default)
			: flags(flags)
		{
		}

		//
		// ノードからプリファレンスを読み込みます。
		//
		virtual BOOL read_node(n_json& node) override
		{
			MY_TRACE_FUNC("");

			if (flags & hive.c_preference_flag.c_config)
			{
				read_tav(node, "tav", hive.tav);
				read_int(node, "tab_height", hive.tab_height);
				read_int(node, "tab_space", hive.tab_space);
				read_int(node, "caption_height", hive.caption_height);
				read_int(node, "border_width", hive.border_width);
				read_color(node, "fill_color", hive.fill_color);
				read_color(node, "border_color", hive.border_color);
				read_color(node, "hot_border_color", hive.hot_border_color);
				read_color(node, "active_caption_color", hive.active_caption_color);
				read_color(node, "active_caption_text_color", hive.active_caption_text_color);
				read_color(node, "inactive_caption_color", hive.inactive_caption_color);
				read_color(node, "inactive_caption_text_color", hive.inactive_caption_text_color);
				read_bool(node, "use_theme", hive.use_theme);
				read_bool(node, "scroll_force", hive.scroll_force);
				read_bool(node, "fullscreen_preview", hive.fullscreen_preview);
				read_bool(node, "show_tab_force", hive.show_tab_force);
				read_bool(node, "bypass_keyboard_message", hive.bypass_keyboard_message);
				read_bool(node, "ignore_ctrl_key_up", hive.ignore_ctrl_key_up);
				read_label(node, "layout_list_mode", hive.layout_list_mode, hive.c_layout_list_mode.labels);
				read_bool(node, "slimbar.flag_use", hive.slimbar.flag_use);
				read_string(node, "slimbar.title_format", hive.slimbar.title_format);

				// スリムバーの設定を適用します。
				main_window->apply_slimbar();
			}

			// 事前にサブウィンドウを読み込みます。
			pre_read_sub_window(node);

			// メインウィンドウを読み込みます。
			read_main_window(node);

			// サブウィンドウを読み込みます。
			read_sub_window(node);

			// フローティングシャトルを読み込みます。
			read_float_shuttle(node);

			// メインウィンドウが非表示の場合は
			if (!::IsWindowVisible(hive.main_window))
			{
				// メインウィンドウを表示します。
				::ShowWindow(hive.main_window, SW_SHOW);
			}

			return TRUE;
		}

		//
		// シャトル名を読み込みます。
		//
		void read_shuttle_name(const n_json& node, const std::string& name, std::wstring& value)
		{
			read_string(node, name, value);

			value = Shuttle::decorate_name(value);
		}

		//
		// タブを読み込みます。
		//
		BOOL read_tav(const n_json& node, const std::string& name, Hive::Tav& tav)
		{
			MY_TRACE_FUNC("");

			// タブノードを読み込みます。
			n_json tav_node;
			read_child_node(node, name, tav_node);
			if (tav_node.is_null()) return FALSE;

			// タブの属性を読み込みます。
			return read_tav(tav_node, tav);
		}

		//
		// タブを読み込みます。
		//
		BOOL read_tav(const n_json& tav_node, Hive::Tav& tav)
		{
			MY_TRACE_FUNC("");

			// タブの属性を読み込みます。
			read_label(tav_node, "display_mode", tav.display_mode, tav.c_display_mode.labels);
			read_label(tav_node, "select_mode", tav.select_mode, tav.c_select_mode.labels);
			read_label(tav_node, "stretch_mode", tav.stretch_mode, tav.c_stretch_mode.labels);
			read_label(tav_node, "location", tav.location, tav.c_location.labels);
			read_label(tav_node, "node_align", tav.node_align, tav.c_node_align.labels);

			return TRUE;
		}

		//
		// ドロワーを読み込みます。
		//
		BOOL read_drawer(const n_json& drawer_node, Drawer& drawer)
		{
			MY_TRACE_FUNC("");

			// ドロワーの属性を読み込みます。
			read_bool(drawer_node, "inside", drawer.inside);

			// タブの属性を読み込みます。
			return read_tav(drawer_node, drawer);
		}

		//
		// ペインを読み込みます。
		//
		void read_pane(const n_json& node, const std::shared_ptr<Pane>& pane)
		{
			MY_TRACE_FUNC("");

			// タブを読み込みます。
			read_tav(node, "tav", pane->tav);

			// ドロワーを読み込みます。
			{
				read_child_nodes(node, "drawer",
					[&](const n_json& drawer_node, size_t i)
				{
					// インデックスが無効の場合はループを終了します。
					if (i >= std::size(pane->drawers)) return FALSE;

					// ドロワーノードが無効の場合は何もしません。
					if (drawer_node.is_null()) return TRUE;

					// ドロワーを作成します。
					auto drawer = pane->create_drawer(i);

					// ドロワーの属性を読み込みます。
					read_drawer(drawer_node, *drawer);

					// タブ項目を読み込みます。
					read_child_nodes(drawer_node, "shuttle",
						[&](const n_json& shuttle_node, size_t i)
					{
						// シャトル名を読み込みます。
						std::wstring name;
						read_shuttle_name(shuttle_node, "name", name);
						MY_TRACE_STR(name);

						// シャトルを取得します。
						auto shuttle = shuttle_manager.get(name);
						if (!shuttle) return TRUE;

						// タブ項目を追加します。
						drawer->insert_node(*shuttle, name.c_str(), -1);

						return TRUE;
					});

					return TRUE;
				});

				n_json drawer_node;
				read_child_node(node, "drawer", drawer_node);

				for (const auto& drawer : pane->drawers)
				{
					// タブの属性を読み込みます。
					Hive::Tav tav;
					if (read_tav(drawer_node, my::format("{/}", &drawer - pane->drawers).c_str(), tav))
					{
						(Hive::Tav&)*drawer = tav;
					}
				}
			}

			// ペインの属性を読み込みます。
			auto current = -1;

			read_label(node, "split_mode", pane->split_mode, hive.pane.c_split_mode.labels);
			read_label(node, "origin", pane->origin, hive.pane.c_origin.labels);
			read_label(node, "caption_mode", pane->caption_mode, hive.pane.c_caption_mode.labels);
			read_label(node, "caption_location", pane->caption_location, hive.pane.c_caption_location.labels);
			read_bool(node, "is_border_locked", pane->is_border_locked);
			read_int(node, "border", pane->border);
			read_int(node, "current", current);

			MY_TRACE_INT(pane->split_mode);
			MY_TRACE_INT(pane->origin);
			MY_TRACE_INT(pane->caption_mode);
			MY_TRACE_INT(pane->caption_location);
			MY_TRACE_INT(pane->is_border_locked);
			MY_TRACE_INT(pane->border);
			MY_TRACE_INT(current);

			// ドッキングシャトルを読み込みます。
			read_child_nodes(node, "dock_shuttle",
				[&](const n_json& dock_shuttle_node, size_t i)
			{
				// シャトル名を読み込みます。
				std::wstring name;
				read_shuttle_name(dock_shuttle_node, "name", name);
				MY_TRACE_STR(name);

				// シャトルを取得します。
				auto shuttle = shuttle_manager.get(name);
				if (!shuttle) return TRUE;

				//  ペインにシャトルを追加します。
				pane->insert_shuttle(shuttle.get(), FALSE);

				return TRUE;
			});

			// ペインのカレントシャトルを設定します。
			pane->set_current_index(current);

			// 子ペインを読み込みます。
			read_child_nodes(node, "pane",
				[&](const n_json& pane_node, size_t i)
			{
				if (i >= 2) return FALSE; // 子ペインは最大2個までです。
				if (pane_node.is_null()) return FALSE;

				pane->children[i] = std::make_shared<Pane>(pane->owner);

				read_pane(pane_node, pane->children[i]);

				return TRUE;
			});
		}

		//
		// ルートペインを読み込みます。
		//
		void read_root_pane(const n_json& node, HWND hwnd)
		{
			MY_TRACE_FUNC("");

			// ルートペインを取得します。
			auto root = SubWindow::get_root_pane(hwnd);
			if (!root) return;

			// フルイドモードの場合は
			if (flags & hive.c_preference_flag.c_fluid)
			{
				// ルートペインが固体化されている場合は何もしません。
				if (root->is_solid) return;
			}

			// 一旦すべてのペインをリセットします。
			root->reset_pane();

			// ルートペインを読み込みます。
			n_json root_pane_node;
			read_child_node(node, "root_pane", root_pane_node);
			read_bool(root_pane_node, "is_solid", root->is_solid);
			read_pane(root_pane_node, root);
		}

		//
		// 事前にサブウィンドウを読み込みます。
		// これにより、レイアウトを読み込む前に必要なすべてのサブウィンドウが存在する状態になります。
		//
		void pre_read_sub_window(const n_json& node)
		{
			MY_TRACE_FUNC("");

			if (flags & hive.c_preference_flag.c_fluid)
			{
				// 一旦流動的なサブウィンドウを削除します。
				sub_window_manager.clear_fluids();
			}
			else
			{
				// 一旦すべてのサブウィンドウを削除します。
				sub_window_manager.clear();
			}

			// サブウィンドウを読み込みます。
			read_child_nodes(node, "sub_window",
				[&](const n_json& sub_window_node, size_t i)
			{
				// シャトル名を読み込みます。
				std::wstring name;
				read_shuttle_name(sub_window_node, "name", name);
				MY_TRACE_STR(name);

				// 既存のサブウィンドウが存在する場合は
				if (auto shuttle = shuttle_manager.get(name))
				{
					// 何もしません。
				}
				// 既存のサブウィンドウが存在しない場合は
				else
				{
					// サブウィンドウを作成します。
					auto sub_window = std::make_shared<SubWindow>();
					if (sub_window->create(name, hive.main_window))
						sub_window->init(name, *sub_window);
				}

				return TRUE;
			});
		}

		//
		// メインウィンドウを読み込みます。
		//
		void read_main_window(const n_json& node)
		{
			MY_TRACE_FUNC("");

			// メインウィンドウを読み込みます。
			n_json main_window_node;
			read_child_node(node, "main_window", main_window_node);

			// ウィンドウ位置を読み込みます。
			read_window_pos(main_window_node, "placement", hive.main_window);

			// ルートペインを読み込みます。
			read_root_pane(main_window_node, hive.main_window);
		}

		//
		// サブウィンドウを読み込みます。
		//
		void read_sub_window(const n_json& node)
		{
			MY_TRACE_FUNC("");

			// サブウィンドウを読み込みます。
			read_child_nodes(node, "sub_window",
				[&](const n_json& sub_window_node, size_t i)
			{
				// シャトル名を読み込みます。
				std::wstring name;
				read_shuttle_name(sub_window_node, "name", name);
				MY_TRACE_STR(name);

				// サブウィンドウを取得します。
				auto shuttle = shuttle_manager.get(name);
				if (!shuttle) return TRUE;

				// ルートペインを読み込みます。
				read_root_pane(sub_window_node, *shuttle);

				return TRUE;
			});
		}

		//
		// フローティングシャトルを読み込みます。
		//
		void read_float_shuttle(const n_json& node)
		{
			MY_TRACE_FUNC("");

			// フローティングシャトルを読み込みます。
			read_child_nodes(node, "float_shuttle",
				[&](const n_json& float_shuttle_node, size_t i)
			{
				// シャトル名を読み込みます。
				std::wstring name;
				read_shuttle_name(float_shuttle_node, "name", name);
				MY_TRACE_STR(name);

				// シャトルを取得します。
				auto shuttle = shuttle_manager.get(name);
				if (!shuttle) return TRUE;

				// ウィンドウ位置を読み込みます。
				auto flags = SWP_NOSIZE;
				if (my::get_style(*shuttle->float_container) & WS_THICKFRAME) flags = 0;
				read_window_pos(float_shuttle_node, "placement", *shuttle->float_container, flags);

				// フローティングコンテナが表示状態の場合は
				if (::IsWindowVisible(*shuttle->float_container))
				{
					// シャトルがドッキング中かもしれないので、ドッキングを解除します。
					shuttle->fire_release_shuttle();

					// ターゲットウィンドウを表示状態にします。
					shuttle->show_target_window();
				}

				return TRUE;
			});
		}
	};
}
