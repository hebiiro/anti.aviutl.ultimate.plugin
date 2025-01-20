#pragma once

namespace apn::workspace
{
	//
	// このクラスはシャトルをドッキングさせることができるウィンドウです。
	// 再帰分割可能なペインを保持し、このペインにシャトルをドッキングさせることができます。
	// 1つのペインに複数のシャトルをドッキングさせることもできます。(タブ化)
	//
	template <typename T>
	struct DockSite : T
	{
		inline static constexpr struct CommandID {
			inline static constexpr struct SplitMode {
				inline static constexpr uint32_t c_none = 1000;
				inline static constexpr uint32_t c_vert = 1001;
				inline static constexpr uint32_t c_horz = 1002;
			} c_split_mode;
			inline static constexpr struct Origin {
				inline static constexpr uint32_t c_top_left = 1010;
				inline static constexpr uint32_t c_bottom_right = 1011;
			} c_origin;
			inline static constexpr struct CaptionMode {
				inline static constexpr uint32_t c_hide = 1020;
				inline static constexpr uint32_t c_show = 1021;
			} c_caption_mode;
			inline static constexpr uint32_t c_is_border_locked = 1100;
			inline static constexpr uint32_t c_rename_sub_window = 1101;
			inline static constexpr uint32_t c_move_to_front = 1102;
			inline static constexpr uint32_t c_move_to_back = 1103;
			inline static constexpr uint32_t c_move_to_left = 1104;
			inline static constexpr uint32_t c_move_to_right = 1105;
			inline static constexpr uint32_t c_undock = 1106;
			inline static constexpr uint32_t c_is_solid = 1107;
			inline static constexpr uint32_t c_pane_config = 1108;
			inline static constexpr struct DrawerConfig {
				inline static constexpr uint32_t c_begin = 1110;
				inline static constexpr uint32_t c_top = 1110;
				inline static constexpr uint32_t c_bottom = 1111;
				inline static constexpr uint32_t c_left = 1112;
				inline static constexpr uint32_t c_right = 1113;
				inline static constexpr uint32_t c_end = 1113;
			} c_drawer_config;
			inline static constexpr struct Shuttle {
				inline static constexpr uint32_t c_begin = 2000;
				inline static constexpr uint32_t c_end = 3000;
			} c_shuttle;
		} c_command_id;

		//
		// ルートペインを取得するためのプロパティ名です。
		//
		inline static constexpr auto c_prop_name = _T("apn::dock_site::root_pane");

		//
		// ホットボーダーを持つペインです。
		//
		inline static std::shared_ptr<Pane> hot_border_pane;

		//
		// WM_MOUSEMOVEのLPARAMです。
		//
		LPARAM mouse_move_pos = -1;

		//
		// ルートペインを作成し、ウィンドウに関連付けます。
		//
		inline static void init_root_pane(HWND hwnd)
		{
			auto p = new std::shared_ptr<RootPane>(std::make_shared<RootPane>(hwnd));

			::SetProp(hwnd, c_prop_name, p);
		}

		//
		// ルートペインを削除し、ウィンドウとの関連付けを解除します。
		//
		inline static void exit_root_pane(HWND hwnd)
		{
			// ルートペインを取得します。
			auto p = (std::shared_ptr<RootPane>*)::GetProp(hwnd, c_prop_name);

			// ルートペインをリセットします。
			// これにより、ドッキング中のすべてのシャトルがフローティング状態になり、
			// hwndが削除されたとしても、他のウィンドウが削除されずに済むようになります。
			(*p)->reset_pane();

			// ルートペインを削除します。
			delete p;

			// ウィンドウとの関連付けを解除します。
			::RemoveProp(hwnd, c_prop_name);
		}

		//
		// ルートペインを返します。
		//
		inline static std::shared_ptr<RootPane> get_root_pane(HWND hwnd)
		{
			auto p = (std::shared_ptr<RootPane>*)::GetProp(hwnd, c_prop_name);
			if (!p) return nullptr; // 取得できなかった場合はnullptrを返します。
			return *p;
		}
		//
		// ルートペインを返します。
		//
		inline static std::shared_ptr<RootPane> get_root_pane(const std::shared_ptr<Shuttle>& shuttle)
		{
			if (shuttle)
				return get_root_pane(*shuttle);
			else
				return nullptr;
		}

		//
		// 指定されたウィンドウがドッキングサイトの場合はTRUEを返します。
		//
		inline static BOOL is_dock_site(HWND hwnd)
		{
			return !!::GetProp(hwnd, c_prop_name);
		}

		//
		// トップレベルのペインを返します。
		// ルートペインではなく、最大化されたペインの場合もあります。
		//
		inline static std::shared_ptr<Pane> get_top_level_pane(HWND dock_site)
		{
			auto root = get_root_pane(dock_site);
			if (!root) return nullptr; // 取得できなかった場合はnullptrを返します。
			auto maximized_pane = root->get_maximized_pane();
			if (maximized_pane) return maximized_pane; // 最大化されたペインがあれば、それを返します。
			return root;
		}

