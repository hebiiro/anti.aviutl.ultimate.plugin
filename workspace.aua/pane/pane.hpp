#pragma once

namespace apn::workspace
{
	struct Pane : Shuttle::Listener, std::enable_shared_from_this<Pane>
	{
		inline static struct SplitMode {
			inline static constexpr int32_t c_none = 0;
			inline static constexpr int32_t c_vert = 1;
			inline static constexpr int32_t c_horz = 2;
			inline static constexpr my::Label labels[] = {
				{ c_none, L"none" },
				{ c_vert, L"vert" },
				{ c_horz, L"horz" },
			};
		} c_split_mode;

		inline static struct Origin {
			inline static constexpr int32_t c_top_left = 0;
			inline static constexpr int32_t c_bottom_right = 1;
			inline static constexpr my::Label labels[] = {
				{ c_top_left, L"top_left" },
				{ c_bottom_right, L"bottom_right" },
			};
		} c_origin;

		inline static struct CaptionMode {
			inline static constexpr int32_t c_hide = 0;
			inline static constexpr int32_t c_show = 1;
			inline static constexpr my::Label labels[] = {
				{ c_hide, L"hide" },
				{ c_show, L"show" },
			};
		} c_caption_mode;

		inline static struct TabMode {
			inline static constexpr int32_t c_caption = 0;
			inline static constexpr int32_t c_top = 1;
			inline static constexpr int32_t c_bottom = 2;
			inline static constexpr my::Label labels[] = {
				{ c_caption, L"caption" },
				{ c_top, L"top" },
				{ c_bottom, L"bottom" },
			};
		} c_tab_mode;

		inline static struct UpdateFlag {
			inline static constexpr uint32_t c_deep = 0x00000001;
			inline static constexpr uint32_t c_invalidate = 0x00000002;
			inline static constexpr uint32_t c_set_focus = 0x00000004;
			inline static constexpr uint32_t c_normalize = 0x00000008;
			inline static constexpr uint32_t c_show_current_shuttle = 0x00000010;
			inline static constexpr uint32_t c_show_tab = 0x00000020;
			inline static constexpr uint32_t c_origin = 0x00000040;
			inline static constexpr uint32_t c_default =
				c_deep | c_invalidate | c_set_focus | c_normalize |
				c_show_current_shuttle | c_show_tab | c_origin;
		} c_update_flag;

		inline static constexpr auto c_prop_name = _T("apn::workspace::pane");
		inline static auto border_width = 8;
		inline static auto caption_height = 24;
		inline static auto tab_height = 24;
		inline static auto tab_mode = TabMode::c_bottom;

		HWND owner = nullptr;
		RECT position = {};
		int32_t split_mode = SplitMode::c_none;
		int32_t origin = Origin::c_bottom_right;
		int32_t caption_mode = CaptionMode::c_show;
		BOOL is_border_locked = FALSE;
		int32_t border = 0; // ボーダーの位置です。
		int32_t drag_offset = 0; // ドラッグ処理に使用します。

		Tab tab; // タブコントロールです。
		std::shared_ptr<Pane> children[2]; // 子ペインです。

		//
		// コンストラクタです。
		// ownerはこのペインを保有するウィンドウ(DockSiteの派生クラス)です。
		//
		Pane(HWND owner)
			: owner(owner)
		{
			// タブを作成します。
			tab.create(
				WS_EX_NOPARENTNOTIFY,
				WC_TABCONTROL,
				_T("apn::workspace::pane::tab"),
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN |
				TCS_FOCUSNEVER,
				0, 0, 0, 0,
				owner, nullptr, hive.instance, nullptr);
			tab.set_font();

			// このペインをタブに関連付けます。
			set_pane(tab, this);
		}

		//
		// デストラクタです。
		//
		virtual ~Pane()
		{
			// タブを削除します。
			tab.destroy();
		}

		//
		// 指定されたウィンドウハンドルに関連付けられているペインを返します。
		//
		inline static Pane* get_pane(HWND hwnd)
		{
			return (Pane*)::GetProp(hwnd, c_prop_name);
		}

		//
		// 指定されたウィンドウハンドルにペインを関連付けます。
		//
		static void set_pane(HWND hwnd, Pane* pane)
		{
			::SetProp(hwnd, c_prop_name, pane);
		}

