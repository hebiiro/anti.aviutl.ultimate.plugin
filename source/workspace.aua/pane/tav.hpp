#pragma once

namespace apn::workspace
{
	//
	// このクラスは自作のタブコントロールです。
	//
	struct Tav : my::Window, Hive::Tav
	{
		//
		// ウィンドウクラス名です。
		//
		inline static constexpr auto c_class_name = _T("apn::workspace::pane::tab");

		//
		// ウィンドウメッセージです。
		//
		inline static constexpr struct Message {
			inline static constexpr uint32_t c_update = WM_APP + 1;
		} c_message;

		//
		// タブコントロールのインスタンスのコレクションです。
		// ウィンドウズフックからグローバルメッセージを受け取るために必要です。
		// (ここで言うグローバルメッセージとは
		// 他のウィンドウのものも含めたすべてのウィンドウメッセージのことです)
		//
		inline static thread_local std::set<Tav*> tavs;

		//
		// このクラスはタブ項目です。
		//
		struct Node {
			//
			// このノードに関連付けられているウィンドウハンドルです。
			//
			HWND hwnd = nullptr;

			//
			// このノードで描画する文字列です。
			//
			std::wstring text;

			//
			// このノードの描画に関する値です。
			//
			struct Display {
				//
				// このノードを描画する行です。
				//
				int32_t row = 0;

				//
				// このノードの描画位置です。
				//
				int32_t pos = 0;

				//
				// このノードの描画サイズです。
				//
				int32_t size = 0;
			} display;
		};

		//
		// このクラスは行データです。
		//
		struct Row {
			//
			// この行に存在する最初のノードのインデックスです。
			//
			int32_t node_index;

			//
			// この行に存在するノードの数です。
			//
			int32_t node_count;
		};

		//
		// このクラスはこのタブコントロールのレイアウトです。
		// このタブコントロールのオーナーが
		// このタブコントロールのウィンドウ位置を変更するために使用されます。
		//
		struct Layout { BOOL show; RECT rc; };

		//
		// ノードのコレクションです。
		//
		std::vector<std::shared_ptr<Node>> nodes;

		//
		// 各行先頭ノードのインデックスです。
		//
		std::vector<Row> rows;

		//
		// 収縮しているタブコントロールのウィンドウ位置です。
		// 座標はタブコントロールの親ウィンドウのクライアント座標になります。
		//
		RECT collapse_rc = {};

		//
		// 展開しているタブコントロールのウィンドウ位置です。
		// 座標はタブコントロールの親ウィンドウのクライアント座標になります。
		//
		RECT expand_rc = {};

		//
		// TRUEの場合はタブコントロールが展開しています。
		//
		BOOL expanded = FALSE;

		//
		// TRUEの場合は更新処理がポスト済みです。
		//
		BOOL update_posted = FALSE;

		//
		// TRUEの場合はトグル選択になります。
		// トグル選択の場合は選択項目をクリックすると
		// その項目は非選択状態になります。
		//
		BOOL toggle_select = FALSE;

		//
		// 選択ノードのインデックスです。
		//
		int32_t selected_node_index = -1;

		//
		// ホットノードのインデックスです。
		//
		int32_t hot_node_index = -1;

		//
		// 追加領域のサイズです。
		//
		int32_t addional_height = 0;

		//
		// このクラスはファセットのインターフェイスです。
		//
		struct Facet {
			virtual RECT get_exclude_rect(Tav* tav, LPCRECT workarea) = 0;
			virtual RECT get_collapse_rect(Tav* tav, LPCRECT workarea) = 0;
			virtual RECT get_expand_rect(Tav* tav) = 0;
			virtual RECT get_icon_rect(Tav* tav, LPCRECT rc) = 0;
			virtual RECT get_node_rect(Tav* tav, LPCRECT rc, const std::shared_ptr<Node>& node) = 0;
			virtual RECT get_expand_trigger_rect(Tav* tav) = 0;
			virtual RECT get_collapse_trigger_rect(Tav* tav) = 0;
		};

		//
		// このクラスはファセットのテンプレートです。
		// コメントはタブコントロールを上側に配置する場合を前提にして記述しています。
		//
		template <LONG RECT::* left, LONG RECT::* top, LONG RECT::* right, LONG RECT::* bottom>
		struct FacetT : Facet {
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
			// ワークエリア領域からタブコントロールの領域を取り除いた領域を返します。
			//
			virtual RECT get_exclude_rect(Tav* tav, LPCRECT workarea) override
			{
				// ワークエリア領域を基準にします。
				auto rc = *workarea;

				// 領域からワークエリア領域の上辺部分を除外します。
				rc.*top += hive.tab_height * vert_sign();

				// 計算結果を返します。
				return rc;
			}

			//
			// 収縮状態のタブコントロールが表示されるべき領域を返します。
			//
			virtual RECT get_collapse_rect(Tav* tav, LPCRECT workarea) override
			{
				// ワークエリア領域を基準にします。
				auto rc = *workarea;

				// タブコントロールを外側に展開する場合は
				if (tav->get_stretch_mode() >= c_stretch_mode.c_outside)
				{
					// 領域を追加領域のサイズ分だけ上にずらします。
					rc.*top -= tav->addional_height * vert_sign();
				}

				// 領域をワークエリア領域の上辺内側に限定します。
				rc.*bottom = rc.*top + hive.tab_height * vert_sign();

				// 計算結果を返します。
				return rc;
			}