		//
		// 指定されたウィンドウがドッキングサイトにドッキング可能な場合はTRUEを返します。
		//
		inline static BOOL can_docking(HWND dock_site, HWND hwnd)
		{
			// hwndがドッキングサイトの場合は
			if (is_dock_site(hwnd))
			{
				// hwndが祖先かどうかチェックします。
				if (my::is_ancestor(dock_site, hwnd))
				{
					// hwndはdock_siteの祖先なのでドッキングできません。
					return FALSE;
				}
			}

			return TRUE;
		}

		//
		// 指定されたメニューのコピーを作成して返します。
		//
		inline static HMENU copy_menu(HMENU src_menu)
		{
			auto dst_menu = ::CreatePopupMenu();

			auto c = ::GetMenuItemCount(src_menu);
			for (decltype(c) i = 0; i < c; i++)
			{
				std::vector<TCHAR> text(MAX_PATH, _T('\0'));
				MENUITEMINFO mii = { sizeof(mii) };
				mii.fMask = MIIM_BITMAP | MIIM_CHECKMARKS | MIIM_DATA | MIIM_FTYPE | MIIM_ID | MIIM_STATE | MIIM_STRING | MIIM_SUBMENU;
				mii.dwTypeData = text.data();
				mii.cch = text.size();
				::GetMenuItemInfo(src_menu, i, TRUE, &mii);

				if (mii.hSubMenu)
					mii.hSubMenu = copy_menu(mii.hSubMenu);

				::InsertMenuItem(dst_menu, i, TRUE, &mii);
			}

			return dst_menu;
		}

		//
		// 指定された座標にカレントシャトルのメニューをポップアップ表示します。
		//
		inline static BOOL show_shuttle_menu(const std::shared_ptr<Pane>& pane, POINT cursor_pos)
		{
			// カレントシャトルを取得します。
			if (auto shuttle = pane->get_current_shuttle())
			{
				// シャトルのメニューを取得します。
				if (auto orig_menu = ::GetMenu(*shuttle))
				{
					// メニューのコピーを作成します。(ポップアップメニュー化します)
					my::menu::unique_ptr<> menu(copy_menu(orig_menu));

					// ポップアップメニューを表示します。
					::TrackPopupMenuEx(menu.get(), 0,
						cursor_pos.x, cursor_pos.y, *shuttle, nullptr);

					return TRUE;
				}
			}

			return FALSE;
		}

		//
		// 指定されたメニューにシャトル一覧メニューを構築して追加します。
		//
		inline static void add_shuttle_list(HMENU menu, uint32_t begin_id, auto func)
		{
			// カテゴリ毎のサブメニューのコレクションです。
			std::unordered_map<my::tstring, HMENU> sub_menus;

			// デフォルトのカテゴリを構築します。
			for (const auto& category : Shuttle::c_default_category)
			{
				// カテゴリ毎のサブメニューを作成します。
				auto sub_menu = ::CreatePopupMenu();

				// カテゴリ毎のサブメニューをメニューに追加します。
				::AppendMenuW(menu, MF_POPUP, (UINT_PTR)sub_menu, category.c_str());

				// カテゴリ毎のサブメニューをコレクションに追加します。
				sub_menus[category] = sub_menu;
			}

			// 最初のコマンドIDを取得します。
			auto id = begin_id;

			// すべてのシャトルを走査します。
			for (auto& pair : shuttle_manager.collection)
			{
				// シャトルを取得します。
				auto& shuttle = pair.second;

				// カテゴリ名を取得します。
				auto category_name = (LPCWSTR)::GetProp(*shuttle, L"aviutl.plugin.category_name");

				// シャトルのカテゴリ名を取得できた場合は
				if (category_name)
				{
					// カテゴリ名が空文字列のシャトルはメニューから除外します。
					if (!*category_name) continue;
				}
				// シャトルのカテゴリ名を取得できなかった場合は
				else
				{
					// デフォルトのカテゴリに割り当てます。
					category_name = L"セカンダリ";
				}

				// カテゴリ毎のサブメニューを取得します。
				auto& sub_menu = sub_menus[category_name];

				// カテゴリ毎のサブメニューがまだ作成されていない場合は
				if (!sub_menu)
				{
					// カテゴリ毎のサブメニューを作成します。
					sub_menu = ::CreatePopupMenu();

					// カテゴリ毎のサブメニューをメニューに追加します。
					::AppendMenuW(menu, MF_POPUP, (UINT_PTR)sub_menu, category_name);

					// カテゴリ毎のサブメニューをコレクションに追加します。
					sub_menus[category_name] = sub_menu;
				}

				// シャトルをカテゴリ毎のサブメニューに追加します。
				::AppendMenuW(sub_menu, MF_STRING, id, shuttle->name.c_str());

				// 追加されたメニュー項目を操作します。
				func(sub_menu, id, shuttle);

				// IDを次に進めます。
				id++;
			}
		}