		//
		// タブの数を返します。
		//
		int32_t get_tab_count()
		{
			return ::SendMessage(tab, TCM_GETITEMCOUNT, 0, 0);
		}

		//
		// 指定されたインデックスにあるタブのシャトルを返します。
		//
		Shuttle* get_shuttle(int32_t index)
		{
			if (index == -1) return nullptr;

			TCITEM item = {};
			item.mask = TCIF_PARAM;
			::SendMessage(tab, TCM_GETITEM, index, (LPARAM)&item);

			return (Shuttle*)item.lParam;
		}

		//
		// タブのカレントインデックスを返します。
		//
		int32_t get_current_index()
		{
			return ::SendMessage(tab, TCM_GETCURSEL, 0, 0);
		}

		//
		// タブのカレントインデックスを指定されたインデックスに変更します。
		//
		int32_t set_current_index(int32_t index)
		{
			if (index < 0 || index >= get_tab_count())
				index = 0;

			return ::SendMessage(tab, TCM_SETCURSEL, index, 0);
		}

		//
		// 指定された座標にあるタブのインデックスを返します。
		// pointはタブの親ウィンドウの座標系で指定します。
		//
		int32_t hittest(POINT point)
		{
			TCHITTESTINFO hti = {};
			hti.pt = point;
			::MapWindowPoints(::GetParent(tab), tab, &hti.pt, 1);
			return ::SendMessage(tab, TCM_HITTEST, 0, (LPARAM)&hti);
		}

		//
		// 指定されたインデックスの位置にタブを追加します。
		// shuttleはタブに関連付けるシャトルです。
		// textはタブ名です。
		//
		int32_t add_tab(Shuttle* shuttle, LPCTSTR text, int32_t index)
		{
			TCITEM item = {};
			item.mask = TCIF_PARAM | TCIF_TEXT;
			item.lParam = (LPARAM)shuttle;
			item.pszText = (LPTSTR)text;
			return ::SendMessage(tab, TCM_INSERTITEM, index, (LPARAM)&item);
		}

		//
		// 指定されたインデックスのタブを削除します。
		//
		void erase_tab(int32_t index)
		{
			::SendMessage(tab, TCM_DELETEITEM, index, 0);
		}

		//
		// すべてのタブを削除します。
		//
		void clear_all_tabs()
		{
			::SendMessage(tab, TCM_DELETEALLITEMS, 0, 0);
		}

		//
		// 指定されたシャトルを持つタブのインデックスを返します。
		// 存在しない場合は-1を返します。
		//
		int32_t find_tab(Shuttle* shuttle)
		{
			auto c = get_tab_count();
			for (decltype(c) i = 0; i < c; i++)
			{
				if (get_shuttle(i) == shuttle)
					return i;
			}

			return -1;
		}

		//
		// fromで指定された位置にあるタブをtoで指定された位置へ移動します。
		// 失敗した場合は-1を返します。
		//
		int32_t move_tab(int32_t from, int32_t to)
		{
			auto c = get_tab_count();

			if (from < 0 || from >= c) return -1;
			if (to < 0 || to >= c) return -1;

			auto current = get_current_index();
			std::vector<TCHAR> text(MAX_PATH, _T('\0'));
			TCITEM item = {};
			item.mask = TCIF_PARAM | TCIF_TEXT;
			item.pszText = text.data();
			item.cchTextMax = text.size();
			::SendMessage(tab, TCM_GETITEM, from, (LPARAM)&item);
			::SendMessage(tab, TCM_DELETEITEM, from, 0);
			::SendMessage(tab, TCM_INSERTITEM, to, (LPARAM)&item);
			if (from == current) set_current_index(to);

			return to;
		}

		//
		// 指定されたシャトルを持つタブを指定されたタブ名に変更します。
		//
		void set_tab_text(Shuttle* shuttle, LPCTSTR text)
		{
			auto index = find_tab(shuttle);
			if (index == -1) return;

			TCITEM item = {};
			item.mask = TCIF_TEXT;
			item.pszText = (LPTSTR)text;
			::SendMessage(tab, TCM_SETITEM, index, (LPARAM)&item);
		}

