﻿#pragma once

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
			inline static constexpr uint32_t c_move_to_left = 1102;
			inline static constexpr uint32_t c_move_to_right = 1103;
			inline static constexpr uint32_t c_undock = 1104;
			inline static constexpr uint32_t c_is_solid = 1105;
			inline static constexpr uint32_t c_pane_config = 1106;
			inline static constexpr struct Shuttle {
				inline static constexpr uint32_t c_begin = 2000;
				inline static constexpr uint32_t c_end = 3000;
			} c_shuttle;
		} c_command_id;

		inline static constexpr auto c_prop_name = _T("apn::dock_site::root_pane");
		inline static std::shared_ptr<Pane> hot_border_pane;

		LPARAM mouse_move_pos = -1; // WM_MOUSEMOVEのLPARAMです。

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
		// メニューのコピーを作成します。
		//
		inline static void copy_menu(HMENU dst_menu, HMENU src_menu)
		{
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
				{
					auto sub_menu = ::CreatePopupMenu();
					copy_menu(sub_menu, mii.hSubMenu);
					mii.hSubMenu = sub_menu;
				}

				::InsertMenuItem(dst_menu, i, TRUE, &mii);
			}
		}

		//
		// ターゲットのメニューを表示します。
		//
		inline static BOOL show_target_menu(HWND dock_site, POINT point)
		{
			MY_TRACE_FUNC("{}, {}", point.x, point.y);

			// ルートペインを取得します。
			auto root = get_top_level_pane(dock_site);
			if (!root) return FALSE;

			// 指定された座標からペインを取得します。
			auto pane = root->hittest_pane(point);
			if (!pane) return FALSE;

			// カレントシャトルを取得します。
			auto shuttle = pane->get_current_shuttle();
			if (!shuttle) return FALSE;

			// ターゲットを取得します。
			auto hwnd = (HWND)*shuttle;

			// ターゲットのメニューを取得します。
			auto src_menu = ::GetMenu(hwnd);
			if (!src_menu) return FALSE;

			// 指定された座標がメニュー矩形の中にあるかチェックします。
			auto rc_menu = pane->get_menu_rect();
			if (!::PtInRect(&rc_menu, point)) return FALSE;

			// メニュー矩形からメニューを表示する座標を算出します。
			point.x = rc_menu.left;
			point.y = rc_menu.bottom;
			::ClientToScreen(dock_site, &point);

			// ポップアップメニューを作成します。
			auto dst_menu = ::CreatePopupMenu();
			copy_menu(dst_menu, src_menu);

			// ポップアップメニューを表示します。
			::TrackPopupMenuEx(dst_menu, TPM_VERPOSANIMATION, point.x, point.y, hwnd, nullptr);

			// ポップアップメニューを削除します。
			::DestroyMenu(dst_menu);

			return TRUE;
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
		inline static void show_pane_menu(HWND dock_site)
		{
			// ルートペインを取得します。
			auto root = get_root_pane(dock_site);
			if (!root || root->get_maximized_pane()) return;

			// カーソル座標を取得して、クライアント座標に変換します。
			auto cursor_pos = my::get_cursor_pos();
			auto point = cursor_pos;
			my::map_window_points(nullptr, dock_site, &point);

			// カーソル座標にヒットするペインを取得します。
			auto pane = root->hittest_pane(point);
			if (!pane) return;

			auto point2 = point;
			my::map_window_points(dock_site, pane->owner, &point2);
			auto in_caption = pane->hittest_caption(point2);

			auto c = pane->get_tab_count();
			auto ht = pane->hittest(point);

			// シャトルを取得します。取得できない場合もあります。
			auto shuttle = (Shuttle*)nullptr;
			if (ht != -1)
				shuttle = pane->get_shuttle(ht);
			else if (in_caption)
				shuttle = pane->get_shuttle(pane->get_current_index());

			// ターゲットがサブウィンドウか判定します。
			auto is_sub_window = shuttle && get_root_pane(*shuttle);

			// メニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// メニューにシャトルの一覧を追加します。
			add_shuttle_list(menu.get(), c_command_id.c_shuttle.c_begin,
				[=](HMENU sub_menu, UINT_PTR id, auto shuttle)
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

			::AppendMenu(menu.get(), MF_SEPARATOR, -1, nullptr);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_split_mode.c_none, _T("分割なし"));
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_split_mode.c_vert, _T("左右に分割"));
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_split_mode.c_horz, _T("上下に分割"));
			switch (pane->split_mode) {
			case Pane::c_split_mode.c_none: ::CheckMenuItem(menu.get(), c_command_id.c_split_mode.c_none, MF_CHECKED); break;
			case Pane::c_split_mode.c_vert: ::CheckMenuItem(menu.get(), c_command_id.c_split_mode.c_vert, MF_CHECKED); break;
			case Pane::c_split_mode.c_horz: ::CheckMenuItem(menu.get(), c_command_id.c_split_mode.c_horz, MF_CHECKED); break;
			}
			::AppendMenu(menu.get(), MF_SEPARATOR, -1, nullptr);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_origin.c_top_left, _T("左上を原点にする"));
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_origin.c_bottom_right, _T("右下を原点にする"));
			switch (pane->origin) {
			case Pane::c_origin.c_top_left: ::CheckMenuItem(menu.get(), c_command_id.c_origin.c_top_left, MF_CHECKED); break;
			case Pane::c_origin.c_bottom_right: ::CheckMenuItem(menu.get(), c_command_id.c_origin.c_bottom_right, MF_CHECKED); break;
			}