		//
		// ペイン操作用のメニューを表示します。
		//
		inline static void show_pane_menu(HWND dock_site, const std::shared_ptr<Pane>& pane)
		{
			// ルートペインを取得します。
			auto root = get_root_pane(dock_site);
			if (!root) return;

			// マウスカーソル座標を取得します。
			auto cursor_pos = my::get_cursor_pos();

			// マウスカーソル座標をクライアント座標に変換します。
			auto point = cursor_pos;
			my::map_window_points(nullptr, dock_site, &point);

			// タブ項目の数を取得します。
			auto c = pane->get_tab_count();

			// マウスカーソル位置にあるタブ項目を取得します。
			auto ht = pane->hittest(point);

			// タブのアイコンがクリックされた場合は
			if (ht == pane->tav.c_icon_index)
			{
				// カレントシャトルのメニューをポップアップ表示します。
				show_shuttle_menu(pane, cursor_pos);

				return;
			}

			// シャトルを取得します。取得できない場合もあります。
			auto shuttle = pane->get_shuttle(ht);

			// ターゲットがサブウィンドウか判定します。
			auto is_sub_window = shuttle && get_root_pane(*shuttle);

			// メニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// メニューにシャトルの一覧を追加します。
			add_shuttle_list(menu.get(), c_command_id.c_shuttle.c_begin,
				[&](HMENU sub_menu, UINT_PTR id, auto shuttle)
			{
				// シャトルのタブが存在するなら
				if (pane->find_tab(shuttle.get()) != -1)
				{
					// メニューアイテムにチェックを入れます。
					::CheckMenuItem(sub_menu, id, MF_CHECKED);
				}

				// ドッキング不可能なら
				if (!can_docking(dock_site, *shuttle))
				{
					// メニューアイテムを無効にします。
					::EnableMenuItem(sub_menu, id, MF_DISABLED | MF_GRAYED);
				}
			});

			auto append_separator = [&]() {
				::AppendMenu(menu.get(), MF_SEPARATOR, -1, nullptr);
			};

			auto append_menu = [&](uint32_t id, LPCTSTR text) {
				::AppendMenu(menu.get(), MF_STRING, id, text);
			};

			auto check_menu = [&](uint32_t id) {
				::CheckMenuItem(menu.get(), id, MF_CHECKED);
			};

			auto disable_menu = [&](uint32_t id) {
				::EnableMenuItem(menu.get(), id, MF_GRAYED | MF_DISABLED);
			};

			auto append_disable_menu = [&](BOOL disable, uint32_t id, LPCTSTR text) {
				append_menu(id, text); if (disable) disable_menu(id);
			};

			auto append_check_menu = [&](BOOL check, uint32_t id, LPCTSTR text) {
				append_menu(id, text); if (check) check_menu(id);
			};

			auto is_front = ht <= 0 || ht > c - 1;
			auto is_back = ht < 0 || ht >= c - 1;

			append_separator();
			append_menu(c_command_id.c_split_mode.c_none, _T("分割なし"));
			append_menu(c_command_id.c_split_mode.c_vert, _T("左右に分割"));
			append_menu(c_command_id.c_split_mode.c_horz, _T("上下に分割"));
			switch (pane->split_mode) {
			case hive.pane.c_split_mode.c_none: check_menu(c_command_id.c_split_mode.c_none); break;
			case hive.pane.c_split_mode.c_vert: check_menu(c_command_id.c_split_mode.c_vert); break;
			case hive.pane.c_split_mode.c_horz: check_menu(c_command_id.c_split_mode.c_horz); break;
			}
			append_separator();
			append_menu(c_command_id.c_origin.c_top_left, _T("左上を原点にする"));
			append_menu(c_command_id.c_origin.c_bottom_right, _T("右下を原点にする"));
			switch (pane->origin) {
			case hive.pane.c_origin.c_top_left: check_menu(c_command_id.c_origin.c_top_left); break;
			case hive.pane.c_origin.c_bottom_right: check_menu(c_command_id.c_origin.c_bottom_right); break;
			}
			append_separator();
			append_disable_menu(is_front, c_command_id.c_move_to_front, _T("先頭に移動する"));
			append_disable_menu(is_back, c_command_id.c_move_to_back, _T("末尾に移動する"));
			append_disable_menu(is_front, c_command_id.c_move_to_left, _T("左に移動する"));
			append_disable_menu(is_back, c_command_id.c_move_to_right, _T("右に移動する"));
			append_check_menu(pane->is_border_locked, c_command_id.c_is_border_locked, _T("ボーダーをロックする"));
			// ペインのオーナーがサブウィンドウではない場合はこのメニューアイテムを無効化します。
			append_disable_menu(!is_sub_window, c_command_id.c_rename_sub_window, _T("名前を変更"));
			append_check_menu(root->is_solid, c_command_id.c_is_solid, _T("レイアウトを固定化"));
			append_menu(c_command_id.c_pane_config, _T("ペインとタブの設定"));
			// ドロワーの設定メニューを作成します。
			my::menu::unique_ptr<> drawer_config_menu(::CreatePopupMenu());
			::AppendMenu(menu.get(), MF_POPUP, (UINT_PTR)drawer_config_menu.get(), _T("ドロワーの設定"));
			::AppendMenu(drawer_config_menu.get(), MF_STRING, c_command_id.c_drawer_config.c_top, _T("上側"));
			::AppendMenu(drawer_config_menu.get(), MF_STRING, c_command_id.c_drawer_config.c_bottom, _T("下側"));
			::AppendMenu(drawer_config_menu.get(), MF_STRING, c_command_id.c_drawer_config.c_left, _T("左側"));
			::AppendMenu(drawer_config_menu.get(), MF_STRING, c_command_id.c_drawer_config.c_right, _T("右側"));
			append_separator();
			// ドッキングしているシャトルが存在しない場合はこのメニューアイテムを無効化します。
			append_disable_menu(c == 0, c_command_id.c_undock, _T("ドッキングを解除"));

			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD,
				cursor_pos.x, cursor_pos.y, dock_site, nullptr);
			if (!id) return;