		//
		// このペインがタブを持つ(表示する)場合はTRUEを返します。
		//
		BOOL has_tab()
		{
			auto c = get_tab_count();
			if (hive.show_tab_force)
				return c >= 1;
			else
				return c >= 2;
		}

		//
		// カレントタブのシャトルを返します。
		//
		Shuttle* get_current_shuttle()
		{
			auto current = get_current_index();
			return get_shuttle(current);
		}

		//
		// 指定されたシャトルをこのペインにドッキングします。
		//
		void dock_shuttle(Shuttle* shuttle)
		{
			set_pane(*shuttle, this);
			shuttle->add_listener(this);
			shuttle->dock_window(owner);
		}

		//
		// 指定されたシャトルをこのペインから切り離します。
		//
		void float_shuttle(Shuttle* shuttle)
		{
			set_pane(*shuttle, nullptr);
			shuttle->remove_listener(this);
			shuttle->float_window();
		}

		//
		// 指定されたタブインデックスの位置に指定されたシャトルを追加します。
		//
		int32_t insert_shuttle(Shuttle* shuttle, BOOL show, int32_t index = -1)
		{
			MY_TRACE_FUNC("{:#010x}, {}", shuttle, index);

			// シャトルはすでにこのペインとドッキング済みなので何もしません。
			if (find_tab(shuttle) != -1) return -1;

			// シャトルが他のペインとドッキング中かもしれないので、ドッキングを解除させます。
			shuttle->fire_release_shuttle();

			// 追加位置が無効の場合は末尾に追加します。
			if (index == -1)
				index = get_tab_count();

			// ウィンドウテキストを取得します。
			auto text = my::get_window_text(*shuttle);

			// タブを追加します。
			auto result = add_tab(shuttle, text.c_str(), index);

			// シャトルをドッキング状態にします。
			dock_shuttle(shuttle);

			return result;
		}

		//
		// タブから指定されたシャトルを削除します。
		//
		void erase_shuttle(Shuttle* shuttle)
		{
			MY_TRACE_FUNC("{:#010x}", shuttle);

			// 指定されたシャトルを保持しているかチェックします。
			auto index = find_tab(shuttle);
			if (index == -1) return; // シャトルを保持していない場合は何もしません。

			// シャトルがカレントかどうかチェックしておきます。
			auto same = index == get_current_index();

			// タブを削除します。
			erase_tab(index);

			// 削除されたタブとカレントタブが同じなら
			if (same)
			{
				// カレントタブを設定し直します。
				set_current_index(index);
			}

			// カレントタブが無効なら
			if (get_current_index() == -1)
			{
				// 末尾のタブをカレントにします。
				set_current_index(get_tab_count() - 1);
			}

			// カレントシャトルが切り替わったので、ペインの表示状態を更新します。
			update_origin();

			// シャトルをフローティング状態にします。
			float_shuttle(shuttle);
		}

		//
		// タブからすべてのシャトルを削除します。
		//
		void clear_all_shuttles()
		{
			MY_TRACE_FUNC("");

			// すべてのシャトルをフローティング状態にします。

			auto c = get_tab_count();
			for (decltype(c) i = 0; i < c; i++)
			{
				auto shuttle = get_shuttle(i);

				float_shuttle(shuttle);
			}

			// すべてのタブを削除します。
			clear_all_tabs();
		}

		//
		// ペインをリセットします。
		//
		virtual void reset_pane()
		{
			clear_all_shuttles();

			for (auto& child : children)
			{
				if (!child) continue;

				child->reset_pane();
				child = nullptr;
			}
		}

		//
		// ペインの分割モードを変更します。
		//
		void set_split_mode(int32_t new_split_mode)
		{
			split_mode = new_split_mode;

			switch (split_mode)
			{
			case SplitMode::c_none:
				{
					reset_pane();

					break;
				}
			case SplitMode::c_vert:
				{
					border = my::get_width(position) / 2;

					if (!children[0]) children[0] = std::make_shared<Pane>(owner);
					if (!children[1]) children[1] = std::make_shared<Pane>(owner);

					break;
				}
			case SplitMode::c_horz:
				{
					border = my::get_height(position) / 2;

					if (!children[0]) children[0] = std::make_shared<Pane>(owner);
					if (!children[1]) children[1] = std::make_shared<Pane>(owner);

					break;
				}
			}

			clear_all_shuttles();
		}