			//
			// 展開状態のタブコントロールが表示されるべき領域を返します。
			//
			virtual RECT get_expand_rect(Tav* tav) override
			{
				// 収縮したウィンドウ領域を基準にします。
				auto expand_rc = tav->collapse_rc;

				// 展開したウィンドウの高さです。
				auto h = tav->rows.size() * hive.tab_height;

				// 内側に展開する場合は
				if (tav->get_stretch_mode() != c_stretch_mode.c_outside)
				{
					// 下辺を変更します。
					expand_rc.*bottom = expand_rc.*top + h * vert_sign();
				}
				// 外側に展開する場合は
				else
				{
					// 全自動モードの場合は
					if (tav->get_display_mode() == c_display_mode.c_full_auto)
					{
						// 下辺を変更します。
						expand_rc.*bottom = expand_rc.*top;
					}

					// 上辺を変更します。
					expand_rc.*top = expand_rc.*bottom - h * vert_sign();
				}

				// 展開したウィンドウ領域を返します。
				return expand_rc;
			}

			//
			// 指定された矩形を基準にしてアイコン領域を返します。
			//
			virtual RECT get_icon_rect(Tav* tav, LPCRECT rc) override
			{
				// ノードが存在しない場合はアイコン領域は取得できません。
				if (tav->nodes.empty()) return {};

				// 最初のノード領域を取得します。
				auto icon_rc = get_node_rect(tav, rc, tav->nodes.front());

				// 寄せが右または下ではない場合は
				if (tav->get_node_align() != c_node_align.c_far)
				{
					// アイコンをノード領域の左側に配置します。
					icon_rc.*right = icon_rc.*left;
					icon_rc.*left = icon_rc.*left - hive.tab_height * horz_sign();
				}
				else
				{
					// アイコンをノード領域の右側に配置します。
					icon_rc.*left = icon_rc.*right;
					icon_rc.*right = icon_rc.*right + hive.tab_height * horz_sign();
				}

				// アイコン領域を返します。
				return icon_rc;
			}

			//
			// 指定された領域を基準にして指定されたノード領域を返します。
			//
			virtual RECT get_node_rect(Tav* tav, LPCRECT rc, const std::shared_ptr<Node>& node) override
			{
				// ノード領域です。
				auto node_rc = RECT {};

				// 基準領域の左辺からX座標を求めます。
				node_rc.*left = rc->*left + node->display.pos;
				node_rc.*right = node_rc.*left + node->display.size;

				// 外側に展開する場合は
				if (tav->get_stretch_mode() == c_stretch_mode.c_outside)
				{
					// 基準領域の下辺からY座標を求めます。
					node_rc.*bottom = rc->*bottom - node->display.row * hive.tab_height * vert_sign();
					node_rc.*top = node_rc.*bottom - hive.tab_height * vert_sign();
				}
				// 内側に展開する場合は
				else
				{
					// 基準領域の上辺からY座標を求めます。
					node_rc.*top = rc->*top + node->display.row * hive.tab_height * vert_sign();
					node_rc.*bottom = node_rc.*top + hive.tab_height * vert_sign();
				}

				// ノード領域を返します。
				return node_rc;
			}

			//
			// 展開を発動する領域を返します。
			//
			virtual RECT get_expand_trigger_rect(Tav* tav)
			{
				// 収縮したウィンドウ領域を基準にします。
				auto trigger_rc = tav->collapse_rc;

				// 追加領域が存在する場合は
				if (tav->addional_height)
				{
					switch (tav->get_display_mode())
					{
					// 全自動モードの場合は
					case c_display_mode.c_full_auto:
						{
							// トリガー領域の下部をタブ項目一行分縮小します。
							trigger_rc.*bottom -= hive.tab_height * vert_sign();
						}
					// 全自動・自動モードの場合は
					case c_display_mode.c_auto:
						{
							// 外側に展開する場合は
							if (tav->get_stretch_mode() == c_stretch_mode.c_outside)
							{
								// 追加領域をトリガー領域下部に加えます。
								trigger_rc.*bottom += tav->addional_height * vert_sign();
							}
							// 内側に展開する場合は
							else
							{
								// 追加領域をトリガー領域上部に加えます。
								trigger_rc.*top -= tav->addional_height * vert_sign();
							}

							break;
						}
					}
				}

				// 半自動モードの場合は
				if (tav->get_display_mode() == c_display_mode.c_semi_auto)
				{
					// トリガー領域をアイコン領域に限定します。
					trigger_rc = get_icon_rect(tav, &trigger_rc);
				}

				// 展開のトリガーとなる領域を返します。
				return trigger_rc;
			}

			//
			// 収縮を発動しない領域を返します。
			//
			virtual RECT get_collapse_trigger_rect(Tav* tav)
			{
				// 展開したウィンドウ領域を基準にします。
				auto trigger_rc = tav->expand_rc;

				// 追加領域が存在する場合は
				if (tav->addional_height)
				{
					switch (tav->get_display_mode())
					{
					// 全自動・自動モードの場合は
					case c_display_mode.c_full_auto:
					case c_display_mode.c_auto:
						{
							// 外側に展開する場合は
							if (tav->get_stretch_mode() == c_stretch_mode.c_outside)
							{
								// 追加領域をトリガー領域下部に加えます。
								trigger_rc.*bottom += tav->addional_height * vert_sign();
							}
							// 内側に展開する場合は
							else
							{
								// 追加領域をトリガー領域上部に加えます。
								trigger_rc.*top -= tav->addional_height * vert_sign();
							}

							break;
						}
					}
				}
				else
				{
					// 全自動モードで外側に展開する場合は
					if (tav->get_display_mode() == c_display_mode.c_full_auto &&
						tav->get_stretch_mode() == c_stretch_mode.c_outside)
					{
						// トリガー領域の下部をタブ項目一行分拡張します。
						trigger_rc.*bottom += hive.tab_height * vert_sign();
					}
				}

				// 収縮のトリガーとなる領域を返します。
				return trigger_rc;
			}
		};

