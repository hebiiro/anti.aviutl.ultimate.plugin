#pragma once

namespace apn::workspace
{
	struct Pane : Hive::Pane, Shuttle::Listener, std::enable_shared_from_this<Pane>
	{
		inline static constexpr auto c_prop_name = _T("apn::workspace::pane");

		inline static constexpr struct UpdateFlag {
			inline static constexpr uint32_t c_deep = 0x00000001;
			inline static constexpr uint32_t c_invalidate = 0x00000002;
			inline static constexpr uint32_t c_set_focus = 0x00000004;
			inline static constexpr uint32_t c_normalize = 0x00000008;
			inline static constexpr uint32_t c_show_current_shuttle = 0x00000010;
			inline static constexpr uint32_t c_show_tab = 0x00000020;
			inline static constexpr uint32_t c_show_drawers = 0x00000040;
			inline static constexpr uint32_t c_origin = 0x00000080;
			inline static constexpr uint32_t c_default =
				c_deep | c_invalidate | c_set_focus | c_normalize |
				c_show_current_shuttle | c_show_tab | c_show_drawers | c_origin;
		} c_update_flag;

		inline static constexpr struct DrawerIndex {
			inline static constexpr size_t c_none = -1;
			inline static constexpr size_t c_top = 0;
			inline static constexpr size_t c_bottom = 1;
			inline static constexpr size_t c_left = 2;
			inline static constexpr size_t c_right = 3;
		} c_drawer_index;

		//
		// このペインを保有するウィンドウです。
		//
		HWND owner = nullptr;

		//
		// このペインの表示位置です。
		//
		RECT position = {};

		//
		// 分割モードです。
		//
		int32_t split_mode = c_split_mode.c_none;

		//
		// セパレータの原点です。
		//
		int32_t origin = c_origin.c_bottom_right;

		//
		// キャプションのモードです。
		//
		int32_t caption_mode = c_caption_mode.c_show;

		//
		// キャプションの位置モードです。
		//
		int32_t caption_location = c_caption_location.c_top;

		//
		// TRUEの場合はセパレータをロックします。
		//
		BOOL is_border_locked = FALSE;

		//
		// セパレータの位置です。
		//
		int32_t border = 0;

		//
		// ドラッグ処理に使用します。
		//
		int32_t drag_offset = 0;

		//
		// タブコントロールです。
		//
		Tav tav;

		//
		// ドロワーコントロールです。
		//
		std::shared_ptr<Drawer> drawers[4];

		//
		// 子ペインです。
		//
		std::shared_ptr<Pane> children[2];

		//
		// このクラスはファセットのインターフェイスです。
		//
		struct CaptionFacet {
			virtual RECT get_exclude_rect(Pane* pane, LPCRECT base_rc) = 0;
			virtual RECT get_caption_rect(Pane* pane) = 0;
			virtual RECT get_caption_text_rect(Pane* pane, LPCRECT caption_rc) = 0;
			virtual RECT get_menu_rect(Pane* pane, LPCRECT caption_rc) = 0;
		};

		//
		// このクラスはファセットのテンプレートです。
		// コメントはキャプションを上側に配置する場合を前提にして記述しています。
		//
		template <LONG RECT::* left, LONG RECT::* top, LONG RECT::* right, LONG RECT::* bottom>
		struct CaptionFacetT : CaptionFacet {
			//
			// 指定されたメンバ変数のオフセットを返します。
			//
			inline static constexpr size_t offset(LONG RECT::* m)
			{
				return (size_t)&(((RECT*)0)->*m);
			}

			//
			// 水平方向の符号を返します。
			//
			inline static constexpr LONG horz_sign()
			{
				return (offset(left) < offset(right)) ? +1 : -1;
			}

			//
			// 垂直方向の符号を返します。
			//
			inline static constexpr LONG vert_sign()
			{
				return (offset(top) < offset(bottom)) ? +1 : -1;
			}

			//
			// 指定された領域からキャプションの領域を取り除いた領域を返します。
			//
			virtual RECT get_exclude_rect(Pane* pane, LPCRECT base_rc) override
			{
				// ワークエリア領域の上辺部分を除外して返します。
				auto rc = *base_rc;
				rc.*top += hive.caption_height * vert_sign();
				return rc;
			}