			switch (id)
			{
			case c_command_id.c_split_mode.c_none: pane->set_split_mode(hive.pane.c_split_mode.c_none); break;
			case c_command_id.c_split_mode.c_vert: pane->set_split_mode(hive.pane.c_split_mode.c_vert); break;
			case c_command_id.c_split_mode.c_horz: pane->set_split_mode(hive.pane.c_split_mode.c_horz); break;

			case c_command_id.c_origin.c_top_left: pane->set_origin(hive.pane.c_origin.c_top_left); break;
			case c_command_id.c_origin.c_bottom_right: pane->set_origin(hive.pane.c_origin.c_bottom_right); break;

			case c_command_id.c_move_to_front: pane->move_tab(ht, 0); break;
			case c_command_id.c_move_to_back: pane->move_tab(ht, c - 1); break;
			case c_command_id.c_move_to_left: pane->move_tab(ht, ht - 1); break;
			case c_command_id.c_move_to_right: pane->move_tab(ht, ht + 1); break;

			case c_command_id.c_is_border_locked: pane->is_border_locked = !pane->is_border_locked; break;

			case c_command_id.c_rename_sub_window: shuttle_manager.show_rename_dialog(shuttle, dock_site); break;

			case c_command_id.c_is_solid: root->is_solid = !root->is_solid; break;

			case c_command_id.c_undock:
				{
					// ドッキングを解除します。

					// カーソルの位置にあるタブアイテムのインデックスです。
					auto index = ht;

					// インデックスが無効の場合はカレントインデックスを使用します。
					if (index < 0)
						index = pane->get_current_index();

					// インデックスが有効の場合は
					if (index >= 0)
					{
						// ドッキングしているシャトルが存在する場合は
						if (auto shuttle = pane->get_shuttle(index))
							pane->release_shuttle(shuttle); // ドッキングを解除します。
					}

					break;
				}
			case c_command_id.c_pane_config:
				{
					PaneConfigDialog dialog(pane, dock_site);

					dialog.do_modal();

					break;
				}
			}

			if (id >= c_command_id.c_drawer_config.c_begin && id <= c_command_id.c_drawer_config.c_end)
			{
				DrawerConfigDialog dialog(pane, id - c_command_id.c_drawer_config.c_begin, dock_site);

				dialog.do_modal();
			}
			else if (id >= c_command_id.c_shuttle.c_begin && id <= c_command_id.c_shuttle.c_end)
			{
				// ユーザーが指定したシャトルをクリックされたペインにドッキングさせます。

				// メニューアイテムのテキストを取得します。
				auto text = my::get_menu_item_text(menu.get(), id, MF_BYCOMMAND);
				MY_TRACE_STR(text);

				// テキストからシャトルを取得します。ドッキングできるかもチェックします。
				auto shuttle = shuttle_manager.get(text);
				if (shuttle && can_docking(dock_site, *shuttle))
				{
					// シャトルをペインにドッキングさせます。
					auto index = pane->insert_shuttle(shuttle.get(), FALSE, ht);
					if (index != -1) // ドッキングに成功した場合は-1以外になります。
					{
						// ドッキングしたシャトルをカレントにしてペインを更新します。
						pane->set_current_index(index);
					}
				}
			}

			// ペインの状態が変更されたのでレイアウトを更新します。
			pane->update_origin();
		}