		//
		// このクラスは左側のファセットです。
		//
		inline static struct LeftFacet : FacetT<&RECT::top, &RECT::left, &RECT::bottom, &RECT::right> {
		} left_facet;

		//
		// このクラスは上側のファセットです。
		//
		inline static struct TopFacet : FacetT<&RECT::left, &RECT::top, &RECT::right, &RECT::bottom> {
		} top_facet;

		//
		// このクラスは右側のファセットです。
		//
		inline static struct RightFacet : FacetT<&RECT::top, &RECT::right, &RECT::bottom, &RECT::left> {
		} right_facet;

		//
		// このクラスは下側のファセットです。
		//
		inline static struct BottomFacet : FacetT<&RECT::left, &RECT::bottom, &RECT::right, &RECT::top> {
		} bottom_facet;

		//
		// ファセットを返します。
		//
		Facet* get_facet()
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
		// 表示モードを返します。
		//
		int32_t get_display_mode()
		{
			return (display_mode != c_display_mode.c_default) ? display_mode : hive.tav.display_mode;
		}

		//
		// 選択モードを返します。
		//
		int32_t get_select_mode()
		{
			return (select_mode != c_select_mode.c_default) ? select_mode : hive.tav.select_mode;
		}

		//
		// 伸展モードを返します。
		//
		int32_t get_stretch_mode()
		{
			return (stretch_mode != c_stretch_mode.c_default) ? stretch_mode : hive.tav.stretch_mode;
		}

		//
		// 配置位置を返します。
		//
		int32_t get_location()
		{
			return (location != c_location.c_default) ? location : hive.tav.location;
		}

		//
		// ノードの配置位置を返します。
		//
		int32_t get_node_align()
		{
			return (node_align != c_node_align.c_default) ? node_align : hive.tav.node_align;
		}

		//
		// 基準となるフォントを返します。
		//
		inline static HFONT get_base_font()
		{
			// フォントを取得します。
			AviUtl::SysInfo si = {};
			magi.auin.get_sys_info(nullptr, &si);
			return si.hfont;
		}

		//
		// 指定されたフォントと属性を元にフォントを作成して返します。
		//
		inline static HFONT create_font(HFONT font, LONG weight = 0)
		{
			auto lf = LOGFONT {};
			::GetObject(font, sizeof(lf), &lf);
			lf.lfWeight = weight;
			return ::CreateFontIndirect(&lf);
		}