			//
			// キャプション領域を返します。
			//
			virtual RECT get_caption_rect(Pane* pane) override
			{
				// ペイン領域からドロワー領域を除外した領域を基準にします。
				auto caption_rc = pane->exclude_drawers(&pane->position);

				// キャプションとタブコントロールが同じ辺に存在する場合は
				if (pane->is_tab_same_location(pane->tav))
				{
					// タブコントロールが全自動モードではなく、
					// なおかつタブコントロールが外側に展開する場合は
					if (pane->tav.get_display_mode() != pane->tav.c_display_mode.c_full_auto &&
						pane->tav.get_stretch_mode() == pane->tav.c_stretch_mode.c_outside)
					{
						// キャプションを内側、タブコントロールを外側に配置します。
						// そのため、キャプション領域をタブ項目一行分下にずらします。
						caption_rc.*top += hive.tab_height * vert_sign();
					}
				}

				// キャプションの高さをセットします。
				caption_rc.*bottom = caption_rc.*top + hive.caption_height * vert_sign();

				// 計算結果を返します。
				return caption_rc;
			}

			//
			// キャプションの文字領域を返します。
			//
			virtual RECT get_caption_text_rect(Pane* pane, LPCRECT caption_rc) override
			{
				// キャプション領域からメニュー領域を除外します。
				auto text_rc = *caption_rc;
				text_rc.*right -= hive.caption_height * horz_sign();
				return text_rc;
			}

			//
			// メニュー領域を返します。
			//
			virtual RECT get_menu_rect(Pane* pane, LPCRECT caption_rc) override
			{
				// キャプションの右端を返します。
				auto menu_rc = *caption_rc;
				menu_rc.*left = menu_rc.*right - hive.caption_height * horz_sign();
				return menu_rc;
			}
		};

		//
		// このクラスは左側のファセットです。
		//
		inline static struct LeftCaptionFacet : CaptionFacetT<&RECT::top, &RECT::left, &RECT::bottom, &RECT::right> {
		} left_caption_facet;

		//
		// このクラスは上側のファセットです。
		//
		inline static struct TopCaptionFacet : CaptionFacetT<&RECT::left, &RECT::top, &RECT::right, &RECT::bottom> {
		} top_caption_facet;

		//
		// このクラスは右側のファセットです。
		//
		inline static struct RightCaptionFacet : CaptionFacetT<&RECT::top, &RECT::right, &RECT::bottom, &RECT::left> {
		} right_caption_facet;

		//
		// このクラスは下側のファセットです。
		//
		inline static struct BottomCaptionFacet : CaptionFacetT<&RECT::left, &RECT::bottom, &RECT::right, &RECT::top> {
		} bottom_caption_facet;

		//
		// ファセットを返します。
		//
		CaptionFacet* get_caption_facet()
		{
			// キャプションを表示する場合は
			if (caption_mode == c_caption_mode.c_show)
			{
				// 各辺のファセットを返します。
				switch (caption_location)
				{
				case c_caption_location.c_left: return &left_caption_facet;
				case c_caption_location.c_top: return &top_caption_facet;
				case c_caption_location.c_right: return &right_caption_facet;
				case c_caption_location.c_bottom: return &bottom_caption_facet;
				}
			}

			// それ以外の場合はnullptrを返します。
			return nullptr;
		}

		//
		// コンストラクタです。
		// ownerはこのペインを保有するウィンドウ(DockSiteの派生クラス)です。
		//
		Pane(HWND owner)
			: owner(owner)
		{
			// タブコントロールを初期化します。
			tav.init(owner);

			// このペインをタブに関連付けます。
			set_pane(tav, this);
		}