		//
		// ドロワー操作用のメニューを表示します。
		//
		inline static void show_drawer_menu(HWND dock_site, const std::shared_ptr<Pane>& pane, const std::shared_ptr<Drawer>& drawer)
		{
			// ドロワーのインデックスを取得します。
			auto drawer_index = pane->get_drawer_index(*drawer);
			if (drawer_index == pane->c_drawer_index.c_none) return;

			// タブ項目の数を取得します。
			auto c = drawer->get_node_count();

			// マウスカーソル座標を取得します。
			auto cursor_pos = my::get_cursor_pos();

			// マウスカーソル座標をクライアント座標に変換します。
			auto point = cursor_pos;
			my::map_window_points(nullptr, *drawer, &point);

			// マウスカーソル位置にあるタブ項目を取得します。
			auto ht = drawer->hittest(point);

			// メニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// メニューにシャトルの一覧を追加します。
			add_shuttle_list(menu.get(), c_command_id.c_shuttle.c_begin,
				[&](HMENU sub_menu, UINT_PTR id, auto shuttle)
			{
				// シャトルのタブ項目が存在するなら
				if (drawer->find_node(*shuttle) != -1)
				{
					// メニューアイテムにチェックを入れます。
					::CheckMenuItem(sub_menu, id, MF_CHECKED);
				}

				// シャトルがドッキングしている場合は
				if (shuttle->is_docking())
				{
					// メニューアイテムを無効にします。
					::EnableMenuItem(sub_menu, id, MF_DISABLED | MF_GRAYED);
				}
			});

			auto append_separator = [&]() {
				::AppendMenu(menu.get(), MF_SEPARATOR, -1, nullptr);
			};

			auto append_menu = [&](uint32_t id, LPCTSTR text) {
				::AppendMenu(menu.get(), MF_STRING, id, text);
			};

			auto check_menu = [&](uint32_t id) {
				::CheckMenuItem(menu.get(), id, MF_CHECKED);
			};

			auto disable_menu = [&](uint32_t id) {
				::EnableMenuItem(menu.get(), id, MF_GRAYED | MF_DISABLED);
			};

			auto append_disable_menu = [&](BOOL disable, uint32_t id, LPCTSTR text) {
				append_menu(id, text); if (disable) disable_menu(id);
			};

			auto append_check_menu = [&](BOOL check, uint32_t id, LPCTSTR text) {
				append_menu(id, text); if (check) check_menu(id);
			};

			auto is_front = ht <= 0 || ht > c - 1;
			auto is_back = ht < 0 || ht >= c - 1;

			append_separator();
			append_disable_menu(is_front, c_command_id.c_move_to_front, _T("先頭に移動"));
			append_disable_menu(is_back, c_command_id.c_move_to_back, _T("末尾に移動"));
			append_disable_menu(is_front, c_command_id.c_move_to_left, _T("左に移動"));
			append_disable_menu(is_back, c_command_id.c_move_to_right, _T("右に移動"));
			append_disable_menu(ht < 0, c_command_id.c_undock, _T("削除"));
			append_separator();
			append_menu(c_command_id.c_drawer_config.c_begin + drawer_index, _T("ドロワーの設定"));

			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD,
				cursor_pos.x, cursor_pos.y, dock_site, nullptr);
			if (!id) return;

			switch (id)
			{
			case c_command_id.c_move_to_front: drawer->move_node(ht, 0); break;
			case c_command_id.c_move_to_back: drawer->move_node(ht, c - 1); break;
			case c_command_id.c_move_to_left: drawer->move_node(ht, ht - 1); break;
			case c_command_id.c_move_to_right: drawer->move_node(ht, ht + 1); break;
			case c_command_id.c_undock:
				{
					// カーソルの位置にあるタブ項目のインデックスを取得します。
					auto index = ht;

					// インデックスが有効の場合は
					if (index >= 0)
					{
						// タブ項目を削除します。
						drawer->erase_node(index);
					}

					break;
				}
			}

			if (id >= c_command_id.c_drawer_config.c_begin && id <= c_command_id.c_drawer_config.c_end)
			{
				DrawerConfigDialog dialog(pane, id - c_command_id.c_drawer_config.c_begin, dock_site);

				dialog.do_modal();
			}
			else if (id >= c_command_id.c_shuttle.c_begin && id <= c_command_id.c_shuttle.c_end)
			{
				// メニューアイテムのテキストを取得します。
				auto text = my::get_menu_item_text(menu.get(), id, MF_BYCOMMAND);
				MY_TRACE_STR(text);

				// テキストからシャトルを取得します。
				if (auto shuttle = shuttle_manager.get(text))
				{
					// シャトルをドロワーに追加します。
					auto index = pane->insert_shuttle(drawer, shuttle.get());
				}
			}