		//
		// グローバルなマウスムーブメッセージを処理します。
		//
		inline static void process_global_mouse_move(const POINT& point)
		{
			// 現存するすべてのタブコントロールに
			// グローバルメッセージを処理させます。
			for (auto& tav : tavs) tav->on_global_mouse_move(point);
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND owner)
		{
			MY_TRACE_FUNC("{:#010x}", owner);

			// ウィンドウクラスを登録します。
			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = c_class_name;
			::RegisterClass(&wc);

			// ウィンドウを作成します。
			if (!create(
				WS_EX_NOPARENTNOTIFY,
				c_class_name,
				c_class_name,
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				0, 0, 0, 0,
				owner, nullptr, hive.instance, nullptr))
			{
				return FALSE;
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return destroy();
		}

		//
		// タブコントロールが有効の場合はTRUEを返します。
		//
		virtual BOOL is_enabled()
		{
			// タブを強制表示する場合は
			if (hive.show_tab_force)
			{
				// タブ項目が存在しない場合はタブコントロールは無効です。
				if (nodes.size() <= 0) return FALSE;
			}
			// タブを強制表示しない場合は
			else
			{
				// タブ項目が複数存在しない場合はタブコントロールは無効です。
				if (nodes.size() <= 1) return FALSE;
			}

			// その他の場合はタブコントロールは有効です。
			return TRUE;
		}

		//
		// タブコントロールを配置する必要がある場合はTRUEを返します。
		//
		BOOL need_deploy()
		{
			// タブコントロールが無効の場合は
			// タブコントロールを配置する必要はありません。
			if (!is_enabled()) return FALSE;

			// タブコントロールが全自動モードの場合は
			// タブコントロールを配置する必要はありません。
			if (get_display_mode() == c_display_mode.c_full_auto) return FALSE;

			// その他の場合はタブコントロールを配置する必要があります。
			return TRUE;
		}

		//
		// タブコントロールを表示する必要がある場合はTRUEを返します。
		//
		BOOL need_show()
		{
			// タブコントロールが無効の場合は
			// タブコントロールを表示する必要はありません。
			if (!is_enabled()) return FALSE;

			// タブコントロールが展開していない場合は
			if (!expanded)
			{
				// タブコントロールが全自動モードの場合は
				// タブコントロールを表示する必要はありません。
				if (get_display_mode() == c_display_mode.c_full_auto) return FALSE;
			}

			// その他の場合はタブコントロールを表示する必要があります。
			return TRUE;
		}

		//
		// このタブコントロールのレイアウトを更新します。
		//
		void update(LPCRECT workarea)
		{
			// 収縮位置を更新します。
			update_collapse_pos(workarea);

			// 各ノードの描画位置を更新します。
			update_node_pos();

			// 展開位置を更新します。
			update_expand_pos();
		}

		//
		// このタブコントロールの収縮位置を更新します。
		//
		void update_collapse_pos(LPCRECT workarea)
		{
			// 収縮位置を取得します。
			collapse_rc = get_facet()->get_collapse_rect(this, workarea);
		}

		//
		// このタブコントロールの展開位置を更新します。
		//
		void update_expand_pos()
		{
			// 展開位置を取得します。
			expand_rc = get_facet()->get_expand_rect(this);
		}

		//
		// 各ノードの描画位置を更新します。
		//
		void update_node_pos()
		{
			switch (get_location())
			{
			case c_location.c_left:
			case c_location.c_right:
				return update_node_pos(&RECT::top, &RECT::bottom);

			default:
			case c_location.c_top:
			case c_location.c_bottom:
				return update_node_pos(&RECT::left, &RECT::right);
			}
		}

		//
		// 各ノードの描画位置を更新します。
		//
		void update_node_pos(LONG RECT::* left, LONG RECT::* right)
		{
			// 更新処理が実行されるのでポスト済みフラグを消去します。
			update_posted = FALSE;

			// 行データをリセットします。
			rows.clear();

			// タブ項目が存在しない場合は何もしません。
			if (nodes.empty()) return;

			// 描画に使用するフォントを取得します。
			auto font = get_base_font();
			if (!font) return;

			// 最初の行データを追加します。
			rows.emplace_back(0, 0);

			// 表示領域のサイズを取得します。
			auto w = collapse_rc.*right - collapse_rc.*left;

			// 描画の準備をします。
			my::ClientDC dc(*this);
			my::gdi::selector font_selector(dc, font);

			// 最初の描画位置です。
			int32_t line = 0, pos = hive.tab_height;

			// すべてのノードを走査します。
			for (const auto& node : nodes)
			{
				// ノードの描画サイズを取得します。
				auto node_rc = RECT {};
				::DrawTextW(dc, node->text.c_str(), -1, &node_rc, DT_CALCRECT);
				auto node_w = node_rc.right - node_rc.left + hive.tab_space * 2; // 少し余白を持たせます。

				// 現在の行の残りの描画領域を取得します。
				auto rest = w - pos;

				// 改行不可能な場合は
				if (node == nodes.front())
				{
					// ノードの描画サイズが残りの描画領域より大きい場合は
					if (node_w > rest)
					{
						// 現在の描画位置からクライアント領域終端までを
						// このノードの描画領域に設定します。
						// (描画領域からはみ出る文字列はクリップして表示します)
						node->display = { line, pos, rest };
					}
					else
					{
						// 現在の描画位置をこのノードの描画領域に設定します。
						node->display = { line, pos, node_w };
					}
				}
				// 改行可能な場合は
				else
				{
					// ノードの描画サイズが残りの描画領域より大きい場合は
					if (node_w > rest)
					{
						// 改行して描画位置を更新します。
						line++, pos = 0;

						// 新しい行データを追加します。
						rows.emplace_back(get_node_index(node), 0);

						// 現在の描画位置をこのノードの描画領域に設定します。
						// 描画サイズがクライアント領域終端を超えないように調整します。
						// (描画領域からはみ出る文字列はクリップして表示します)
						node->display = { line, pos, std::min(node_w, w) };
					}
					else
					{
						// 現在の描画位置をこのノードの描画領域に設定します。
						node->display = { line, pos, node_w };
					}
				}

				// 次の描画位置を更新します。
				pos += node->display.size;

				// この行のノードの数を増やします。
				rows.back().node_count++;
			}

			switch (get_node_align())
			{
			case c_node_align.c_far:
				{
					// すべてのノードを走査します。
					for (const auto& node : nodes)
					{
						// タブコントロールの右辺が基軸となるように
						// ノードのX座標を変更します。
						node->display.pos = w - node->display.size - node->display.pos;
					}

					break;
				}
			case c_node_align.c_center:
				{
					// すべての行を走査します。
					for (auto& row : rows)
					{
						// 先頭行の場合はアイコン分の幅を加算しておきます。
						int32_t total_w = (&row == &rows.front()) ? hive.tab_height : 0;

						// この行のトータルの幅を求めるために
						// この行に存在するノードを走査します。
						for (int32_t i = 0; i < row.node_count; i++)
						{
							const auto& node = nodes[row.node_index + i];

							total_w += node->display.size;
						}

						// 各ノードのX座標を調整するために
						// この行に存在するノードを走査します。
						for (int32_t i = 0; i < row.node_count; i++)
						{
							const auto& node = nodes[row.node_index + i];

							// タブコントロールの中心が基軸となるように
							// ノードのX座標を変更します。
							node->display.pos += (w - total_w) / 2;
						}
					}

					break;
				}
			}
		}

		//
		// このタブコントロールのレイアウトを返します。
		// この関数を呼ぶ前に、予めレイアウトを更新しておく必要があります。
		//
		Layout get_layout()
		{
			return  {
				.show = need_show(),
				.rc = expanded ? expand_rc : collapse_rc,
			};
		}

		//
		// 指定された矩形を基準にアイコンの描画領域を算出して返します。
		//
		RECT get_icon_rect(LPCRECT rc)
		{
			return get_facet()->get_icon_rect(this, rc);
		}

		//
		// 指定された矩形を基準にノードの描画領域を算出して返します。
		//
		RECT get_node_rect(LPCRECT rc, const std::shared_ptr<Node>& node)
		{
			return get_facet()->get_node_rect(this, rc, node);
		}

		//
		// ノードの数を返します。
		//
		int32_t get_node_count() const
		{
			return (int32_t)nodes.size();
		}

		//
		// 指定されたノードのインデックスを返します。
		//
		int32_t get_node_index(const std::shared_ptr<Node>& node) const
		{
			return (int32_t)std::distance(nodes.data(), &node);
		}

		//
		// 指定されたインデックスにあるノードの文字列を返します。
		//
		std::wstring get_node_text(int32_t index)
		{
			auto c = (int32_t)nodes.size();
			if (index < 0 || index >= c) return {};

			return nodes[index]->text;
		}

		//
		// 指定されたインデックスにあるノードの文字列を変更します。
		//
		BOOL set_node_text(int32_t index, const std::wstring& text)
		{
			auto c = (int32_t)nodes.size();
			if (index < 0 || index >= c) return FALSE;

			nodes[index]->text = text;

			// タブ項目を変更したのでレイアウトを更新します。
			post_update_message();

			return TRUE;
		}

		//
		// 選択項目を返します。
		//
		std::shared_ptr<Node> get_selected_node()
		{
			return (selected_node_index >= 0) ? nodes[selected_node_index] : nullptr;
		}

		//
		// 指定されたインデックスの位置にタブ項目を追加します。
		//
		int32_t insert_node(HWND hwnd, LPCWSTR text, int32_t index)
		{
			auto node = std::make_shared<Node>();
			node->hwnd = hwnd;
			node->text = text;
			return insert_node(node, index);
		}

		//
		// 指定されたインデックスの位置にタブ項目を追加します。
		//
		int32_t insert_node(const std::shared_ptr<Node>& node, int32_t index)
		{
			auto c = (int32_t)nodes.size();
			if (index < 0 || index > c) index = c;

			// ノードをコレクションに追加します。
			nodes.insert(nodes.begin() + index, node);

			// 保有しているインデックスがずれる場合があるので修正します。
			auto normalize = [index](int32_t& target_index) {
				if (target_index >= index) target_index++;
			};
			normalize(selected_node_index);
			normalize(hot_node_index);

			// タブ項目を変更したのでレイアウトを更新します。
			post_update_message();

			return index;
		}

		//
		// 指定されたインデックスの位置にあるタブ項目を削除します。
		//
		BOOL erase_node(int32_t index)
		{
			auto c = (int32_t)nodes.size();
			if (index < 0 || index >= c) return FALSE;

			// ノードをコレクションから削除します。
			nodes.erase(nodes.begin() + index);

			// 保有しているインデックスがずれる場合があるので修正します。
			auto normalize = [index](int32_t& target_index) {
				if (target_index == index) target_index = -1;
				else if (target_index > index) target_index--;
			};
			normalize(selected_node_index);
			normalize(hot_node_index);

			// タブ項目を変更したのでレイアウトを更新します。
			post_update_message();

			return TRUE;
		}

		//
		// すべてのタブ項目を消去します。
		//
		void clear()
		{
			nodes.clear();
			selected_node_index = -1;
			hot_node_index = -1;
		}

		//
		// fromで指定された位置にあるタブ項目をtoで指定された位置へ移動します。
		// 失敗した場合は-1を返します。
		//
		int32_t move_node(int32_t from, int32_t to)
		{
			auto c = (int32_t)nodes.size();

			if (from < 0 || from >= c) return -1;
			if (to < 0 || to >= c) return -1;

			auto node = nodes[from];
			nodes.erase(nodes.begin() + from);
			nodes.insert(nodes.begin() + to, node);

			// 保有しているインデックスがずれる場合があるので修正します。
			auto normalize = [from, to](int32_t& target_index) {
				if (target_index == from)
				{
					target_index = to;
				}
				else if (from < to)
				{
					if (target_index > from && target_index <= to)
						target_index--;
				}
				else
				{
					if (target_index >= to && target_index < from)
						target_index++;
				}
			};
			normalize(selected_node_index);
			normalize(hot_node_index);

			// タブ項目を変更したのでレイアウトを更新します。
			post_update_message();

			return to;
		}

		//
		// 指定されたウィンドウハンドルに関連付けられているタブ項目のインデックスを返します。
		//
		int32_t find_node(HWND hwnd)
		{
			// すべてのノードを走査します。
			for (const auto& node : nodes)
			{
				// ノードが指定されたウィンドウハンドルを持っている場合は
				if (node->hwnd == hwnd)
				{
					// このノードのインデックスを返します。
					return get_node_index(node);
				}
			}

			// ノードが見つからなかった場合の処理です。
			return -1;
		}

		//
		// 指定された座標にあるタブ項目のインデックスを返します。
		//
		int32_t hittest(POINT point)
		{
			// クライアント領域を取得します。
			auto rc = my::get_client_rect(*this);

			// 指定された座標がクライアント領域内の場合は
			if (::PtInRect(&rc, point))
			{
				// アイコンのヒットテストを行います。
				{
					// アイコンの描画領域を取得します。
					auto icon_rc = get_icon_rect(&rc);

					// 指定された座標がアイコンの描画領域内の場合は
					if (::PtInRect(&icon_rc, point))
					{
						// アイコンのインデックスを返します。
						return c_icon_index;
					}
				}

				// すべてのノードを走査します。
				for (const auto& node : nodes)
				{
					// ノードの描画領域を取得します。
					auto node_rc = get_node_rect(&rc, node);

					// 指定された座標がノードの描画領域内の場合は
					if (::PtInRect(&node_rc, point))
					{
						// このノードのインデックスを返します。
						return get_node_index(node);
					}
				}
			}

			// ノードが見つからなかった場合の処理です。
			return -1;
		}

		//
		// このタブコントロールを展開します。
		//
		BOOL expand()
		{
			MY_TRACE_FUNC("");

			// 展開済みの場合は何もしません。
			if (expanded) return FALSE;

			// 展開済みフラグを立てます。
			expanded = TRUE;

			// タブコントロールのウィンドウ位置を展開位置に変更します。
			return set_window_pos(&expand_rc, HWND_TOP, SWP_NOACTIVATE |
				((get_display_mode() == c_display_mode.c_full_auto) ? SWP_SHOWWINDOW : 0));
		}

		//
		// このタブコントロールを収縮します。
		//
		BOOL collapse()
		{
			MY_TRACE_FUNC("");

			// 収縮済みの場合は何もしません。
			if (!expanded) return FALSE;

			// 展開済みフラグを消去します。
			expanded = FALSE;

			// タブコントロールのウィンドウ位置を収縮位置に変更します。
			return set_window_pos(&collapse_rc, nullptr, SWP_NOZORDER |
				((get_display_mode() == c_display_mode.c_full_auto) ? SWP_HIDEWINDOW : 0));
		}

		//
		// このタブコントロールのウィンドウ位置を変更します。
		//
		BOOL set_window_pos(LPCRECT rc, HWND insert_after, UINT flags)
		{
			MY_TRACE_FUNC("");

			return my::set_window_pos(*this, insert_after, rc, flags);
		}

		//
		// 親ウィンドウにWM_NOTIFYを送信します。
		//
		BOOL notify(uint32_t code)
		{
			MY_TRACE_FUNC("{}", code);

			auto header = NMHDR {
				.hwndFrom = hwnd,
				.idFrom = (UINT_PTR)::GetWindowLongPtr(hwnd, GWLP_ID),
				.code = code,
			};
			return ::SendMessage(::GetParent(hwnd), WM_NOTIFY, header.idFrom, (LPARAM)&header);
		}

		//
		// 更新メッセージをポストします。
		//
		BOOL post_update_message()
		{
			MY_TRACE_FUNC("");

			// 更新メッセージがポスト済みなので何もしません。
			if (update_posted)
			{
				MY_TRACE("更新メッセージをスキップします\n");

				return FALSE;
			}

			// 更新メッセージをポストするのでポスト済みフラグを立てます。
			update_posted = TRUE;

			// 更新メッセージをポストします。
			return ::PostMessage(*this, c_message.c_update, 0, 0);
		}

		//
		// タブコントロールを描画します。
		//
		LRESULT draw_tav()
		{
			switch (get_location())
			{
			case c_location.c_left:
			case c_location.c_right:
				return draw_tav(
					[this](const RECT& rc, const RECT& base_rc) -> RECT {
						auto h = my::get_height(base_rc);
						return {
							rc.top,
							h - rc.right,
							rc.bottom,
							h - rc.left,
						};
					});
			default:
			case c_location.c_top:
			case c_location.c_bottom:
				return draw_tav(
					[this](const RECT& rc, const RECT& base_rc) -> RECT {
						return rc;
					});
			}
		}

		//
		// タブコントロールを描画します。
		//
		LRESULT draw_tav(auto get_paint_rect)
		{
			// フォントを取得します。
			auto base_font = get_base_font();
			if (!base_font) return 0;

			// フォントを作成します。
			my::gdi::unique_ptr<HFONT> font(create_font(base_font));

			// アイコンフォントを作成します。
			my::gdi::unique_ptr<HFONT> icon_font(create_font(base_font, FW_BLACK));

			// 描画の準備をします。
			auto client_rc = my::get_client_rect(hwnd);
			auto client_w = my::get_width(client_rc);
			auto client_h = my::get_height(client_rc);
			auto rc = get_paint_rect(client_rc, {}); ::OffsetRect(&rc, -rc.left, -rc.top);
			auto enabled = ::IsWindowEnabled(hwnd);
			my::PaintDC pdc(hwnd);
			my::MemDC dc(pdc, &rc);
			my::gdi::selector font_selector(dc, font.get());

			painter.fill_background(dc, &rc, hive.fill_color, hive.theme_tav.get(), 0, 0);

			// アイコンを描画します。
			{
				my::gdi::selector icon_font_selector(dc, icon_font.get());

				// アイコン代わりの文字です。
				const std::wstring icon_text = expanded ? L"-" : L"+";

				// アイコンの描画領域を取得します。
				auto icon_rc = get_paint_rect(get_icon_rect(&client_rc), rc);

				// アイコンのテーマのパートとステートです。
				auto part_id = TABP_TOPTABITEM;
				auto state_id = TIS_NORMAL;

				// アイコンがホット状態の場合は
				if (hot_node_index == c_icon_index) state_id = TIS_HOT;

				// アイコンを描画します。
				draw_element(dc, part_id, state_id, icon_rc, icon_text,
					DT_CENTER | DT_VCENTER | DT_SINGLELINE,
					[](LPCRECT rc) {
						auto text_rc = *rc;
						// 矢印マークは1ピクセル下にずれて描画されるので
						// 描画位置を上に1ピクセルずらして相殺します。
//						::OffsetRect(&text_rc, 0, -1);
						return text_rc;
					});
			}

			// すべてのノードを走査します。
			for (const auto& node : nodes)
			{
				// ノードの描画領域を取得します。
				auto node_rc = get_paint_rect(get_node_rect(&client_rc, node), rc);

				// ノードのインデックスを取得します。
				auto i = get_node_index(node);

				// タブ項目のテーマのパートとステートです。
				auto part_id = TABP_TOPTABITEM;
				auto state_id = TIS_NORMAL;

				// タブコントロール自体が無効の場合は
				if (!enabled)
				{
					state_id = TIS_DISABLED;
				}
				// 選択ノードの場合は
				else if (i == selected_node_index)
				{
					state_id = TIS_SELECTED;
				}
				// ホットノードの場合は
				else if (i == hot_node_index)
				{
					state_id = TIS_HOT;
				}

				// ノードを描画します。
				draw_element(dc, part_id, state_id, node_rc, node->text,
					DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS,
					[](LPCRECT rc) {
						auto text_rc = *rc;
						// 縁と文字列が重ならないように描画領域を収縮します。
						::InflateRect(&text_rc, -hive.tab_space, 0);
						return text_rc;
					});
			}

			switch (get_location())
			{
			case c_location.c_left:
			case c_location.c_right:
				{
					// PlgBltの罠 - 門前の小僧
					// https://motchy99.blog.fc2.com/blog-entry-60.html
					POINT points[3] = { { dc.h, 0 }, { dc.h, dc.w }, { 0, 0 } }; // 90度回転させます。
					::PlgBlt(pdc, points, dc, 0, 0, dc.w, dc.h, nullptr, 0, 0);

					break;
				}
			case c_location.c_top:
			case c_location.c_bottom:
				{
					::BitBlt(pdc, client_rc.left, client_rc.top, client_w, client_h, dc, 0, 0, SRCCOPY);

					break;
				}
			}

			return 0;
		}

		//
		// タブコントロールの要素を描画します。
		//
		void draw_element(HDC dc, int32_t part_id, int32_t state_id,
			RECT rc, const std::wstring& text, DWORD text_flags, auto get_text_rect)
		{
			// テーマを使用する場合は
			if (hive.use_theme)
			{
				// テーマAPIを使用して描画します。
#if 1
				// タブコントロールのテーマを使用して描画する場合はこの調整が必要です。
				rc.bottom += 4;
				if (state_id != TIS_SELECTED)
					::InflateRect(&rc, 0, c_icon_index);
#endif
				// 背景を描画します。
				::DrawThemeBackground(hive.theme_tav.get(), dc, part_id, state_id, &rc, nullptr);

				// 文字の描画位置を取得します。
				auto text_rc = get_text_rect(&rc);

				// 文字の描画位置が有効の場合は
				if (text_rc.left < text_rc.right)
				{
					// 文字列を描画します。
					::DrawThemeText(hive.theme_tav.get(), dc,
						part_id, state_id, text.c_str(), -1, text_flags, 0, &text_rc);
				}
			}
			// テーマを使用しない場合は
			else
			{
				// GDIを使用して描画します。

				auto caption_color = hive.active_caption_color;
				auto caption_text_color = hive.active_caption_text_color;

				if (state_id == TIS_NORMAL)
				{
					caption_color = hive.inactive_caption_color;
					caption_text_color = hive.inactive_caption_text_color;
				}

				{
					my::gdi::unique_ptr<HBRUSH> brush(
						::CreateSolidBrush(caption_color));

					::FillRect(dc, &rc, brush.get());
				}

				auto old_bk_mode = ::SetBkMode(dc, TRANSPARENT);
				auto old_text_color = ::SetTextColor(dc, caption_text_color);
				{
					// 文字の描画位置を取得します。
					auto text_rc = get_text_rect(&rc);

					// 文字の描画位置が有効の場合は
					if (text_rc.left < text_rc.right)
						::DrawTextW(dc, text.c_str(), text.length(), &text_rc, text_flags);
				}
				::SetTextColor(dc, old_text_color);
				::SetBkMode(dc, old_bk_mode);
			}
		}

		//
		// WM_CREATEを処理します。
		//
		LRESULT on_create(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

			// このタブコントロールをコレクションに追加します。
			tavs.insert(this);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_DESTROYを処理します。
		//
		LRESULT on_destroy(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

			// このタブコントロールをコレクションから削除します。
			tavs.erase(this);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_PAINTを処理します。
		//
		LRESULT on_paint(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_PAINT, {:#010x}, {:#010x}", wParam, lParam);

			return draw_tav();
		}

		//
		// WM_MOUSEMOVEを処理します。
		//
		LRESULT on_mouse_move(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_MOUSEMOVE, {:#010x}, {:#010x}", wParam, lParam);

			// マウス座標を取得します。
			auto point = my::lp_to_pt(lParam);

			// マウス座標にあるタブ項目のインデックスを取得します。
			auto ht = hittest(point);

			// マウスキャプチャ中の場合は
			if (::GetCapture() == hwnd)
			{
				// 何もしません。
			}
			// マウスキャプチャ中ではない場合は
			else
			{
				// 選択モードがクリックの場合は
				if (get_select_mode() == c_select_mode.c_click)
				{
					// マウス座標にあるタブ項目がホット状態ではない場合は
					if (hot_node_index != ht)
					{
						// マウス座標にあるタブ項目をホット状態にします。
						hot_node_index = ht;

						// 再描画します。
						my::invalidate(hwnd);

						// ホットノードが有効の場合は
						if (hot_node_index != -1)
						{
							// WM_MOUSELEAVEが発行されるようにします。
							my::track_mouse_event(hwnd);
						}
					}
				}
				// 選択モードがホバーの場合は
				else
				{
					// マウス座標にタブ項目がある場合は
					if (ht >= 0)
					{
						// マウス座標にあるタブ項目が選択状態ではない場合は
						if (selected_node_index != ht)
						{
							// マウス座標にあるタブ項目を選択状態にします。
							selected_node_index = ht;

							// 再描画します。
							my::invalidate(hwnd);

							// 親ウィンドウに通知します。
							notify(TCN_SELCHANGE);
						}
					}
				}

				// タブコントロールが収縮状態でマウス座標にアイコンがある場合は
				if (!expanded && ht == c_icon_index)
				{
					// 半自動モードの場合は
					if (get_display_mode() == c_display_mode.c_semi_auto)
					{
						// タブコントロールを展開します。
						expand();
					}
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// グローバルなマウスムーブメッセージを処理します。
		//
		LRESULT on_global_mouse_move(const POINT& point)
		{
//			MY_TRACE_FUNC("{}, {}", point.x, point.y);

			// 親ウィンドウが非表示の場合は何もしません。
			if (!::IsWindowVisible(::GetParent(*this))) return 0;

			// 手動モード以外の場合は
			if (get_display_mode() != c_display_mode.c_manual)
			{
				// タブコントロールが展開している場合は
				if (expanded)
				{
					// 収縮トリガーを取得します。
					auto rc = get_facet()->get_collapse_trigger_rect(this);

					// 親ウィンドウのクライアント座標からスクリーン座標に変換します。
					my::map_window_points(::GetParent(*this), nullptr, &rc);

					// 収縮トリガー外の場合は
					if (!::PtInRect(&rc, point))
					{
						// タブコントロールを収縮します。
						collapse();
					}
				}
				// タブコントロールが展開していない場合で
				// なおかつタブコントロールが有効の場合は
				else if (is_enabled())
				{
					// 展開トリガーを取得します。
					auto rc = get_facet()->get_expand_trigger_rect(this);

					// 親ウィンドウのクライアント座標からスクリーン座標に変換します。
					my::map_window_points(::GetParent(*this), nullptr, &rc);

					// 展開トリガー内の場合は
					if (::PtInRect(&rc, point))
					{
						// タブコントロールを展開します。
						expand();
					}
				}
			}

			return 0;
		}

		//
		// WM_MOUSELEAVEを処理します。
		//
		LRESULT on_mouse_leave(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_MOUSELEAVE, {:#010x}, {:#010x}", wParam, lParam);

			// ホットノードを無効化します。
			hot_node_index = -1;

			// 再描画します。
			my::invalidate(hwnd);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_LBUTTONDOWNを処理します。
		//
		LRESULT on_l_button_down(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_LBUTTONDOWN, {:#010x}, {:#010x}", wParam, lParam);

			// マウス座標を取得します。
			auto point = my::lp_to_pt(lParam);

			// マウス座標にあるタブ項目のインデックスを取得します。
			auto ht = hittest(point);

			// マウス座標にあるタブ項目をホット状態にします。
			hot_node_index = ht;

			// 再描画します。
			my::invalidate(hwnd);

			// マウスキャプチャを開始します。
			::SetCapture(hwnd);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_LBUTTONUPを処理します。
		//
		LRESULT on_l_button_up(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_LBUTTONUP, {:#010x}, {:#010x}", wParam, lParam);

			// マウスキャプチャを終了します。
			::ReleaseCapture();

			// マウス座標を取得します。
			auto point = my::lp_to_pt(lParam);

			// マウス座標にあるタブ項目のインデックスを取得します。
			auto ht = hittest(point);

			// ホットノード上でマウスアップした場合は
			if (ht == hot_node_index)
			{
				// マウス座標にアイコンがある場合は
				if (ht == c_icon_index)
				{
					// 展開・収縮を切り替えます。
					expanded ? collapse() : expand();
				}
				else
				{
					if (toggle_select)
					{
						if (ht == selected_node_index) ht = -1;

						// 選択項目とマウス座標にあるタブ項目が異なる場合は
						if (selected_node_index != ht)
						{
							// マウス座標にあるタブ項目を選択状態にします。
							selected_node_index = ht;

							// 再描画します。
							my::invalidate(hwnd);

							// 親ウィンドウに通知します。
							notify(TCN_SELCHANGE);
						}
					}
					else
					{
						// マウス座標にタブ項目がある場合は
						if (ht >= 0 && selected_node_index != ht)
						{
							// マウス座標にあるタブ項目を選択状態にします。
							selected_node_index = ht;

							// 再描画します。
							my::invalidate(hwnd);

							// 親ウィンドウに通知します。
							notify(TCN_SELCHANGE);
						}
					}
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_LBUTTONDBLCLKを処理します。
		//
		LRESULT on_l_button_dbl_clk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_LBUTTONDBLCLK, {:#010x}, {:#010x}", wParam, lParam);

			// 親ウィンドウにNM_DBLCLKを通知します。
			notify(NM_DBLCLK);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// WM_RBUTTONUPを処理します。
		//
		LRESULT on_r_button_up(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("WM_RBUTTONUP, {:#010x}, {:#010x}", wParam, lParam);

			// 親ウィンドウにNM_RCLICKを通知します。
			notify(NM_RCLICK);

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// c_message.c_updateを処理します。
		//
		LRESULT on_update(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			MY_TRACE_FUNC("c_message.c_update, {:#010x}, {:#010x}", wParam, lParam);

			// 各ノードの描画位置を更新します。
			update_node_pos();

			// 展開位置を更新します。
			update_expand_pos();

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

			switch (message)
			{
			case WM_CREATE: return on_create(hwnd, message, wParam, lParam);
			case WM_DESTROY: return on_destroy(hwnd, message, wParam, lParam);
			case WM_PAINT: return on_paint(hwnd, message, wParam, lParam);
			case WM_MOUSEMOVE: return on_mouse_move(hwnd, message, wParam, lParam);
			case WM_MOUSELEAVE: return on_mouse_leave(hwnd, message, wParam, lParam);
			case WM_LBUTTONDOWN: return on_l_button_down(hwnd, message, wParam, lParam);
			case WM_LBUTTONUP: return on_l_button_up(hwnd, message, wParam, lParam);
			case WM_LBUTTONDBLCLK: return on_l_button_dbl_clk(hwnd, message, wParam, lParam);
			case WM_RBUTTONUP: return on_r_button_up(hwnd, message, wParam, lParam);
			case c_message.c_update: return on_update(hwnd, message, wParam, lParam);
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	};
}