#if 0
			::AppendMenu(menu.get(), MF_SEPARATOR, -1, nullptr);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_caption_mode.c_show, _T("キャプションを表示する"));
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_caption_mode.c_hide, _T("キャプションを表示しない"));
			switch (pane->caption_mode) {
			case Pane::c_caption_mode.c_hide: ::CheckMenuItem(menu.get(), c_command_id.c_caption_mode.c_hide, MF_CHECKED); break;
			case Pane::c_caption_mode.c_show: ::CheckMenuItem(menu.get(), c_command_id.c_caption_mode.c_show, MF_CHECKED); break;
			}
#endif
			::AppendMenu(menu.get(), MF_SEPARATOR, -1, nullptr);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_move_to_left, _T("左に移動する"));
			if (ht == -1 || ht <= 0)
				::EnableMenuItem(menu.get(), c_command_id.c_move_to_left, MF_GRAYED | MF_DISABLED);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_move_to_right, _T("右に移動する"));
			if (ht == -1 || ht >= c - 1)
				::EnableMenuItem(menu.get(), c_command_id.c_move_to_right, MF_GRAYED | MF_DISABLED);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_is_border_locked, _T("ボーダーをロックする"));
			if (pane->is_border_locked)
				::CheckMenuItem(menu.get(), c_command_id.c_is_border_locked, MF_CHECKED);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_rename_sub_window, _T("名前を変更"));
			if (!is_sub_window) // ペインのオーナーがサブウィンドウではない場合はこのメニューアイテムを無効化します。
				::EnableMenuItem(menu.get(), c_command_id.c_rename_sub_window, MF_GRAYED | MF_DISABLED);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_is_solid, _T("レイアウトを固定化"));
			if (root->is_solid)
				::CheckMenuItem(menu.get(), c_command_id.c_is_solid, MF_CHECKED);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_pane_config, _T("ペインの設定"));
			::AppendMenu(menu.get(), MF_SEPARATOR, -1, nullptr);
			::AppendMenu(menu.get(), MF_STRING, c_command_id.c_undock, _T("ドッキングを解除"));
			if (c == 0) // ドッキングしているシャトルが存在しない場合はこのメニューアイテムを無効化します。
				::EnableMenuItem(menu.get(), c_command_id.c_undock, MF_DISABLED | MF_GRAYED);

			auto id = ::TrackPopupMenu(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD,
				cursor_pos.x, cursor_pos.y, 0, dock_site, nullptr);
			if (!id) return;

			switch (id)
			{
			case c_command_id.c_split_mode.c_none: pane->set_split_mode(Pane::c_split_mode.c_none); break;
			case c_command_id.c_split_mode.c_vert: pane->set_split_mode(Pane::c_split_mode.c_vert); break;
			case c_command_id.c_split_mode.c_horz: pane->set_split_mode(Pane::c_split_mode.c_horz); break;

			case c_command_id.c_origin.c_top_left: pane->set_origin(Pane::c_origin.c_top_left); break;
			case c_command_id.c_origin.c_bottom_right: pane->set_origin(Pane::c_origin.c_bottom_right); break;

			case c_command_id.c_caption_mode.c_hide: pane->set_caption_mode(Pane::c_caption_mode.c_hide); break;
			case c_command_id.c_caption_mode.c_show: pane->set_caption_mode(Pane::c_caption_mode.c_show); break;

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
					if (index == -1)
						index = pane->get_current_index();

					// インデックスが有効の場合は
					if (index != -1)
					{
						// ドッキングしているシャトルを取得します。
						auto shuttle = pane->get_shuttle(index);
						if (shuttle) // ドッキングしているシャトルが存在する場合は
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

			if (id >= c_command_id.c_shuttle.c_begin && id <= c_command_id.c_shuttle.c_end)
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
		// ルートペインをリセットします。
		//
		void reset_pane()
		{
			auto pane = get_root_pane(*this);
			if (!pane) return;
			pane->reset_pane();
		}

		//
		// クライアント矩形を返します。
		//
		virtual RECT get_client_rect()
		{
			return my::get_client_rect(*this);
		}

		//
		// すべてのペインのレイアウトを更新します。
		//
		virtual void update_dock_site(const RECT& rc)
		{
			auto root = get_root_pane(*this);
			if (!root) return;

			root->update_origin(&rc, Pane::c_update_flag.c_default);
		}

		//
		// すべてのペインのレイアウトを更新します。
		//
		void update_dock_site()
		{
			update_dock_site(get_client_rect());
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
							// タブからペインを取得します。
							auto pane = Pane::get_pane(header->hwndFrom);
							if (!pane) break;

							// タブ上で右クリックが発生した場合はペインメニューを表示します。
							show_pane_menu(hwnd);

							break;
						}
					case TCN_SELCHANGE:
						{
							// タブからペインを取得します。
							auto pane = Pane::get_pane(header->hwndFrom);
							if (!pane) break;

							// カレントシャトルが切り替わったので、ペインの表示状態を更新します。
							pane->update_origin();

							break;
						}
					}

					break;
				}
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

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
					update_dock_site();

					break;
				}
			case WM_SHOWWINDOW:
				{
					MY_TRACE_FUNC("WM_SHOWWINDOW, {:#010x}, {:#010x}", wParam, lParam);

					if (wParam)
					{
						auto root = get_root_pane(hwnd);

						root->update_origin();
					}

					break;
				}
			case WM_PAINT:
				{
					auto rc = my::get_client_rect(hwnd);

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

					rc = get_client_rect();

					{
						// 背景を塗りつぶします。

						painter.fill_background(dc, &rc);
					}

					{
						// ボーダーを再帰的に描画します。

						root->draw_border(dc);
					}

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
						// 各ウィンドウのキャプションを再帰的に描画します。

						LOGFONTW lf = {};
						::GetThemeSysFont(hive.theme.get(), TMT_CAPTIONFONT, &lf);
						my::gdi::unique_ptr<HFONT> font(::CreateFontIndirectW(&lf));
						my::gdi::selector font_selector(dc, font.get());

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

					auto point = my::get_cursor_pos();
					::ScreenToClient(hwnd, &point);

					auto border_pane = root->hittest_border(point);
					if (!border_pane) break;

					switch (border_pane->split_mode)
					{
					case Pane::c_split_mode.c_vert:
						{
							::SetCursor(::LoadCursor(nullptr, IDC_SIZEWE));

							return TRUE;
						}
					case Pane::c_split_mode.c_horz:
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

					// マウス座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// 必要であればターゲットウィンドウのメニューを表示します。
					if (show_target_menu(hwnd, point))
						break; // メニューの表示が行われた場合はここで処理を終わります。

					// ルートペインを取得します。
					auto root = get_top_level_pane(hwnd);
					if (!root) break;

					// マウス座標にあるボーダーを取得します。
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
						// マウス座標にあるペインを取得できたら
						auto pane = root->hittest_pane(point);
						if (pane)
						{
							// クリックされたペインがシャトルを持っているなら
							auto shuttle = pane->get_current_shuttle();
							if (shuttle)
								::SetFocus(*shuttle); // そのシャトルにフォーカスを当てます。
						}
					}

					break;
				}
			case WM_LBUTTONUP:
				{
					MY_TRACE_FUNC("WM_LBUTTONUP, {:#010x}, {:#010x}", wParam, lParam);

					// マウス座標を取得します。
					auto point = my::lp_to_pt(lParam);

					// マウスをキャプチャ中かチェックします。
					if (::GetCapture() == hwnd)
					{
						// マウスキャプチャを終了します。
						::ReleaseCapture();

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

					// 最大化されたペインが存在するなら
					if (root->get_maximized_pane())
					{
						// 最大化を解除します。
						root->set_maximized_pane(nullptr);
					}
					else
					{
						// マウス座標を取得します。
						auto point = my::lp_to_pt(lParam);

						// マウス座標にあるキャプションのペインを取得します。
						auto pane = root->hittest_pane(point);
						if (!pane || !pane->hittest_caption(point)) break;

						// キャプションをダブルクリックされたペインを
						// 最大化されたペインにセットします。
						root->set_maximized_pane(pane);
					}

					// レイアウトを更新します。
					update_dock_site();

					break;
				}
			case WM_RBUTTONDOWN:
				{
					MY_TRACE_FUNC("WM_RBUTTONDOWN, {:#010x}, {:#010x}", wParam, lParam);

					// ペイン編集用メニューを表示します。
					show_pane_menu(hwnd);

					break;
				}
			case WM_MOUSEMOVE:
				{
					// 同じ座標のWM_MOUSEMOVEが大量に送られてくるので間引きます。
					if (mouse_move_pos == lParam) break;
					mouse_move_pos = lParam;

//					MY_TRACE_FUNC("WM_MOUSEMOVE, {:#010x}, {:#010x}", wParam, lParam);

					// マウス座標を取得します。
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

						// マウス座標にあるボーダーを取得します。
						auto pane = root->hittest_border(point);

						// 現在のホットボーダーと違うなら
						if (hot_border_pane != pane)
						{
							// ホットボーダーを更新します。
							hot_border_pane = pane;

							// 再描画します。
							::InvalidateRect(hwnd, nullptr, FALSE);
						}

						// マウスリーブイベントをトラックします。
						TRACKMOUSEEVENT tme = { sizeof(tme) };
						tme.dwFlags = TME_LEAVE;
						tme.hwndTrack = hwnd;
						::TrackMouseEvent(&tme);
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

					auto delta = (short)HIWORD(wParam);
					auto point = my::lp_to_pt(lParam);
					my::map_window_points(nullptr, hwnd, &point);
					auto child = ::ChildWindowFromPointEx(hwnd, point,
						CWP_SKIPDISABLED | CWP_SKIPINVISIBLE | CWP_SKIPTRANSPARENT);

					auto pane = Pane::get_pane(child);
					if (pane) // childがタブならペインを取得できます。
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

		//
		// このクラスはペインの設定を変更するためのダイアログです。
		//
		struct PaneConfigDialog : my::Dialog
		{
			//
			// 編集対象のペインです。
			//
			std::shared_ptr<Pane> pane;

			//
			// コンストラクタです。
			//
			PaneConfigDialog(const std::shared_ptr<Pane>& pane, HWND parent)
				: pane(pane)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_PANE_CONFIG), parent);
			}

			//
			// ペインのサイズ(エディットボックス)を更新します。
			//
			void update_pane_size_editbox()
			{
				auto origin = get_combobox_index(IDC_PANE_ORIGIN);
				auto border_pos = get_int(IDC_PANE_BORDER_POS);

				auto top_left = int32_t {};
				auto bottom_right = int32_t {};

				auto w = (pane->split_mode == pane->c_split_mode.c_vert) ?
					my::get_width(pane->position) : my::get_height(pane->position);

				if (origin == pane->c_origin.c_top_left)
				{
					top_left = border_pos;
					bottom_right = w - (border_pos + pane->border_width);
				}
				else
				{
					top_left = w - (border_pos - pane->border_width);
					bottom_right = border_pos;
				}

				set_int(IDC_PANE_TOP_LEFT, top_left);
				set_int(IDC_PANE_BOTTOM_RIGHT, bottom_right);
			}

			//
			// ダイアログを表示します。
			//
			int do_modal()
			{
				auto x = pane->position.left;
				auto y = pane->position.top;
				auto w = my::get_width(pane->position);
				auto h = my::get_height(pane->position);

				init_combobox(IDC_PANE_ORIGIN, _T("左上"), _T("右下"));

				set_int(IDC_PANE_X, x);
				set_int(IDC_PANE_Y, y);
				set_int(IDC_PANE_W, w);
				set_int(IDC_PANE_H, h);
				set_combobox_index(IDC_PANE_ORIGIN, pane->origin);
				set_int(IDC_PANE_BORDER_POS, pane->border);

				auto result = __super::do_modal();
				if (result != IDOK) return result;

				get_combobox_index(IDC_PANE_ORIGIN, pane->origin);
				get_int(IDC_PANE_BORDER_POS, pane->border);

				return result;
			}

			//
			// ダイアログプロシージャです。
			//
			virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						auto id = LOWORD(wParam);
						auto code = HIWORD(wParam);
						auto control = (HWND)lParam;

						switch (id)
						{
						case IDC_PANE_ORIGIN:
							{
								if (code == CBN_SELCHANGE)
									update_pane_size_editbox();

								break;
							}
						case IDC_PANE_BORDER_POS:
							{
								if (code == EN_CHANGE)
									update_pane_size_editbox();

								break;
							}
						}

						break;
					}
				}

				return __super::on_dlg_proc(hwnd, message, wParam, lParam);
			}
		};
	};
}