		//
		// デストラクタです。
		//
		virtual ~Pane()
		{
			// ドロワーを走査します。
			for (auto& drawer : drawers)
			{
				// ドロワーが有効の場合は
				if (drawer)
				{
					// このペインとドロワーの関連付けを解除します。
					set_pane(*drawer, nullptr);

					// ドロワーコントロールを終了します。
					drawer->exit();
				}
			}

			// このペインとタブの関連付けを解除します。
			set_pane(tav, nullptr);

			// タブコントロールを終了します。
			tav.exit();
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
		inline static void set_pane(HWND hwnd, Pane* pane)
		{
			::SetProp(hwnd, c_prop_name, pane);
		}

		//
		// タブの数を返します。
		//
		int32_t get_tab_count()
		{
			return (int32_t)tav.nodes.size();
		}

		//
		// 指定されたインデックスにあるタブのシャトルを返します。
		//
		Shuttle* get_shuttle(int32_t index)
		{
			if (index < 0 || index >= get_tab_count())
				return nullptr;

			return Shuttle::get_pointer(tav.nodes[index]->hwnd);
		}

		//
		// タブのカレントインデックスを返します。
		//
		int32_t get_current_index()
		{
			return tav.selected_node_index;
		}

		//
		// タブのカレントインデックスを指定されたインデックスに変更します。
		//
		void set_current_index(int32_t index)
		{
			if (index < 0 || index >= get_tab_count())
				index = 0;

			tav.selected_node_index = index;
		}

		//
		// 指定された座標にあるタブのインデックスを返します。
		// pointはオーナーの座標系で指定します。
		//
		int32_t hittest(POINT point)
		{
			// オーナーの座標系からタブの座標系に変換します。
			my::map_window_points(owner, tav, &point);

			return tav.hittest(point);
		}

		//
		// 指定されたインデックスの位置にタブを追加します。
		// shuttleはタブに関連付けるシャトルです。
		// textはタブ名です。
		//
		int32_t add_tab(Shuttle* shuttle, LPCTSTR text, int32_t index)
		{
			return tav.insert_node(shuttle->hwnd, text, index);
		}

		//
		// 指定されたインデックスのタブを削除します。
		//
		BOOL erase_tab(int32_t index)
		{
			return tav.erase_node(index);
		}

		//
		// すべてのタブを削除します。
		//
		void clear_all_tabs()
		{
			tav.clear();
		}

		//
		// 指定されたシャトルを持つタブのインデックスを返します。
		// 存在しない場合は-1を返します。
		//
		int32_t find_tab(Shuttle* shuttle)
		{
			return tav.find_node(*shuttle);
		}

		//
		// fromで指定された位置にあるタブをtoで指定された位置へ移動します。
		// 失敗した場合は-1を返します。
		//
		int32_t move_tab(int32_t from, int32_t to)
		{
			return tav.move_node(from, to);
		}

		//
		// 指定されたシャトルを持つタブを指定されたタブ名に変更します。
		//
		void set_tab_text(Shuttle* shuttle, LPCTSTR text)
		{
			auto index = find_tab(shuttle);
			if (index == -1) return;
			tav.set_node_text(index, text);
		}

		//
		// タブがキャプションと同じ位置モードの場合はTRUEを返します。
		//
		BOOL is_tab_same_location(Tav& tav)
		{
			return get_caption_location() == tav.get_location();
		}

		//
		// ドロワーを作成して返します。
		//
		auto create_drawer(size_t drawer_index)
		{
			// ドロワーを作成します。
			auto& drawer = drawers[drawer_index] = std::make_unique<Drawer>();

			// ドロワーを初期化します。
			drawer->init(owner);

			// このペインをドロワーに関連付けます。
			set_pane(*drawer, this);

			// 作成したドロワーを返します。
			return drawer;
		}

		//
		// ドロワーを削除します。
		//
		BOOL erase_drawer(size_t drawer_index)
		{
			auto& drawer = drawers[drawer_index];
			if (!drawer) return FALSE;
			drawer = nullptr;
			return TRUE;
		}

		//
		// ドロワーのインデックスを返します。
		//
		size_t get_drawer_index(HWND hwnd)
		{
			for (const auto& drawer : drawers)
			{
				if (drawer && *drawer == hwnd)
					return &drawer - drawers;
			}

			return c_drawer_index.c_none;
		}

		//
		// カレントタブのシャトルを返します。
		//
		Shuttle* get_current_shuttle()
		{
			return get_shuttle(get_current_index());
		}

		//
		// 指定されたシャトルをこのペインにドッキングします。
		//
		void dock_shuttle(Shuttle* shuttle)
		{
//			set_pane(*shuttle, this);
			shuttle->add_listener(this);
			shuttle->dock_window(owner);
		}

		//
		// 指定されたシャトルをこのペインから切り離します。
		//
		void float_shuttle(Shuttle* shuttle)
		{
//			set_pane(*shuttle, nullptr);
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
			if (index == -1) index = get_tab_count();

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

			// タブ項目を走査します。
			auto c = get_tab_count();
			for (decltype(c) i = 0; i < c; i++)
			{
				// タブ項目からシャトルを取得できた場合は
				if (auto shuttle = get_shuttle(i))
				{
					// シャトルをフローティング状態にします。
					float_shuttle(shuttle);
				}
			}

			// すべてのタブを削除します。
			clear_all_tabs();
		}

		//
		// ドロワーにシャトルを追加します。
		//
		int32_t insert_shuttle(const std::shared_ptr<Drawer>& drawer, Shuttle* shuttle, int32_t index = -1)
		{
			MY_TRACE_FUNC("{:#010x}, {}", shuttle, index);

			// 指定されたシャトルはすでにドロワー内に存在するので何もしません。
			if (drawer->find_node(*shuttle) != -1) return -1;

			// シャトルのウィンドウテキストを取得します。
			auto text = my::get_window_text(*shuttle);

			// ドロワーにタブ項目を追加します。
			return drawer->insert_node(*shuttle, text.c_str(), index);
		}

		//
		// ドロワーからシャトルを削除します。
		//
		BOOL erase_shuttle(const std::shared_ptr<Drawer>& drawer, int32_t index)
		{
			MY_TRACE_FUNC("{}", index);

			// ドロワーからタブ項目を削除します。
			return drawer->erase_node(index);
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
			case c_split_mode.c_none:
				{
					reset_pane();

					break;
				}
			case c_split_mode.c_vert:
				{
					border = my::get_width(position) / 2;

					if (!children[0]) children[0] = std::make_shared<Pane>(owner);
					if (!children[1]) children[1] = std::make_shared<Pane>(owner);

					break;
				}
			case c_split_mode.c_horz:
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
			if (split_mode == c_split_mode.c_none) return;

			border = -hive.border_width - border;

			switch (split_mode)
			{
			case c_split_mode.c_horz: border += position.bottom - position.top; break;
			case c_split_mode.c_vert: border += position.right - position.left; break;
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
		// キャプション位置モードを返します。
		//
		int32_t get_caption_location()
		{
			if (caption_mode == c_caption_mode.c_show)
				return caption_location;
			else
				return c_caption_location.c_nowhere;
		}

		//
		// キャプション位置モードを変更します。
		//
		void set_caption_location(int32_t new_caption_location)
		{
			if (caption_location == new_caption_location) return;
			caption_location = new_caption_location;
		}

		//
		// 与えられた値をクランプして返します。
		//
		inline static int32_t clamp(int32_t x, int32_t min_value, int32_t max_value)
		{
			if (x < min_value) return min_value;
			if (x > max_value) return max_value;
			return x;
		}

		int32_t absolute_x(int32_t x)
		{
			switch (origin)
			{
			case c_origin.c_top_left: return position.left + x;
			case c_origin.c_bottom_right: return position.right - x - hive.border_width;
			}

			return 0;
		}

		int32_t absolute_y(int32_t y)
		{
			switch (origin)
			{
			case c_origin.c_top_left: return position.top + y;
			case c_origin.c_bottom_right: return position.bottom - y - hive.border_width;
			}

			return 0;
		}

		int32_t relative_x(int32_t x)
		{
			switch (origin)
			{
			case c_origin.c_top_left: return x - position.left;
			case c_origin.c_bottom_right: return position.right - x + hive.border_width;
			}

			return 0;
		}

		int32_t relative_y(int32_t y)
		{
			switch (origin)
			{
			case c_origin.c_top_left: return y - position.top;
			case c_origin.c_bottom_right: return position.bottom - y + hive.border_width;
			}

			return 0;
		}

		//
		// 指定された領域からタブ領域を除外した領域を返します。
		//
		RECT exclude_tab(LPCRECT base_rc)
		{
			// 指定された領域です。
			auto rc = *base_rc;

			// タブを配置する必要がある場合は
			if (tav.need_deploy())
			{
				// タブ領域を除外します。
				rc = tav.get_facet()->get_exclude_rect(&tav, &rc);
			}

			// 除外後の領域を返します。
			return rc;
		}

		//
		// 指定された領域からドロワー領域を除外した領域を返します。
		//
		RECT exclude_drawers(LPCRECT base_rc)
		{
			// 指定された領域です。
			auto rc = *base_rc;

			// ドロワーを走査します。
			for (const auto& drawer : drawers)
			{
				// ドロワーが有効の場合は
				if (drawer)
				{
					// ドロワーを配置する必要がある場合は
					if (drawer->need_deploy())
					{
						// ドロワー領域を除外します。
						rc = drawer->get_facet()->get_exclude_rect(drawer.get(), &rc);
					}
				}
			}

			// 除外後の領域を返します。
			return rc;
		}

		//
		// 指定された領域からキャプション領域を除外した領域を返します。
		//
		RECT exclude_caption(LPCRECT base_rc)
		{
			if (auto caption_facet = get_caption_facet())
			{
				// 指定された領域からキャプション領域を除外して返します。
				return caption_facet->get_exclude_rect(this, base_rc);
			}
			else
			{
				// 指定された領域をそのまま返します。
				return *base_rc;
			}
		}

		//
		// ドッキング領域を返します。
		//
		RECT get_dock_rect()
		{
			// ペイン領域を基準にします。
			auto rc = position;

			// ドロワー領域を除外します。
			rc = exclude_drawers(&rc);

			// キャプション領域を除外します。
			rc = exclude_caption(&rc);

			// タブ領域を除外します。
			rc = exclude_tab(&rc);

			// 残りの領域を返します。
			return rc;
		}

		//
		// ボーダーの位置をノーマライズします。
		//
		void normalize()
		{
//			MY_TRACE_FUNC("");

			switch (split_mode)
			{
			case c_split_mode.c_vert: border = clamp(border, 0, my::get_width(position) - hive.border_width); break;
			case c_split_mode.c_horz: border = clamp(border, 0, my::get_height(position) - hive.border_width); break;
			}
		}

		//
		// 指定されたシャトルを表示します。
		//
		void show_shuttle(my::DeferWindowPos& dwp, const auto& shuttle)
		{
#if 1
			// シャトルが格納されているドッキングコンテナを
			// タブコントロールのすぐ後ろに表示にします。
			dwp.set_window_pos(*shuttle->dock_container,
				tav, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
			// SWP_SHOWWINDOWを使用してウィンドウを表示した場合は
			// WM_SHOWWINDOWが送信されないので手動で送信しています。
			::PostMessage(*shuttle->dock_container, WM_SHOWWINDOW, TRUE, 0);
#else
			dwp.set_window_pos(*shuttle->dock_container,
				tav, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
			::ShowWindowAsync(*shuttle->dock_container, SW_SHOWNA);
#endif
		};

		//
		// 指定されたシャトルを非表示にします。
		//
		void hide_shuttle(my::DeferWindowPos& dwp, const auto& shuttle)
		{
			// シャトルが格納されているドッキングコンテナを非表示にします。
			dwp.set_window_pos(*shuttle->dock_container,
				nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
			// SWP_HIDEWINDOWを使用してウィンドウを非表示にした場合は
			// WM_SHOWWINDOWが送信されないので手動で送信しています。
			::PostMessage(*shuttle->dock_container, WM_SHOWWINDOW, FALSE, 0);
		};

		//
		// タブコントロールを非表示にします。
		//
		void hide_tab(my::DeferWindowPos& dwp)
		{
			// タブコントロールを非表示にします。
			dwp.set_window_pos(tav, nullptr, 0, 0, 0, 0,
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
		};

		//
		// ドロワーコントロールを非表示にします。
		//
		void hide_drawers(HDWP dwp)
		{
			for (const auto& drawer : drawers)
			{
				if (drawer)
				{
					// ドロワーコントロールを非表示にします。
					::DeferWindowPos(dwp, *drawer, nullptr, 0, 0, 0, 0,
						SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_HIDEWINDOW);
				}
			}
		};

		//
		// このペインを起点にして、
		// このペインおよび子孫ペインの位置情報を更新します。
		//
		void update_origin(uint32_t flags = c_update_flag.c_default)
		{
//			MY_TRACE_FUNC("{:#010x}", flags);

			update_origin(&position, flags);
		}

		//
		// このペインを起点にして、
		// このペインおよび子孫ペインの位置情報を更新します。
		//
		void update_origin(LPCRECT rc, uint32_t flags = c_update_flag.c_default)
		{
//			MY_TRACE_FUNC("{:#010x}", flags);

			my::DeferWindowPos dwp(100);

			update(dwp, rc, flags);
		}

		//
		// このペインおよび子孫ペインの位置情報を更新します。
		// rcはこのペインの新しい位置です。
		//
		virtual void update(my::DeferWindowPos& dwp, LPCRECT rc, uint32_t flags)
		{
//			MY_TRACE_FUNC("{:#010x}", flags);

			if (::IsIconic(owner))
				return;

			// このペインの位置を更新します。
			position = *rc;

			if (flags & c_update_flag.c_invalidate)
			{
				// ペイン矩形が変更されたので、
				// オーナーにペイン矩形の再描画処理を発注しておきます。
				invalidate();
			}

			// 基準領域です。
			auto base_rc = position;

			if (flags & c_update_flag.c_show_drawers)
			{
				// ドロワーを走査します。
				for (const auto& drawer : drawers)
				{
					// ドロワーが有効の場合は
					if (drawer)
					{
						// ドロワーを更新します。
						update_tab(dwp, &base_rc, *drawer, FALSE);

						// 基準領域からドロワー領域を除外していきます。
						base_rc = drawer->get_facet()->get_exclude_rect(drawer.get(), &base_rc);
					}
				}
			}
			else
			{
				// ドロワーコントロールを非表示にします。
				hide_drawers(dwp);
			}

			if (flags & c_update_flag.c_show_tab)
			{
				// タブコントロールを更新します。
				update_tab(dwp, &base_rc);
			}
			else
			{
				// タブコントロールを非表示にします。
				hide_tab(dwp);
			}

			// シャトルを持っている場合は
			if (get_tab_count())
			{
				if (flags & c_update_flag.c_show_current_shuttle)
				{
					// カレントシャトルのみを表示し、フォーカスを与えます。
					update_shuttles(dwp, flags);
				}

				return; // シャトルを持つペインはボーダーも子ペインも持たないので、ここで処理を終了します。
			}

			if (flags & c_update_flag.c_normalize)
			{
				// ボーダーが飛び出ないように調整します。
				normalize();
			}

			if (flags & c_update_flag.c_deep)
			{
				// 子ペインを再帰的に更新します。
				// すでに親ペインの再描画が発行されている場合は
				// c_invalidateフラグは冗長になるので除外します。
				update_children(dwp, flags & ~c_update_flag.c_invalidate);
			}
		}

		//
		// タブコントロールのウィンドウ位置を更新します。
		// この関数はupdate()から呼び出されます。
		//
		void update_tab(my::DeferWindowPos& dwp, LPCRECT base_rc)
		{
//			MY_TRACE_FUNC("");

			return update_tab(dwp, base_rc, tav, TRUE);
		}

		//
		// 指定されたタブコントロールのウィンドウ位置を更新します。
		//
		void update_tab(my::DeferWindowPos& dwp, LPCRECT base_rc, Tav& tav, BOOL integral)
		{
//			MY_TRACE_FUNC("");

			// タブコントロールに追加領域をリセットします。
			tav.addional_height = 0;

			// キャプションとタブコントロールが同じ辺に配置されている場合は
			if (integral && is_tab_same_location(tav))
			{
				// タブコントロールに追加領域を設定します。
				tav.addional_height = hive.caption_height;
			}

			// タブコントロールのレイアウトを更新します。
			tav.update(base_rc);

			// タブコントロールのレイアウトを取得します。
			auto layout = tav.get_layout();

			if (layout.show)
			{
				// タブコントロールを表示します。
				dwp.set_window_pos(tav, HWND_TOP,
					&layout.rc, SWP_NOACTIVATE | SWP_SHOWWINDOW);
			}
			else
			{
				// タブコントロールを非表示にします。
				dwp.set_window_pos(tav, nullptr,
					&layout.rc, SWP_NOZORDER | SWP_HIDEWINDOW);
			}
		}

		//
		// シャトル(が格納されているドッキングコンテナ)のウィンドウ位置を更新します。
		// この関数はupdate()から呼び出されます。
		//
		void update_shuttles(my::DeferWindowPos& dwp, uint32_t flags)
		{
//			MY_TRACE_FUNC("{:#010x}", flags);

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
				if (!shuttle) continue;

				// シャトルをリサイズします。
				shuttle->resize(&rc_dock);

				MY_TRACE("『{}』の表示状態を変更します\n", shuttle->name);

				MY_TRACE_INT(::IsWindowVisible(*shuttle));
				MY_TRACE_INT(::IsWindowVisible(*shuttle->dock_container));
				MY_TRACE_INT(::IsWindowVisible(::GetParent(*shuttle->dock_container)));

				if (i == current_index)
				{
					// 空の設定ダイアログの場合は
					if (*shuttle == hive.setting_dialog &&
						magi.exin.get_current_object_index() == -1)
					{
						MY_TRACE("空の設定ダイアログなので表示できません\n");

						hide_shuttle(dwp, shuttle);
					}
					else
					{
						MY_TRACE("「{}」を表示します\n", shuttle->name);

						show_shuttle(dwp, shuttle);
					}

					if (flags & c_update_flag.c_set_focus)
					{
						if (::IsWindowVisible(*shuttle))
							::SetFocus(*shuttle);
					}
				}
				else
				{
					MY_TRACE("「{}」を非表示にします\n", shuttle->name);

					hide_shuttle(dwp, shuttle);
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
		void update_children(my::DeferWindowPos& dwp, uint32_t flags)
		{
			switch (split_mode)
			{
			case c_split_mode.c_vert:
				{
					auto abs_border = absolute_x(border);

					if (children[0])
					{
						RECT rc = { position.left, position.top, abs_border, position.bottom };

						children[0]->update(dwp, &rc, flags);
					}

					if (children[1])
					{
						RECT rc = { abs_border + hive.border_width, position.top, position.right, position.bottom };

						children[1]->update(dwp, &rc, flags);
					}

					break;
				}
			case c_split_mode.c_horz:
				{
					auto abs_border = absolute_y(border);

					if (children[0])
					{
						RECT rc = { position.left, position.top, position.right, abs_border };

						children[0]->update(dwp, &rc, flags);
					}

					if (children[1])
					{
						RECT rc = { position.left, abs_border + hive.border_width, position.right, position.bottom };

						children[1]->update(dwp, &rc, flags);
					}

					break;
				}
			}
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
			case c_split_mode.c_vert:
			case c_split_mode.c_horz:
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
				case c_split_mode.c_vert:
					{
						auto abs_border = absolute_x(border);

						// pointがボーダーの範囲内なら
						if (point.x >= abs_border && point.x < abs_border + hive.border_width)
							return shared_from_this(); // ヒットしているのでこのペインを返します。

						break;
					}
				case c_split_mode.c_horz:
					{
						auto abs_border = absolute_y(border);

						// pointがボーダーの範囲内なら
						if (point.y >= abs_border && point.y < abs_border + hive.border_width)
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
			case c_split_mode.c_vert: return border - relative_x(point.x);
			case c_split_mode.c_horz: return border - relative_y(point.y);
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
			case c_split_mode.c_vert: border = relative_x(point.x) + drag_offset; break;
			case c_split_mode.c_horz: border = relative_y(point.y) + drag_offset; break;
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
			case c_split_mode.c_vert:
				{
					auto abs_border = absolute_x(border);

					rc->left = abs_border;
					rc->top = position.top;
					rc->right = abs_border + hive.border_width;
					rc->bottom = position.bottom;

					return TRUE;
				}
			case c_split_mode.c_horz:
				{
					auto abs_border = absolute_y(border);

					rc->left = position.left;
					rc->top = abs_border;
					rc->right = position.right;
					rc->bottom = abs_border + hive.border_width;

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
			case c_split_mode.c_vert:
			case c_split_mode.c_horz:
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
			// カレントシャトルが存在する場合は
			if (auto shuttle = get_current_shuttle())
			{
				switch (get_caption_location())
				{
				case c_caption_location.c_left:
				case c_caption_location.c_right:
					return draw_caption(dc, shuttle,
						[this](const RECT& rc, const RECT& base_rc) -> RECT {
							auto h = my::get_height(base_rc);
							return {
								rc.top,
								h - rc.right,
								rc.bottom,
								h - rc.left,
							};
						});
				case c_caption_location.c_top:
				case c_caption_location.c_bottom:
					return draw_caption(dc, shuttle,
						[this](const RECT& rc, const RECT& base_rc) -> RECT {
							return rc;
						});
				}

				return;
			}

			// 子ペインも再帰的に処理させます。
			switch (split_mode)
			{
			case c_split_mode.c_vert:
			case c_split_mode.c_horz:
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
		// キャプションを描画します。
		//
		void draw_caption(HDC paint_dc, Shuttle* shuttle, auto get_mem_rect)
		{
			// ファセットが取得できない場合はキャプションは描画されないので何もしません。
			auto caption_facet = get_caption_facet(); if (!caption_facet) return;

			// キャプション領域を取得します。
			auto caption_rc = caption_facet->get_caption_rect(this);
			if (caption_rc.top < position.top ||
				caption_rc.bottom > position.bottom)
			{
				return; // キャプション領域が描画範囲からはみ出てしまう場合は何もしません。
			}

			// キャプション領域から描画先領域を取得します。
			auto paint_rc = caption_rc;

			// キャプション領域から基準領域を取得します。
			auto base_rc = caption_rc; ::OffsetRect(&base_rc, -base_rc.left, -base_rc.top);

			// メモリDC用の領域を取得します。
			auto rc = get_mem_rect(base_rc, {}); ::OffsetRect(&rc, -rc.left, -rc.top);

			// メモリDCを作成します。
			my::MemDC dc(paint_dc, &rc);
			my::gdi::selector font_selector(dc, ::GetCurrentObject(paint_dc, OBJ_FONT));

			// 背景を塗りつぶします。
			painter.fill_background(dc, &rc);

			// メニューアイコン代わりの文字です。
			constexpr auto menu_icon = L"\xFE19";

			// キャプション領域を再取得します。
			caption_rc = rc;

			// メニュー領域を取得します。
			auto menu_rc = caption_facet->get_menu_rect(this, &base_rc);

			// メニュー領域をメモリDC用に変換します。
			menu_rc = get_mem_rect(menu_rc, rc);

			// シャトルのメニュー状態を取得します。
			auto has_menu = !!::GetMenu(*shuttle);

			// シャトルのフォーカス状態を取得します。
			auto has_focus = ::GetFocus() == *shuttle;

			// シャトルのウィンドウ名を取得します。
			auto text = my::get_window_text(*shuttle);

			// テーマを使用する場合はテーマAPIで描画します。
			if (hive.use_theme)
			{
				// ウィンドウの状態からstate_idを取得します。
				auto state_id = CS_ACTIVE;
				if (!has_focus) state_id = CS_INACTIVE;
				if (!::IsWindowEnabled(*shuttle)) state_id = CS_DISABLED;

				// キャプションの背景を描画します。
				::DrawThemeBackground(hive.theme.get(), dc, WP_CAPTION, state_id, &caption_rc, nullptr);

				// シャトルがメニューを持っている場合は
				if (has_menu)
				{
					// キャプションの文字領域を取得します。
					caption_rc = caption_facet->get_caption_text_rect(this, &base_rc);

					// キャプションの文字領域をメモリDC用に変換します。
					caption_rc = get_mem_rect(caption_rc, rc);

					// メニューアイコンを描画します。
					::DrawThemeText(hive.theme.get(), dc, WP_CAPTION, state_id,
						menu_icon, -1, DT_CENTER | DT_VCENTER | DT_SINGLELINE, 0, &menu_rc);
				}

				// 描画位置が有効の場合は
				if (caption_rc.top < caption_rc.bottom &&
					caption_rc.left < caption_rc.right)
				{
					// キャプションの文字を描画します。
					::DrawThemeText(hive.theme.get(), dc, WP_CAPTION, state_id,
						text.c_str(), -1, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS, 0, &caption_rc);
				}
			}
			// テーマを使用しない場合はGDIで描画します。
			else
			{
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

					// キャプションの背景を描画します。
					::FillRect(dc, &caption_rc, brush.get());
				}

				auto old_bk_mode = ::SetBkMode(dc, TRANSPARENT);
				auto old_text_color = ::SetTextColor(dc, caption_text_color);

				// シャトルがメニューを持っている場合は
				if (has_menu)
				{
					// キャプションの文字領域を取得します。
					caption_rc = caption_facet->get_caption_text_rect(this, &base_rc);

					// キャプションの文字領域をメモリDC用に変換します。
					caption_rc = get_mem_rect(caption_rc, rc);

					// メニューアイコンを描画します。
					::DrawTextW(dc, menu_icon, -1, &menu_rc,
						DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				}

				// 描画位置が有効の場合は
				if (caption_rc.top < caption_rc.bottom &&
					caption_rc.left < caption_rc.right)
				{
					// キャプションの文字を描画します。
					::DrawTextW(dc, text.c_str(), -1, &caption_rc,
						DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
				}

				::SetTextColor(dc, old_text_color);
				::SetBkMode(dc, old_bk_mode);
			}

			auto x = paint_rc.left;
			auto y = paint_rc.top;
			auto w = my::get_width(paint_rc);
			auto h = my::get_height(paint_rc);

			switch (caption_location)
			{
			case c_caption_location.c_left:
			case c_caption_location.c_right:
				{
					// PlgBltの罠 - 門前の小僧
					// https://motchy99.blog.fc2.com/blog-entry-60.html
					POINT points[3] = { { dc.h, 0 }, { dc.h, dc.w }, { 0, 0 } }; // 90度回転させます。
					for (auto& point : points) point.x += x, point.y += y;
					::PlgBlt(paint_dc, points, dc, 0, 0, dc.w, dc.h, nullptr, 0, 0);

					break;
				}
			case c_caption_location.c_top:
			case c_caption_location.c_bottom:
				{
					::BitBlt(paint_dc, x, y, w, h, dc, 0, 0, SRCCOPY);

					break;
				}
			}
		}

		//
		// このペインを再描画します。
		//
		void invalidate()
		{
			my::invalidate(owner, &position, FALSE);
		}

		//
		// グローバルなマウスムーブメッセージを処理します。
		//
		LRESULT on_global_mouse_move(HWND hwnd, const POINT& point)
		{
			if (tav == hwnd)
				return tav.on_global_mouse_move(point);

			for (const auto& drawer : drawers)
			{
				if (drawer && *drawer == hwnd)
					drawer->on_global_mouse_move(point);
			}

			return 0;
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