			my::invalidate(*drawer);
		}

		//
		// ルートペインをリセットします。
		//
		void reset_pane()
		{
			if (auto pane = get_root_pane(*this))
				pane->reset_pane();
		}

		//
		// ワークエリア領域を返します。
		//
		virtual RECT get_workarea()
		{
			return my::get_client_rect(*this);
		}

		//
		// 指定されたワークエリア領域を使用して
		// すべてのペインのレイアウトを更新します。
		//
		virtual void update_dock_site(const RECT& workarea)
		{
			if (auto pane = get_top_level_pane(*this))
				pane->update_origin(&workarea, pane->c_update_flag.c_default);
		}

		//
		// すべてのペインのレイアウトを更新します。
		//
		void update_dock_site()
		{
			update_dock_site(get_workarea());
		}

		//
		// コントロールからドロワーを取得して返します。
		//
		inline static std::shared_ptr<Drawer> get_drawer(Pane* pane, HWND control)
		{
			// ドロワーインデックスが取得できた場合は
			auto drawer_index = pane->get_drawer_index(control);
			if (drawer_index != pane->c_drawer_index.c_none)
			{
				// ドロワーを返します。
				return pane->drawers[drawer_index];
			}

			// それ以外の場合はnullptrを返します。
			return nullptr;
		}

		//
		// ドロワーの選択項目からシャトルを取得して返します。
		//
		inline static Shuttle* get_shuttle(const std::shared_ptr<Drawer>& drawer)
		{
			// ドロワーの選択項目が有効の場合は
			if (drawer->selected_node_index >= 0)
			{
				// ドロワーの選択項目からシャトルを取得して返します。
				return Shuttle::get_pointer(drawer->nodes[drawer->selected_node_index]->hwnd);
			}

			// それ以外の場合はnullptrを返します。
			return nullptr;
		}

		//
		// ドロワーの選択項目が切り替わったときの処理です。
		//
		BOOL on_drawer_sel_change(const std::shared_ptr<Drawer>& drawer)
		{
			// シャトルを取得します。
			auto shuttle = get_shuttle(drawer);
			if (!shuttle) return FALSE;

			// シャトルがドッキング状態の場合は何もしません。
			if (shuttle->is_docking()) return FALSE;

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_NOTIFY: // ペインのタブから通知されます。
				{
					auto header = (NMHDR*)lParam;

					MY_TRACE_FUNC("WM_NOTIFY, {}", header->code);

					switch (header->code)
					{
					case NM_DBLCLK:
						{
							// タブからペインを取得します。
							auto pane = Pane::get_pane(header->hwndFrom);
							if (!pane) break;

							// マウスカーソルの座標を取得します。
							auto point = my::get_cursor_pos();

							// マウスカーソルの座標をクライアント座標に変換します。
							my::map_window_points(nullptr, hwnd, &point);

							// マウスカーソルの座標にあるタブ項目を取得します。
							auto ht = pane->hittest(point);

							// タブ項目が無効の場合は何もしません。
							if (ht < 0) break;

							// ルートペインを取得します。
							auto root = get_root_pane(hwnd);

							// 最大化されたペインが存在する場合は
							if (root->get_maximized_pane().get() == pane)
							{
								// 最大化を解除します。
								root->set_maximized_pane(nullptr);
							}
							else
							{
								// 最大化されたペインにセットします。
								root->set_maximized_pane(pane->shared_from_this());
							}

							// レイアウトを更新します。
							update_dock_site();

							break;
						}
					case NM_RCLICK:
						{
							// コントロールに関連付けられているペインを取得します。
							auto pane = Pane::get_pane(header->hwndFrom);
							if (!pane) break;

							// コントロールがタブコントロールの場合は
							if (pane->tav == header->hwndFrom)
							{
								// タブコントロール上で右クリックが発生した場合は
								// ペインメニューを表示します。
								show_pane_menu(hwnd, pane->shared_from_this());
							}
							// コントロールがドロワーコントロールの場合は
							else if (auto drawer = get_drawer(pane, header->hwndFrom))
							{
								// ドロワーメニューを表示します。
								show_drawer_menu(hwnd, pane->shared_from_this(), drawer);
							}

							break;
						}
					case TCN_SELCHANGE:
						{
							// コントロールに関連付けられているペインを取得します。
							auto pane = Pane::get_pane(header->hwndFrom);
							if (!pane) break;

							// コントロールがタブコントロールの場合は
							if (pane->tav == header->hwndFrom)
							{
								// カレントシャトルが切り替わったので、ペインの表示状態を更新します。
								pane->update_origin();
							}
							// コントロールがドロワーコントロールの場合は
							else if (auto drawer = get_drawer(pane, header->hwndFrom))
							{
								// ドロワーの選択項目が切り替わったので、専用ハンドラで処理します。
								drawer_manager.on_drawer_sel_change(drawer);
							}

							break;
						}
					}

					break;
				}
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					// IMEは必要ないので無効化します。
					::ImmAssociateContextEx(hwnd, nullptr, IACE_IGNORENOCONTEXT);

					// ルートペインを作成します。
					init_root_pane(hwnd);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// ルートペインを削除します。
					exit_root_pane(hwnd);

					break;
				}
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {:#010x}, {:#010x}", wParam, lParam);

					// ドッキングサイトのウィンドウサイズが変更されたので
					// すべてのペインのレイアウトを再計算します。
					update_dock_site();

					break;
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE_FUNC("WM_SHOWWINDOW, {:#010x}, {:#010x}", wParam, lParam);

					if (wParam)
					{
						// ルートペインを取得できた場合は
						if (auto root = get_top_level_pane(hwnd))
						{
							// ルートペインのレイアウトを更新します。
							root->update_origin();
						}
					}

					break;
				}
			case WM_PAINT:
				{
					// メモリDCを作成するためのクライアント領域を取得します。
					auto rc = my::get_client_rect(hwnd);

					// 描画の準備をします。
					my::PaintDC pdc(hwnd);
#if 1
					my::DoubleBufferDC dc(pdc, &rc);
#else
					my::UxDC dc(pdc, &rc);
					if (!dc.is_valid()) return 0;
#endif
					// ルートペインを取得します。
					auto root = get_top_level_pane(hwnd);
					if (!root) return 0;

					// 描画に適したワークエリア領域を取得します。
					rc = get_workarea();

					// 背景を塗りつぶします。
					painter.fill_background(dc, &rc);

					// 各ペインのボーダーを再帰的に描画します。
					root->draw_border(dc);

					{
						// ホットボーダーの矩形を取得できる場合は
						RECT rc_hot_border;
						if (hot_border_pane &&
							hot_border_pane->owner == hwnd &&
							hot_border_pane->get_border_rect(&rc_hot_border))
						{
							// ホットボーダーを描画します。
							painter.fill_background(dc, &rc_hot_border, hive.hot_border_color, WP_SEPARATOR, CS_ACTIVE);
						}
					}

					{
						LOGFONTW lf = {};
						::GetThemeSysFont(hive.theme.get(), TMT_CAPTIONFONT, &lf);
						my::gdi::unique_ptr<HFONT> font(::CreateFontIndirectW(&lf));
						my::gdi::selector font_selector(dc, font.get());

						// 各ペインのキャプションを再帰的に描画します。
						root->draw_caption(dc);
					}

					return 0;
				}
			case WM_SETCURSOR:
				{
					// マウスカーソルがボーダーの上にある場合はマウスカーソルを変更します。

					if (hwnd != (HWND)wParam) break;

					// ルートペインを取得します。
					auto root = get_top_level_pane(hwnd);
					if (!root) break;

					// マウスカーソルの座標を取得します。
					auto point = my::get_cursor_pos();

					// スクリーン座標からクライアント座標に変換します。
					my::map_window_points(nullptr, hwnd, &point);

					// マウスカーソルの座標にあるボーダーを取得します。
					auto border_pane = root->hittest_border(point);
					if (!border_pane) break;

					// マウスカーソルの形状を変更します。
					switch (border_pane->split_mode)
					{
					case hive.pane.c_split_mode.c_vert:
						{
							::SetCursor(::LoadCursor(nullptr, IDC_SIZEWE));

							return TRUE;
						}
					case hive.pane.c_split_mode.c_horz:
						{
							::SetCursor(::LoadCursor(nullptr, IDC_SIZENS));

							return TRUE;
						}
					}

					break;
				}
			case WM_LBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_LBUTTONDOWN, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// ルートペインを取得します。
					auto root = get_top_level_pane(hwnd);
					if (!root) break;

					// 指定された座標からペインを取得します。
					auto pane = root->hittest_pane(point);
					if (!pane) break;

					// ペインのファセットを取得します。
					if (auto caption_facet = pane->get_caption_facet())
					{
						// キャプション領域を取得します。
						auto caption_rc = caption_facet->get_caption_rect(pane.get());

						// メニュー領域を取得します。
						auto menu_rc = caption_facet->get_menu_rect(pane.get(), &caption_rc);

						// メニュー領域で左クリックした場合は
						if (::PtInRect(&menu_rc, point))
						{
							// マウスカーソルの座標をスクリーン座標に変換します。
							auto cursor_pos = point;
							my::map_window_points(hwnd, nullptr, &cursor_pos);

							// シャトルのメニューを表示します。
							if (show_shuttle_menu(pane, cursor_pos))
								break;
						}
					}

					// マウスカーソルの座標にあるボーダーを取得します。
					hot_border_pane = root->hittest_border(point);

					// ボーダーが有効かチェックします。
					if (hot_border_pane)
					{
						// オフセットを取得します。
						hot_border_pane->drag_offset = hot_border_pane->get_drag_offset(point);

						// マウスキャプチャを開始します。
						::SetCapture(hwnd);

						// 再描画します。
						hot_border_pane->invalidate();

						break;
					}

					{
						// マウスカーソルの座標にあるペインを取得できた場合は
						if (auto pane = root->hittest_pane(point))
						{
							// クリックされたペインがシャトルを持っている場合は
							if (auto shuttle = pane->get_current_shuttle())
								::SetFocus(*shuttle); // そのシャトルにフォーカスを当てます。
						}
					}

					break;
				}
			case WM_LBUTTONUP:
				{
					MY_TRACE_FUNC("WM_LBUTTONUP, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// マウスをキャプチャ中かチェックします。
					if (::GetCapture() == hwnd)
					{
						// マウスキャプチャを終了します。
						::ReleaseCapture();

						// ボーダーが有効かチェックします。
						if (hot_border_pane)
						{
							// ボーダーを動かします。
							hot_border_pane->drag_border(point);

							// レイアウトを更新します。
							hot_border_pane->update_origin();
						}
					}

					break;
				}
			case WM_LBUTTONDBLCLK:
				{
					MY_TRACE_FUNC("WM_LBUTTONDBLCLK, {:#010x}, {:#010x}", wParam, lParam);

					// ルートペインを取得します。
					auto root = get_root_pane(hwnd);

					// 既に最大化されたペインが存在するなら
					if (root->get_maximized_pane())
					{
						// 最大化を解除します。
						root->set_maximized_pane(nullptr);
					}
					else
					{
						// マウスカーソルの座標を取得します。
						auto point = my::lp_to_pt(lParam);

						// マウスカーソルの座標にあるペインを取得します。
						auto pane = root->hittest_pane(point);
						if (!pane) break;

						// ペインのファセットを取得します。
						auto caption_facet = pane->get_caption_facet();
						if (!caption_facet) break;

						// ペインのキャプション領域を取得します。
						auto caption_rc = caption_facet->get_caption_rect(pane.get());
						if (::PtInRect(&caption_rc, point))
						{
							// ペインのキャプションが
							// ダブルクリックされたので最大化します。
							root->set_maximized_pane(pane);
						}
					}

					// レイアウトを更新します。
					update_dock_site();

					break;
				}
			case WM_RBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_RBUTTONDOWN, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// ルートペインを取得します。
					auto root = get_top_level_pane(hwnd);
					if (!root) break;

					// 指定された座標からペインを取得します。
					auto pane = root->hittest_pane(point);
					if (!pane) break;

					// ペイン編集用メニューを表示します。
					show_pane_menu(hwnd, pane);

					break;
				}
			case WM_MOUSEMOVE:
				{
					// 同じ座標のWM_MOUSEMOVEが大量に送られてくるので間引きます。
					if (mouse_move_pos == lParam) break;
					mouse_move_pos = lParam;

//					MY_TRACE_FUNC("WM_MOUSEMOVE, {:#010x}, {:#010x}", wParam, lParam);

					// マウスカーソルの座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// マウスをキャプチャ中かチェックします。
					if (::GetCapture() == hwnd)
					{
						if (hot_border_pane)
						{
							// ボーダーを動かします。
							hot_border_pane->drag_border(point);

							// レイアウトを更新します。
							hot_border_pane->update_origin();
						}
					}
					else
					{
						// ルートペインを取得します。
						auto root = get_top_level_pane(hwnd);
						if (!root) break;

						// マウスカーソルの座標にあるボーダーを取得します。
						auto pane = root->hittest_border(point);

						// 現在のホットボーダーと違うなら
						if (hot_border_pane != pane)
						{
							// ホットボーダーを更新します。
							hot_border_pane = pane;

							// 再描画します。
							my::invalidate(hwnd);
						}

						// WM_MOUSELEAVEが発行されるようにします。
						my::track_mouse_event(hwnd);
					}

					break;
				}
			case WM_MOUSELEAVE:
				{
//					MY_TRACE_FUNC("WM_MOUSELEAVE, {:#010x}, {:#010x}", wParam, lParam);

					// ホットボーダーが存在する場合は
					if (hot_border_pane)
					{
						// ホットボーダーを再描画します。
						hot_border_pane->invalidate();

						// ホットボーダーを無効にします。
						hot_border_pane = nullptr;
					}

					break;
				}
			case WM_MOUSEWHEEL:
				{
					MY_TRACE_FUNC("WM_MOUSEWHEEL, {:#010x}, {:#010x}", wParam, lParam);

					// タブ上でマウスホイールが発生した場合はカレントタブを切り替えます。

					// デルタを取得します。
					auto delta = (short)HIWORD(wParam);

					// マウスカーソルの座標を取得します。
					auto point = my::lp_to_pt(lParam);
					my::map_window_points(nullptr, hwnd, &point);

					// マウスカーソルの座標にある子ウィンドウを取得します。
					auto child = ::ChildWindowFromPointEx(hwnd, point,
						CWP_SKIPDISABLED | CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);

					// childがタブならペインを取得できます。
					if (auto pane = Pane::get_pane(child))
					{
						auto c = pane->get_tab_count();
						auto current = pane->get_current_index();

						if (delta > 0)
						{
							current--;
						}
						else
						{
							current++;
						}

						current = std::max<int>(current, 0);
						current = std::min<int>(current, c - 1);

						pane->set_current_index(current);
						pane->update_origin();

						return 0;
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
