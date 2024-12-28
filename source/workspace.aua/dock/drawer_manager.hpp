#pragma once

namespace apn::workspace
{
	//
	// このクラスはドロワーを管理します。
	//
	inline thread_local struct DrawerManager
	{
		//
		// このクラスはノードです。
		//
		struct Node
		{
			//
			// 展開中のドロワーです。
			//
			std::shared_ptr<Drawer> drawer;

			//
			// 表示中のシャトルです。
			//
			std::shared_ptr<Shuttle> shuttle;
		};

		//
		// ノードのコレクションです。
		//
		std::vector<Node> nodes;

		//
		// 指定された位置以降のノードを削除します。
		//
		void erase_nodes(auto erase_it)
		{
			for (auto it = erase_it; it != nodes.end(); it++)
			{
				// 指定された位置以降のドロワーの選択項目を無効にします。
				it->drawer->selected_node_index = -1;
				my::invalidate(*it->drawer);

				// 指定された位置以降のシャトルを非表示にします。
				hide_shuttle(it->shuttle);
			}

			// 指定された位置以降をコレクションから削除します。
			nodes.erase(erase_it, nodes.end());
		}

		//
		// 指定された位置がノード内の場合はTRUEを返します。
		//
		BOOL pt_in_node(const Node& node, const POINT& point)
		{
			// シャトルのウィンドウ矩形とヒットテストを行います。
			auto window_rc = my::get_window_rect(*node.shuttle->float_container);
			if (::PtInRect(&window_rc, point)) return TRUE;

			// ドロワーのウィンドウ矩形とヒットテストを行います。
//			auto drawer_rc = my::get_window_rect(*drawer);
			auto drawer_rc = node.drawer->get_facet()->get_expand_rect(node.drawer.get());
			my::map_window_points(::GetParent(*node.drawer), nullptr, &drawer_rc);
			if (::PtInRect(&drawer_rc, point)) return TRUE;

			return FALSE;
		}

		//
		// 指定された矩形がモニタ領域に収まるようにクランプして返します。
		//
		inline static RECT clamp_rect(HWND hwnd, LPCRECT base_rc)
		{
			auto rc = *base_rc;

			auto offset = POINT {};
			auto monitor_rc = my::get_monitor_rect(hwnd);
			if (rc.right > monitor_rc.right) offset.x = monitor_rc.right - rc.right;
			if (rc.bottom > monitor_rc.bottom) offset.y = monitor_rc.bottom - rc.bottom;
			if (rc.left < monitor_rc.left) offset.x = monitor_rc.left - rc.left;
			if (rc.top < monitor_rc.top) offset.y = monitor_rc.top - rc.top;

			::OffsetRect(&rc, offset.x, offset.y);

			return rc;
		}

		//
		// シャトルを表示します。
		// 表示位置はドロワーの設定と項目を基準にします。
		//
		inline static BOOL show_shuttle(
			const std::shared_ptr<Shuttle>& shuttle,
			const std::shared_ptr<Drawer>& drawer,
			const std::shared_ptr<Tav::Node>& tav_node)
		{
			// フローティングウィンドウを取得します。
			auto hwnd = (HWND)*shuttle->float_container;

			// フローティングウィンドウのウィンドウ矩形を取得します。
			auto window_rc = my::get_window_rect(hwnd);

			// フローティングウィンドウを表示する位置を取得します。
			auto rc = drawer->get_floating_window_rect(&window_rc, tav_node);

			// 表示位置を正規化します。
			rc = clamp_rect(*drawer, &rc);

			auto x = rc.left;
			auto y = rc.top;
			auto w = my::get_width(rc);
			auto h = my::get_height(rc);

			// フローティングウィンドウの位置を変更します。
			::SetWindowPos(hwnd, HWND_TOP, x, y, w, h, 0);

			// フローティングウィンドウを表示します。
			::ShowWindow(hwnd, SW_SHOW);

			return TRUE;
		}

		//
		// シャトルを非表示にします。
		//
		inline static BOOL hide_shuttle(const auto& shuttle)
		{
			return ::ShowWindow(*shuttle, SW_HIDE);
		}

		//
		// ドロワーの項目からシャトルを取得して返します。
		//
		inline static std::shared_ptr<Shuttle> get_shuttle(const std::shared_ptr<Tav::Node>& tav_node)
		{
			if (auto shuttle = Shuttle::get_pointer(tav_node->hwnd))
				return shuttle->shared_from_this();
			else
				return nullptr;
		}

		//
		// 指定されたドロワーとシャトルが
		// 現在のノードコレクションと関連がある場合はTRUEを返します。
		//
		BOOL is_relative(const std::shared_ptr<Drawer>& drawer, const std::shared_ptr<Shuttle>& shuttle)
		{
			// 新しいドロワーが所属するドックサイトを取得します。
			auto dock_site = ::GetParent(*drawer);

			// ノードを走査します。
			for (const auto& node : nodes)
			{
				// 同じドロワーの場合は
				if (node.drawer == drawer)
					return TRUE; // 関連があります。

				// 表示中のシャトルがドックサイトの場合は
				if (*node.shuttle == dock_site)
					return TRUE; // 関連があります。
			}

			return FALSE; // 関連がありません。
		}

		//
		// コレクション内のドロワーを検索して、そのイテレータを返します。
		//
		auto find_drawer(const std::shared_ptr<Drawer>& drawer)
		{
			return std::find_if(nodes.begin(), nodes.end(),
				[&](const auto& node)
			{
				return node.drawer == drawer;
			});
		}

		//
		// 選択項目のシャトルを表示します。
		//
		BOOL show_selected_shuttle(const std::shared_ptr<Drawer>& drawer)
		{
			// 選択項目を取得します。
			auto tav_node = drawer->get_selected_node();
			if (!tav_node) return FALSE;

			// 選択項目のシャトルを取得します。
			auto shuttle = get_shuttle(tav_node);
			if (!shuttle) return FALSE;

			// シャトルがドッキング状態の場合は何もしません。
			if (shuttle->is_docking()) return FALSE;

			// ドロワーリスト(ノードのコレクション)に存在しない
			// 全く新しいドロワーのシャトルを表示する場合は
			if (!is_relative(drawer, shuttle))
			{
				// 現在表示中のすべてのシャトルを非表示にします。
				erase_nodes(nodes.begin());
			}

			// ドロワーの位置を取得します。
			auto drawer_it = find_drawer(drawer);

			// ドロワーの位置が有効の場合は
			if (drawer_it != nodes.end())
			{
				// 前回選択されていたシャトルを非表示にします。
				hide_shuttle(drawer_it->shuttle);

				// 選択項目のシャトルを更新します。
				drawer_it->shuttle = shuttle;

				// ドロワーの位置以降のノードを削除します。
				erase_nodes(drawer_it + 1);
			}
			// ドロワーの位置が無効の場合は
			else
			{
				// ドロワーとシャトルをコレクションに追加します。
				nodes.emplace_back(drawer, shuttle);
			}

			// シャトルを表示します。
			show_shuttle(shuttle, drawer, tav_node);

			return TRUE;
		}

		//
		// ドロワーの選択項目が切り替わったときの処理です。
		//
		BOOL on_drawer_sel_change(const std::shared_ptr<Drawer>& drawer)
		{
			MY_TRACE_FUNC("");

			// シャトルを表示できなかった場合は
			if (!show_selected_shuttle(drawer))
			{
				// ドロワーの位置を取得します。
				auto drawer_it = find_drawer(drawer);
				if (drawer_it != nodes.end())
				{
					// 前回選択されていたシャトルを非表示にします。
					hide_shuttle(drawer_it->shuttle);

					// ドロワーとそれ以降のノードを削除します。
					erase_nodes(drawer_it);
				}
			}

			return TRUE;
		}

		//
		// フローティングコンテナが非表示になったときの処理です。
		//
		BOOL on_hide_floating_container(HWND hwnd)
		{
			// 削除対象のノードを検索します。
			auto erase_it = std::find_if(nodes.rbegin(), nodes.rend(),
				[hwnd](const auto& node) { return *node.shuttle->float_container == hwnd; });

			// 削除対象のノードが有効の場合は
			if (erase_it != nodes.rend())
			{
				// ノードを削除します。
				erase_nodes(erase_it.base() - 1);
			}

			return TRUE;
		}
	} drawer_manager;
}