		//
		// ペインの原点を変更します。
		// そのとき、ボーダーの見た目の位置が変化しないように調整します。
		//
		void set_origin(int32_t new_origin)
		{
			if (origin == new_origin) return;
			origin = new_origin;
			if (split_mode == Pane::SplitMode::c_none) return;

			border = -border_width - border;

			switch (split_mode)
			{
			case Pane::SplitMode::c_horz: border += position.bottom - position.top; break;
			case Pane::SplitMode::c_vert: border += position.right - position.left; break;
			}
		}

		//
		// キャプションモードを変更します。
		//
		void set_caption_mode(int32_t new_caption_mode)
		{
			if (caption_mode == new_caption_mode) return;
			caption_mode = new_caption_mode;
		}

		//
		// このペインのキャプションの高さを返します。
		//
		int32_t get_caption_height()
		{
			if (caption_mode == CaptionMode::c_hide) return 0;
			return caption_height;
		}

		inline static int32_t clamp(int32_t x, int32_t minValue, int32_t maxValue)
		{
			if (x < minValue) return minValue;
			if (x > maxValue) return maxValue;
			return x;
		}

		int32_t absolute_x(int32_t x)
		{
			switch (origin)
			{
			case Origin::c_top_left: return position.left + x;
			case Origin::c_bottom_right: return position.right - x - border_width;
			}

			return 0;
		}

		int32_t absolute_y(int32_t y)
		{
			switch (origin)
			{
			case Origin::c_top_left: return position.top + y;
			case Origin::c_bottom_right: return position.bottom - y - border_width;
			}

			return 0;
		}

		int32_t relative_x(int32_t x)
		{
			switch (origin)
			{
			case Origin::c_top_left: return x - position.left;
			case Origin::c_bottom_right: return position.right - x + border_width;
			}

			return 0;
		}

		int32_t relative_y(int32_t y)
		{
			switch (origin)
			{
			case Origin::c_top_left: return y - position.top;
			case Origin::c_bottom_right: return position.bottom - y + border_width;
			}

			return 0;
		}

		//
		// ドッキング領域を返します。
		//
		RECT get_dock_rect()
		{
			// キャプションの高さを取得します。
			auto caption_height = get_caption_height();

			// このペインがタブを表示する場合は
			if (has_tab())
			{
				// タブを考慮に入れてドッキング領域を返します。
				switch (tab_mode)
				{
				case TabMode::c_caption: // タブをキャプションに配置する場合
					{
						return RECT
						{
							position.left,
							position.top + std::max<int>(caption_height, tab_height),
							position.right,
							position.bottom,
						};
					}
				case TabMode::c_top: // タブを上に配置する場合
					{
						return RECT
						{
							position.left,
							position.top + caption_height + tab_height,
							position.right,
							position.bottom,
						};
					}
				case TabMode::c_bottom: // タブを下に配置する場合
					{
						return RECT
						{
							position.left,
							position.top + caption_height,
							position.right,
							position.bottom - tab_height,
						};
					}
				}
			}

			// タブを考慮に入れずにドッキング領域を返します。
			return RECT
			{
				position.left,
				position.top + caption_height,
				position.right,
				position.bottom,
			};
		}

		//
		// キャプション領域を返します。
		//
		RECT get_caption_rect()
		{
			return RECT
			{
				position.left,
				position.top,
				position.right,
				position.top + caption_height,
			};
		}

		//
		// メニューボタンの領域を返します。
		//
		RECT get_menu_rect()
		{
			return RECT
			{
				position.right - caption_height,
				position.top,
				position.right,
				position.top + caption_height,
			};
		}

		void normalize()
		{
//			MY_TRACE_FUNC("");

			switch (split_mode)
			{
			case SplitMode::c_vert: border = std::clamp(border, 0, my::get_width(position) - border_width); break;
			case SplitMode::c_horz: border = std::clamp(border, 0, my::get_height(position) - border_width); break;
			}
		}

		//
		// このペインを起点にして、
		// このペインおよび子孫ペインの位置情報を更新します。
		//
		void update_origin(uint32_t flags = UpdateFlag::c_default)
		{
			update_origin(&position, flags);
		}

