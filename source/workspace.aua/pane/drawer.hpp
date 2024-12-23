#pragma once

namespace apn::workspace
{
	//
	// このクラスはドロワーコントロールです。
	// 項目選択時にドッキングウィンドウを表示するタブコントロールとは違い、
	// このコントロールは項目選択時にフローティングウィンドウを表示するために使用されます。
	//
	struct Drawer : Tav
	{
		//
		// TRUEの場合はフローティングウィンドウを内側に表示します。
		//
		BOOL inside = TRUE;

		//
		// このクラスはファセットのインターフェイスです。
		//
		struct DrawerFacet {
			virtual RECT get_floating_window_rect(Drawer* drawer, LPCRECT window_rc, const std::shared_ptr<Node>& node) = 0;
		};

		//
		// このクラスはファセットのテンプレートです。
		// コメントはタブコントロールを上側に配置する場合を前提にして記述しています。
		//
		template <LONG RECT::* left, LONG RECT::* top, LONG RECT::* right, LONG RECT::* bottom>
		struct DrawerFacetT : DrawerFacet {
			//
			// フローティングウィンドウの表示位置を返します。
			//
			virtual RECT get_floating_window_rect(Drawer* drawer, LPCRECT window_rc, const std::shared_ptr<Node>& node) override
			{
				// ドロワーのウィンドウ矩形を取得します。
//				auto drawer_rc = my::get_window_rect(*drawer);
				auto drawer_rc = drawer->get_facet()->get_expand_rect(drawer);
				my::map_window_points(::GetParent(*drawer), nullptr, &drawer_rc);

				// 項目の矩形を取得します。
				auto node_rc = drawer->get_node_rect(&drawer_rc, node);

				// 基準となるウィンドウ矩形を取得します。
				auto rc = *window_rc;
				auto w = rc.*right - rc.*left;
				auto h = rc.*bottom - rc.*top;

				if (drawer->inside)
				{
					// フローティングウィンドウをドロワーの下側に表示します。
					rc.*top = drawer_rc.*bottom;
					rc.*bottom = rc.*top + h;
				}
				else
				{
					// フローティングウィンドウをドロワーの上側に表示します。
					rc.*bottom = drawer_rc.*top;
					rc.*top = rc.*bottom - h;
				}

				// フローティングウィンドウの位置を項目の位置と中央揃えにします。
				rc.*left = (node_rc.*left + node_rc.*right - w) / 2;
				rc.*right = rc.*left + w;

				// 計算結果を返します。
				return rc;
			}
		};

		//
		// このクラスは左側のファセットです。
		//
		inline static struct LeftDrawerFacet : DrawerFacetT<&RECT::top, &RECT::left, &RECT::bottom, &RECT::right> {
		} left_facet;

		//
		// このクラスは上側のファセットです。
		//
		inline static struct TopDrawerFacet : DrawerFacetT<&RECT::left, &RECT::top, &RECT::right, &RECT::bottom> {
		} top_facet;

		//
		// このクラスは右側のファセットです。
		//
		inline static struct RightDrawerFacet : DrawerFacetT<&RECT::top, &RECT::right, &RECT::bottom, &RECT::left> {
		} right_facet;

		//
		// このクラスは下側のファセットです。
		//
		inline static struct BottomDrawerFacet : DrawerFacetT<&RECT::left, &RECT::bottom, &RECT::right, &RECT::top> {
		} bottom_facet;

		//
		// コンストラクタです。
		//
		Drawer()
		{
			toggle_select = TRUE;
		}

		//
		// ドロワーファセットを返します。
		//
		DrawerFacet* get_drawer_facet()
		{
			switch (get_location())
			{
			case c_location.c_left: return &left_facet;
			default:
			case c_location.c_top: return &top_facet;
			case c_location.c_right: return &right_facet;
			case c_location.c_bottom: return &bottom_facet;
			}
		}

		//
		// タブコントロールが有効の場合はTRUEを返します。
		//
		virtual BOOL is_enabled() override
		{
			// 項目数に関わらず、必ずタブコントロールを表示します。
			return TRUE;
		}

		//
		// フローティングウィンドウの表示位置を返します。
		//
		RECT get_floating_window_rect(LPCRECT window_rc, const std::shared_ptr<Node>& node)
		{
			return get_drawer_facet()->get_floating_window_rect(this, window_rc, node);
		}
	};
}