		//
		// このペインを起点にして、
		// このペインおよび子孫ペインの位置情報を更新します。
		//
		void update_origin(LPCRECT rc, uint32_t flags = UpdateFlag::c_default)
		{
			auto dwp = ::BeginDeferWindowPos(100);
			update(dwp, rc, flags);
			::EndDeferWindowPos(dwp);
		}

		//
		// このペインおよび子孫ペインの位置情報を更新します。
		// rcはこのペインの新しい位置です。
		//
		virtual void update(HDWP dwp, LPCRECT rc, uint32_t flags)
		{
//			MY_TRACE_FUNC("");

			if (::IsIconic(owner))
				return;

			// このペインの位置を更新します。
			position = *rc;

			if (flags & UpdateFlag::c_invalidate)
			{
				// ペイン矩形が変更されたので、
				// オーナーにペイン矩形の再描画処理を発注しておきます。
				invalidate();
			}

			// タブの数が最小数以上の場合は
			if (has_tab() && flags & UpdateFlag::c_show_tab)
			{
				update_tab(dwp);
			}
			// タブの数が最小数未満の場合は
			else
			{
				// タブコントロールを非表示にします。
				::DeferWindowPos(dwp, tab, nullptr, 0, 0, 0, 0,
					SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
//				::ShowWindow(tab, SW_HIDE);
			}

			// シャトルを持っている場合は
			auto c = get_tab_count();
			if (c)
			{
				if (flags & UpdateFlag::c_show_current_shuttle)
				{
					// カレントシャトルのみを表示し、フォーカスを与えます。
					update_shuttles(dwp, flags);
				}

				return; // シャトルを持つペインはボーダーも子ペインも持たないので、ここで処理を終了します。
			}

			if (flags & UpdateFlag::c_normalize)
			{
				// ボーダーが飛び出ないように調整します。
				normalize();
			}

			if (flags & UpdateFlag::c_deep)
			{
				// 子ペインを再帰的に更新します。
				// すでに親ペインの再描画が発行されている場合は
				// c_invalidateフラグは冗長になるので除外します。
				update_children(dwp, flags & ~UpdateFlag::c_invalidate);
			}
		}

		//
		// タブのウィンドウ位置を更新します。
		// update()から呼び出されます。
		//
		void update_tab(HDWP dwp)
		{
			// キャプションの高さを取得します。
			auto caption_height = get_caption_height();

			switch (tab_mode)
			{
			case TabMode::c_caption: // タブをキャプションに表示するなら
				{
					auto cx = 0;
					auto cy = std::max<int32_t>(caption_height, tab_height);

					// 現在表示しているシャトルを取得します。
					auto shuttle = get_current_shuttle();
					if (shuttle)
					{
						// シャトルがメニューを持っている場合は
						if (::GetMenu(*shuttle))
						{
							// メニューアイコンのスペースを確保します。
							cx = cy;
						}
					}

					auto x = position.left;
					auto y = position.top;
					auto w = my::get_width(position) - cx; // 右側にあるメニューアイコンの分だけ少し狭めます。
					auto h = cy;

					modify_style(tab, TCS_BOTTOM, 0);

					// タブコントロールを表示します。
					::DeferWindowPos(dwp, tab, HWND_TOP,
						x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

					break;
				}
			case TabMode::c_top: // タブを上に表示するなら
				{
					auto x = position.left;
					auto y = position.top + caption_height;
					auto w = my::get_width(position);
					auto h = tab_height;

					modify_style(tab, TCS_BOTTOM, 0);

					// タブコントロールを表示します。
					::DeferWindowPos(dwp, tab, HWND_TOP,
						x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

					break;
				}
			case TabMode::c_bottom: // タブを下に表示するなら
				{
					auto x = position.left;
					auto y = position.bottom - tab_height;
					auto w = my::get_width(position);
					auto h = tab_height;

					modify_style(tab, 0, TCS_BOTTOM);

					// タブコントロールを表示します。
					::DeferWindowPos(dwp, tab, HWND_TOP,
						x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);

					break;
				}
			}
		}

		//
		// シャトルを更新します。
		// update()から呼び出されます。
		//
		void update_shuttles(HDWP dwp, uint32_t flags)
		{
			// タブの数を取得します。
			auto c = get_tab_count();

			// カレントシャトルのインデックスを取得します。
			auto current_index = get_current_index();

			// ドッキング領域の矩形を取得します。
			auto rc_dock = get_dock_rect();

			// このペインにドッキングしているすべてのシャトルを順々に処理します。
			for (decltype(c) i = 0; i < c; i++)
			{
				// インデックスからシャトルを取得します。
				auto shuttle = get_shuttle(i);

				// シャトルをリサイズします。
				shuttle->resize(&rc_dock);

				MY_TRACE("「{}」の表示状態を変更します\n", shuttle->name);

				MY_TRACE_INT(::IsWindowVisible(*shuttle));
				MY_TRACE_INT(::IsWindowVisible(*shuttle->dock_container));
				MY_TRACE_INT(::IsWindowVisible(::GetParent(*shuttle->dock_container)));

				if (i == current_index)
				{
					// 空の設定ダイアログかどうかチェックします。
					if (*shuttle == hive.setting_dialog &&
						magi.exin.get_current_object_index() == -1)
					{
						MY_TRACE("空の設定ダイアログなので表示できません\n");

						::DeferWindowPos(dwp, *shuttle->dock_container,
							nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
//						::ShowWindow(*shuttle->dock_container, SW_HIDE);
					}
					else
					{
						MY_TRACE("「{}」を表示します\n", shuttle->name);
#if 1
						::DeferWindowPos(dwp, *shuttle->dock_container,
							HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
						::SendMessage(*shuttle->dock_container, WM_SHOWWINDOW, TRUE, 0);
#else
						// SWP_SHOWWINDOWを使用してウィンドウを表示した場合は
						// WM_SHOWWINDOWが送信されないので使用できません。
						::SetWindowPos(*shuttle->dock_container,
							HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
						::ShowWindow(*shuttle->dock_container, SW_SHOW);
#endif
					}

					if (flags & UpdateFlag::c_set_focus)
					{
						if (::IsWindowVisible(*shuttle))
							::SetFocus(*shuttle);
					}
				}
				else
				{
					MY_TRACE("「{}」を非表示にします\n", shuttle->name);
#if 1
					::DeferWindowPos(dwp, *shuttle->dock_container,
						nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
					::SendMessage(*shuttle->dock_container, WM_SHOWWINDOW, FALSE, 0);
#else
					::ShowWindow(*shuttle->dock_container, SW_HIDE);
#endif
				}

				MY_TRACE_INT(::IsWindowVisible(*shuttle));
				MY_TRACE_INT(::IsWindowVisible(*shuttle->dock_container));
				MY_TRACE_INT(::IsWindowVisible(::GetParent(*shuttle->dock_container)));
			}
		}

		//
		// 子ペインを更新します。
		// update()から呼び出されます。
		//
		void update_children(HDWP dwp, uint32_t flags)
		{
			switch (split_mode)
			{
			case SplitMode::c_vert:
				{
					auto abs_border = absolute_x(border);

					if (children[0])
					{
						RECT rc = { position.left, position.top, abs_border, position.bottom };

						children[0]->update(dwp, &rc, flags);
					}

					if (children[1])
					{
						RECT rc = { abs_border + border_width, position.top, position.right, position.bottom };

						children[1]->update(dwp, &rc, flags);
					}

					break;
				}
			case SplitMode::c_horz:
				{
					auto abs_border = absolute_y(border);

					if (children[0])
					{
						RECT rc = { position.left, position.top, position.right, abs_border };

						children[0]->update(dwp, &rc, flags);
					}

					if (children[1])
					{
						RECT rc = { position.left, abs_border + border_width, position.right, position.bottom };

						children[1]->update(dwp, &rc, flags);
					}

					break;
				}
			}
		}

		//
		// 指定された位置がキャプション内の場合はTRUEを返します。
		//
		BOOL hittest_caption(const POINT& point)
		{
			RECT rc = get_caption_rect();

			return ::PtInRect(&rc, point);
		}

		//
		// 指定された位置にあるペインを返します。
		//
		std::shared_ptr<Pane> hittest_pane(const POINT& point)
		{
			// pointがこのペインの範囲外なら
			if (!::PtInRect(&position, point))
				return nullptr; // ヒットしていないのでnullptrを返します。

			// このペインがシャトルを持っているなら
			if (get_tab_count())
				return shared_from_this(); // ヒットしているのでこのペインを返します。

			switch (split_mode)
			{
			case SplitMode::c_vert:
			case SplitMode::c_horz:
				{
					// 子ペインに再帰的に処理させます。
					for (auto& child : children)
					{
						if (!child) continue;

						auto pane = child->hittest_pane(point);
						if (pane) return pane;
					}

					break;
				}
			}

			return shared_from_this(); // ボーダーなどにヒットしているのでこのペインを返します。
		}

		//
		// 指定された位置にあるボーダーを持つペインを返します。
		//
		std::shared_ptr<Pane> hittest_border(const POINT& point)
		{
			// pointがこのペインの範囲外なら
			if (!::PtInRect(&position, point))
				return nullptr; // ヒットしていないのでnullptrを返します。

			// このペインがウィンドウを持っているなら
			if (get_tab_count())
				return nullptr; // ヒットしていないのでnullptrを返します。

			// このペインのボーダーがロックされていないなら
			if (!is_border_locked || ::GetKeyState(VK_CONTROL) < 0)
			{
				switch (split_mode)
				{
				case SplitMode::c_vert:
					{
						auto abs_border = absolute_x(border);

						// pointがボーダーの範囲内なら
						if (point.x >= abs_border && point.x < abs_border + border_width)
							return shared_from_this(); // ヒットしているのでこのペインを返します。

						break;
					}
				case SplitMode::c_horz:
					{
						auto abs_border = absolute_y(border);

						// pointがボーダーの範囲内なら
						if (point.y >= abs_border && point.y < abs_border + border_width)
							return shared_from_this(); // ヒットしているのでこのペインを返します。

						break;
					}
				default:
					{
						return nullptr;
					}
				}
			}

			// 子ペインに再帰的に処理させます。
			for (auto& child : children)
			{
				if (!child) continue;

				auto pane = child->hittest_border(point);
				if (pane) return pane;
			}

			return nullptr;
		}

		//
		// ドラッグ量を返します。
		// 内部的に使用されます。
		//
		int32_t get_drag_offset(POINT point)
		{
			switch (split_mode)
			{
			case SplitMode::c_vert: return border - relative_x(point.x);
			case SplitMode::c_horz: return border - relative_y(point.y);
			}

			return 0;
		}

		//
		// ボーダーをドラッグ移動します。
		// 内部的に使用されます。
		//
		void drag_border(POINT point)
		{
			switch (split_mode)
			{
			case SplitMode::c_vert: border = relative_x(point.x) + drag_offset; break;
			case SplitMode::c_horz: border = relative_y(point.y) + drag_offset; break;
			}
		}

		//
		// ボーダー矩形を返します。
		//
		BOOL get_border_rect(LPRECT rc)
		{
			// シャトルを持っている場合は
			if (get_tab_count())
				return FALSE; // ボーダーを持たないのでFALSEを返します。

			switch (split_mode)
			{
			case SplitMode::c_vert:
				{
					auto abs_border = absolute_x(border);

					rc->left = abs_border;
					rc->top = position.top;
					rc->right = abs_border + border_width;
					rc->bottom = position.bottom;

					return TRUE;
				}
			case SplitMode::c_horz:
				{
					auto abs_border = absolute_y(border);

					rc->left = position.left;
					rc->top = abs_border;
					rc->right = position.right;
					rc->bottom = abs_border + border_width;

					return TRUE;
				}
			}

			return FALSE;
		}

		//
		// ボーダーを描画します。
		//
		void draw_border(HDC dc)
		{
			// シャトルを持っている場合は
			if (get_tab_count())
				return; // ボーダーを持たないので何もしません。

			switch (split_mode)
			{
			case SplitMode::c_vert:
			case SplitMode::c_horz:
				{
					auto rc = RECT {};
					if (get_border_rect(&rc))
					{
						if (is_border_locked)
						{
							// 無効状態のボーダーを描画します。
							painter.fill_background(dc, &rc, hive.border_color, WP_SEPARATOR, CS_DISABLED);
						}
						else
						{
							// 通常状態のボーダーを描画します。
							painter.fill_background(dc, &rc, hive.border_color, WP_SEPARATOR, CS_INACTIVE);
						}
					}

					for (auto& child : children)
					{
						if (!child) continue;

						child->draw_border(dc);
					}

					break;
				}
			}
		}

		//
		// キャプションを描画します。
		//
		void draw_caption(HDC dc)
		{
			// カレントシャトルが存在するなら
			if (auto shuttle = get_current_shuttle())
			{
				// キャプションを描画します。

				auto rc = get_caption_rect();

				if (rc.bottom > position.bottom)
					return; // キャプションが描画範囲からはみ出てしまう場合は何もしません。

				auto rc_menu = get_menu_rect();
				auto has_menu = !!::GetMenu(*shuttle);
				auto has_focus = ::GetFocus() == *shuttle;

				// シャトルのウィンドウテキストを取得します。
				auto text = my::get_window_text(*shuttle);

				constexpr auto menu_icon = L"\xFE19";

				// テーマを使用するなら
				if (hive.use_theme)
				{
					// ウィンドウの状態からstate_idを取得します。
					auto state_id = CS_ACTIVE;
					if (!has_focus) state_id = CS_INACTIVE;
					if (!::IsWindowEnabled(*shuttle)) state_id = CS_DISABLED;

					// テーマAPIを使用してタイトルを描画します。
					::DrawThemeBackground(hive.theme.get(), dc, WP_CAPTION, state_id, &rc, nullptr);

					{
						auto rc_text = rc;

						if (has_menu)
						{
							rc_text.right = rc_menu.left;

							::DrawThemeText(hive.theme.get(), dc, WP_CAPTION, state_id,
								menu_icon, -1, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rc_menu);
						}

						if (rc_text.left < rc_text.right)
							::DrawThemeText(hive.theme.get(), dc, WP_CAPTION, state_id,
								text.c_str(), text.length(), DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &rc_text);
					}
				}
				// テーマを使用しないなら
				else
				{
					// 直接GDIを使用して描画します。

					auto caption_color = hive.active_caption_color;
					auto caption_text_color = hive.active_caption_text_color;

					if (!has_focus)
					{
						caption_color = hive.inactive_caption_color;
						caption_text_color = hive.inactive_caption_text_color;
					}

					{
						my::gdi::unique_ptr<HBRUSH> brush(
							::CreateSolidBrush(caption_color));

						::FillRect(dc, &rc, brush.get());
					}

					auto bk_mode = ::SetBkMode(dc, TRANSPARENT);
					auto text_color = ::SetTextColor(dc, caption_text_color);
					{
						auto rc_text = rc;

						if (has_menu)
						{
							rc_text.right = rc_menu.left;

							::DrawTextW(dc, menu_icon, -1, &rc_menu, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
						}

						if (rc_text.left < rc_text.right)
							::DrawTextW(dc, text.c_str(), text.length(), &rc_text, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
					::SetTextColor(dc, text_color);
					::SetBkMode(dc, bk_mode);
				}

				return;
			}

			// 子ペインも再帰的に処理させます。

			switch (split_mode)
			{
			case SplitMode::c_vert:
			case SplitMode::c_horz:
				{
					for (auto& child : children)
					{
						if (!child) continue;

						child->draw_caption(dc);
					}

					break;
				}
			}
		}

		//
		// このペインを再描画します。
		//
		void invalidate()
		{
			::InvalidateRect(owner, &position, FALSE);
		}

		//
		// ドッキングサイトからシャトルを切り離すときに呼ばれます。
		//
		virtual void release_shuttle(Shuttle* shuttle) override
		{
			// このペインからシャトルを切り離します。
			erase_shuttle(shuttle);

			// レイアウトを更新します。
			update_origin();
		}

		//
		// ターゲットのテキストが変更されたとき、シャトルから通知されます。
		//
		virtual void on_change_text(Shuttle* shuttle, LPCTSTR new_text) override
		{
			// タブのテキストを変更します。
			set_tab_text(shuttle, new_text);

			// ペインのタイトル部分を再描画します。
			invalidate();
		}

		//
		// ターゲットのフォーカスが変更されたとき、シャトルから通知されます。
		//
		virtual void on_change_focus(Shuttle* shuttle) override
		{
			// ペインのタイトル部分を再描画します。
			invalidate();
		}
	};
}
