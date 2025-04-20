#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはカスタムオブジェクトのツリーダイアログです。
	//
	inline struct TreeDialog : StdAddinDialog<IDD_CUSTOM_OBJECT_TREE>
	{
		//
		// アイテム矩形のマージンです。
		//
		inline static constexpr auto c_margin = 8;

		//
		// ハンドルからツリーノードを取得するためのコレクションです。
		//
		std::unordered_map<HTREEITEM, std::shared_ptr<Agit::Tree::Node>> from_handle_map;

		//
		// ドラッグ用のオブジェクトです。
		//
		struct : Dragger
		{
			//
			// ドラッグ元項目です。
			//
			HTREEITEM drop_source = {};

			//
			// ドラッグ先項目です。
			//
			HTREEITEM drop_target = {};

			//
			// 挿入先項目です。
			//
			HTREEITEM insert_mark = {};

			//
			// 挿入先位置です。
			//
			BOOL insert_after = {};

			//
			// ドロップ項目をリセットします。
			//
			void reset()
			{
				drop_source = {};
				drop_target = {};
				insert_mark = {};
				insert_after = {};
			}
		} dragger;

		//
		// チェック状態をモードに変換して返します。
		//
		inline static int32_t check_state_to_mode(int32_t check_state)
		{
			return std::max((check_state >> 12) - 1, 0);
		}

		//
		// モードをチェック状態に変換して返します。
		//
		inline static int32_t mode_to_check_state(int32_t mode)
		{
			return (mode + 1) << 12;
		}

		//
		// 指定された項目に関連付けられているツリーノードを返します。
		//
		auto get_tree_node(HTREEITEM item_handle)
		{
			auto it = from_handle_map.find(item_handle);
			if (it == from_handle_map.end()) return std::shared_ptr<Agit::Tree::Node>();
			return it->second;
		}

		//
		// 指定された項目にツリーノードを関連付けます。
		//
		void set_tree_node(HTREEITEM item_handle, const auto& node)
		{
			from_handle_map[item_handle] = node;
		}

		//
		// 指定された項目に対応するイテレータを返します。
		//
		auto get_iterator(const auto& parent_node, HTREEITEM item_handle)
		{
			if (item_handle == TVI_FIRST)
			{
				return parent_node->vec.begin();
			}
			else if (item_handle == TVI_LAST)
			{
				return parent_node->vec.end();
			}
			else
			{
				return std::find(parent_node->vec.begin(), parent_node->vec.end(), get_tree_node(item_handle));
			}
		}

		//
		// 項目名を作成して返します。
		//
		auto make_item_text(const auto& node)
		{
			if (node->pattern.empty()) return node->name;
			auto target = agit.c_column.c_datas[node->target].text;
			return my::format(_T("{/} ({/} = {/})"), node->name, target, node->pattern);
		}

		//
		// ツリービューを初期化します。
		//
		BOOL init_tree_view()
		{
			MY_TRACE_FUNC("");

			auto tree_view = ctrl(IDC_TREE);

			TreeView_SetExtendedStyle(
				tree_view,
				(uint32_t)-1, // すべてのマスクです。
//				TVS_EX_DOUBLEBUFFER |
//				TVS_EX_DIMMEDCHECKBOXES |
				TVS_EX_EXCLUSIONCHECKBOXES |
//				TVS_EX_PARTIALCHECKBOXES |
//				TVS_EX_FADEINOUTEXPANDOS |
				0);

			auto item_height = TreeView_GetItemHeight(tree_view);
			item_height += c_margin; // 見た目の隙間が大きすぎるので2倍にしません。
//			item_height += c_margin * 2;
			TreeView_SetItemHeight(tree_view, item_height);

			return TRUE;
		}

		//
		// ツリービューの指定された項目と位置に子項目を作成します。
		//
		BOOL insert_tree_item(HWND tree_view, HTREEITEM parent_handle, HTREEITEM insert_after, const auto& child)
		{
			// モードから項目の状態を取得します。
			auto state = mode_to_check_state(child->mode);

			// 展開状態を項目の状態に適用します。
			if (child->expanded) state |= TVIS_EXPANDED;

			// ツリービュー項目を作成します。
			auto item_handle = tv::insert_item(tree_view,
				parent_handle, insert_after, make_item_text(child).c_str(), state, 0);

			// 作成した項目とノードを同一化します。
			return equate_tree_item(tree_view, item_handle, child);
		}

		//
		// ツリービューの項目とノードを同一化します。
		//
		BOOL equate_tree_item(HWND tree_view, HTREEITEM parent_handle, const auto& parent)
		{
			// 親ノードと親項目を関連付けます。
			set_tree_node(parent_handle, parent);

			// 子ノードを走査します。
			for (const auto& node : parent->vec)
			{
				// 子項目を作成します。
				insert_tree_item(tree_view, parent_handle, TVI_LAST, node);
			}

			return TRUE;
		}

		//
		// ツリービューから項目を削除します。
		//
		BOOL erase_tree_item(HWND tree_view, HTREEITEM item_handle)
		{
			return TreeView_DeleteItem(tree_view, item_handle);
		}

		//
		// ツリービューのすべての項目を削除します。
		//
		BOOL erase_all_tree_items(HWND tree_view)
		{
			from_handle_map.clear();

			return TreeView_DeleteAllItems(tree_view);
		}

		//
		// ツリービューの項目を移動します。
		// fromをtoの子項目に変更します。
		//
		BOOL move_tree_item(HWND tree_view, HTREEITEM from, HTREEITEM to)
		{
			// 移動元ノードを取得します。
			auto from_node = get_tree_node(from);
			if (!from_node) return FALSE;

			// 移動先ノードを取得します。
			auto to_node = get_tree_node(to);
			if (!to_node) return FALSE;

			// 移動元ノードを親ノードから切り離します。
			agit.tree.erase(from_node);

			// 移動元項目を削除します。
			erase_tree_item(tree_view, from);

			// 移動元ノードを移動先ノードに追加します。
			agit.tree.add(to_node, to_node->vec.end(), from_node);

			// 移動元項目を追加します。
			return insert_tree_item(tree_view, to, TVI_LAST, from_node);
		}

		//
		// ツリービューの項目を移動します。
		// item_handleをparent_handleのinsert_afterの位置に移動します。
		//
		BOOL move_tree_item(HWND tree_view, HTREEITEM parent_handle, HTREEITEM insert_after, HTREEITEM item_handle)
		{
			// 移動元ノードを取得します。
			auto node = get_tree_node(item_handle);
			if (!node) return FALSE;

			// 移動先ノードを取得します。
			auto parent_node = get_tree_node(parent_handle);
			if (!parent_node) return FALSE;

			// 移動元ノードを親ノードから切り離します。
			agit.tree.erase(node);

			// 移動元項目を削除します。
			erase_tree_item(tree_view, item_handle);

			// 挿入位置を取得します。
			auto it = get_iterator(parent_node, insert_after);

			// 移動元ノードを移動先ノードに追加します。
			agit.tree.add(parent_node, it, node);

			// 移動元項目を追加します。
			return insert_tree_item(tree_view, parent_handle, insert_after, node);
		}

		//
		// ツリービューの項目を更新します。
		//
		BOOL update_tree_item(HWND tree_view, HTREEITEM item_handle, const auto& node)
		{
			auto text = make_item_text(node);
			TVITEM item = {
				.mask = TVIF_HANDLE | TVIF_TEXT,
				.hItem = item_handle,
				.pszText = text.data(),
			};
			return TreeView_SetItem(tree_view, &item);
		}

		//
		// ツリービューを更新します。
		//
		BOOL update_tree_view()
		{
			MY_TRACE_FUNC("");

			// ツリービューを取得します。
			auto tree_view = ctrl(IDC_TREE);

			// 一旦すべての項目を削除します。
			erase_all_tree_items(tree_view);

			// ルート項目とノードを同一化します。
			equate_tree_item(tree_view, TVI_ROOT, agit.tree.root);

			return TRUE;
		}

		//
		// 指定された項目が有効なドロップ先項目の場合はTRUEを返します。
		//
		BOOL is_valid_drop_target(HWND tree_view, HTREEITEM item_handle)
		{
			// 指定された項目がルートの場合は
			if (!item_handle || item_handle == TVI_ROOT)
				return TRUE; // ドロップ先項目にできます。

			// 項目からツリーノードを取得します。
			auto node = get_tree_node(item_handle);

			// ツリーノードが取得できない場合はドロップ先項目にできません。
			if (!node) return FALSE;

			// ツリーノードが組み込みの場合はドロップ先項目にできません。
			if (node->id.length()) return FALSE;

			// 指定された項目の先祖を走査します。
			for (auto ancestor = item_handle; ancestor;
				ancestor = TreeView_GetParent(tree_view, ancestor))
			{
				// ドロップ元項目が先祖の場合はドロップ先項目にできません。
				if (dragger.drop_source == ancestor)
					return FALSE;
			}

			return TRUE;
		}

		//
		// 項目のドラッグを開始します。
		//
		BOOL begin_drag(HWND tree_view, HTREEITEM item_handle)
		{
			// 項目からツリーノードを取得します。
			auto node = get_tree_node(item_handle);
			if (!node) return FALSE;

			// ツリーノードのIDが有効の場合は
			// 組み込みノードなのでドラッグできません。
			if (node->id.length()) return FALSE;

			// ドロップ項目をリセットします。
			dragger.reset();

			// ドラッグ元項目をセットします。
			dragger.drop_source = item_handle;

			// ドラッグを開始します。
			dragger.start(node->name);

			// マウスキャプチャを開始します。
			::SetCapture(*this);

			return TRUE;
		}

		//
		// 項目のドラッグを更新します。
		//
		BOOL move_drag(HWND tree_view)
		{
			// マウスをキャプチャしていない場合は何もしません。
			if (::GetCapture() != *this) return FALSE;

			// ヒットテストを実行します。
			auto hti = TVHITTESTINFO {};
			hti.pt = my::get_cursor_pos();
			my::map_window_points(nullptr, tree_view, &hti.pt);
			auto item_handle = TreeView_HitTest(tree_view, &hti);

			// ドロップ先項目の親項目を取得します。
			auto parent_handle = TreeView_GetParent(tree_view, item_handle);
			if (!parent_handle) parent_handle = TVI_ROOT;

			// ドロップ先項目の矩形を取得します。
			auto rc = RECT {}; TreeView_GetItemRect(tree_view, item_handle, &rc, TRUE);
			::InflateRect(&rc, 0, -c_margin); // 項目間に隙間が空くように調整します。

			dragger.drop_target = {};
			dragger.insert_mark = {};
			dragger.insert_after = {};

			if (hti.pt.y < rc.top)
			{
				// 親項目がドロップターゲットとして有効の場合は
				if (is_valid_drop_target(tree_view, parent_handle))
				{
					dragger.insert_mark = item_handle;
					dragger.insert_after = FALSE;
				}
			}
			else if (hti.pt.y > rc.bottom)
			{
				// 親項目がドロップターゲットとして有効の場合は
				if (is_valid_drop_target(tree_view, parent_handle))
				{
					dragger.insert_mark = item_handle;
					dragger.insert_after = TRUE;
				}
			}
			else
			{
				// ドロップ先項目がドロップターゲットとして有効の場合は
				if (is_valid_drop_target(tree_view, item_handle))
				{
					dragger.drop_target = item_handle;
				}
			}

			// ドロップターゲットをセットします。
			TreeView_SelectDropTarget(tree_view, dragger.drop_target);

			// 挿入マークをセットします。
			TreeView_SetInsertMark(tree_view, dragger.insert_mark, dragger.insert_after);

			// ドラッグを更新します。
			dragger.move();

			return TRUE;
		}

		//
		// 項目のドラッグを終了します。
		//
		BOOL end_drag(HWND tree_view)
		{
			// マウスをキャプチャしていない場合は何もしません。
			if (::GetCapture() != *this) return FALSE;

			// マウスキャプチャを終了します。
			::ReleaseCapture();

			// ドラッグを終了します。
			dragger.stop();

			// ドロップ先項目が有効の場合は
			if (dragger.drop_target)
			{
				// ドロップ元項目をドロップ先項目の子項目にします。
				move_tree_item(tree_view, dragger.drop_source, dragger.drop_target);
			}
			// 挿入項目が有効の場合は
			else if (dragger.insert_mark)
			{
				// 挿入項目の親項目を取得します。
				auto parent_handle = TreeView_GetParent(tree_view, dragger.insert_mark);
				if (!parent_handle) parent_handle = TVI_ROOT;

				if (dragger.insert_after)
				{
					// ドロップ元項目をドロップ先項目の後項目にします。
					move_tree_item(tree_view, parent_handle, dragger.insert_mark, dragger.drop_source);
				}
				else
				{
					// ドロップ先項目の前項目を取得します。
					auto prev_handle = TreeView_GetPrevSibling(tree_view, dragger.insert_mark);
					if (!prev_handle) prev_handle = TVI_FIRST;

					// ドロップ元項目をドロップ先項目の前項目にします。
					move_tree_item(tree_view, parent_handle, prev_handle, dragger.drop_source);
				}
			}

			// ドロップターゲットを消去します。
			TreeView_SelectDropTarget(tree_view, nullptr);

			// 挿入マークを消去します。
			TreeView_SetInsertMark(tree_view, nullptr, FALSE);

			// ドロップ項目をリセットします。
			dragger.reset();

			// フィルタが変更された可能性があるのでリストを更新します。
			action->filter_list();

			return TRUE;
		}

		//
		// コンテキストメニューを表示します。
		//
		BOOL show_context_menu(HWND tree_view, HTREEITEM item_handle)
		{
			// 項目からツリーノードを取得します。
			auto node = get_tree_node(item_handle);

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_add_tree_node_as_top_level = 1;
			constexpr uint32_t c_add_tree_node_as_child = 2;
			constexpr uint32_t c_edit_tree_node = 10;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// ポップアップメニューに項目を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_add_tree_node_as_top_level, _T("トップレベル項目を追加(&R)"));
			if (node) {
				::AppendMenu(menu.get(), MF_STRING, c_add_tree_node_as_child, _T("選択項目に子項目を追加(&C)"));
				::AppendMenu(menu.get(), MF_STRING, c_edit_tree_node, _T("選択項目を編集(&E)"));
			}

			// ポップアップメニューを表示します。
			auto point = my::get_cursor_pos();
			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, *this, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_add_tree_node_as_top_level:
				{
					// ツリーノードの設定用のダイアログを作成します。
					TreeNodeConfigDialog dialog(*this);

					// ダイアログのタイトルを変更します。
					::SetWindowText(dialog, _T("トップレベル項目を追加"));

					// ダイアログを表示します。
					if (IDOK != dialog.do_modal(*this)) return FALSE;

					// トップレベルツリーノードを先頭に追加します。
					auto new_node = agit.tree.add(
						agit.tree.root,
						agit.tree.root->vec.begin(),
						L"",
						dialog.name,
						dialog.pattern,
						dialog.target,
						dialog.mode,
						dialog.expanded);

					// トップレベル項目を先頭に追加します。
					insert_tree_item(tree_view, TVI_ROOT, TVI_FIRST, new_node);

					// フィルタが変更された可能性があるのでリストを更新します。
					action->filter_list();

					break;
				}
			case c_add_tree_node_as_child:
				{
					if (!node) return FALSE;

					// ツリーノードの設定用のダイアログを作成します。
					TreeNodeConfigDialog dialog(*this);

					// ダイアログのタイトルを変更します。
					::SetWindowText(dialog, _T("子項目を追加"));

					// ダイアログを表示します。
					if (IDOK != dialog.do_modal(*this)) return FALSE;

					// 子ツリーノードを先頭に追加します。
					auto new_node = agit.tree.add(
						node,
						node->vec.begin(),
						L"",
						dialog.name,
						dialog.pattern,
						dialog.target,
						dialog.mode,
						dialog.expanded);

					// 子項目を先頭に追加します。
					insert_tree_item(tree_view, item_handle, TVI_FIRST, new_node);

					// フィルタが変更された可能性があるのでリストを更新します。
					action->filter_list();

					break;
				}
			case c_edit_tree_node:
				{
					if (!node) return FALSE;

					// ツリーノードの設定用のダイアログを作成します。
					TreeNodeConfigDialog dialog(*this);

					// ダイアログのタイトルを変更します。
					::SetWindowText(dialog, _T("ツリーノードを編集"));

					dialog.name = node->name;
					dialog.pattern = node->pattern;
					dialog.target = node->target;

					// ダイアログを表示します。
					if (IDOK != dialog.do_modal(*this)) return FALSE;

					node->name = dialog.name;
					node->pattern = dialog.pattern;
					node->target = dialog.target;

					// ツリービューの項目を更新します。
					update_tree_item(tree_view, item_handle, node);

					// フィルタが変更された可能性があるのでリストを更新します。
					action->filter_list();

					break;
				}
			}

			return TRUE;
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_tree_view();

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row_size = base_size + margin_value * 2;
			auto row = std::make_shared<RelativePos>(row_size);
			auto size_s = std::make_shared<RelativePos>(base_size * 3);
			auto size_m = std::make_shared<RelativePos>(base_size * 4);
			auto size_l = std::make_shared<RelativePos>(base_size * 6);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, full);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, {}, ctrl(IDC_TREE));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_MOUSEMOVE:
				{
					MY_TRACE_FUNC("WM_MOUSEMOVE, {/hex}, {/hex}", wParam, lParam);

					move_drag(ctrl(IDC_TREE));

					break;
				}
			case WM_LBUTTONUP:
				{
					MY_TRACE_FUNC("WM_LBUTTONUP, {/hex}, {/hex}", wParam, lParam);

					end_drag(ctrl(IDC_TREE));

					break;
				}
			case WM_NOTIFY:
				{
					MY_TRACE_FUNC("WM_NOTIFY, {/hex}, {/hex}", wParam, lParam);

					auto header = (NMHDR*)lParam;
					if (header->idFrom != IDC_TREE) break;

					// ツリービューを取得します。
					auto tree_view = header->hwndFrom;

					switch (header->code)
					{
					case NM_CLICK:
						{
							// クリックされた項目を選択状態にします。
							// チェック状態も切り替えます。

							// ヒットテストを実行します。
							auto hti = TVHITTESTINFO {};
							hti.pt = my::get_cursor_pos();
							my::map_window_points(nullptr, tree_view, &hti.pt);
							auto item_handle = TreeView_HitTest(tree_view, &hti);

							// 状態アイコンがクリックされた場合は
							if (hti.flags & TVHT_ONITEMSTATEICON)
							{
								// クリックされた項目の状態を取得します。
								// ※マスクを渡しても無視されるので0を指定しています。
								auto item_state = TreeView_GetItemState(tree_view, item_handle, 0);

								// クリックされた項目のチェック状態を取得します。
								auto check_state = item_state & TVIS_STATEIMAGEMASK;

								// チェック状態からモードを取得します。
								auto mode = check_state_to_mode(check_state);

								// Shiftキーが押されている場合は
								if (::GetKeyState(VK_SHIFT) < 0)
								{
									// exclusion/offを切り替えます。
									mode = (mode != agit.tree.c_mode.c_exclusion) ?
										agit.tree.c_mode.c_exclusion : agit.tree.c_mode.c_off;
								}
								// Shiftキーが押されていない場合は
								else
								{
									// on/offを切り替えます。
									mode = (mode == agit.tree.c_mode.c_off) ?
										agit.tree.c_mode.c_on : agit.tree.c_mode.c_off;
								}

								// モードからチェック状態を取得します。
								check_state = mode_to_check_state(mode);

								// 項目の状態からチェック状態を取り除きます。
								item_state &= ~TVIS_STATEIMAGEMASK;

								// 項目の状態にチェック状態を結合します。
								item_state |= check_state;

								// クリックされた項目のチェック状態を変更します。
								TreeView_SetItemState(tree_view, item_handle, item_state, TVIS_STATEIMAGEMASK);

								// ツリーノードに新しいチェック状態を適用します。
								if (auto node = get_tree_node(item_handle)) node->mode = mode;

								// ツリーノードのチェック状態を変更したのでリストを更新します。
								action->filter_list();

								// デフォルト処理を抑制するためにTRUEを返します。
								return TRUE;
							}

							break;
						}
					case NM_RCLICK:
						{
							// クリックされた項目を選択状態にします。
							// コンテキストメニューも表示します。

							// ヒットテストを実行します。
							auto hti = TVHITTESTINFO {};
							hti.pt = my::get_cursor_pos();
							my::map_window_points(nullptr, tree_view, &hti.pt);
							auto item_handle = TreeView_HitTest(tree_view, &hti);

							if (item_handle)
							{
								// クリックされた項目を選択状態にします。
								TreeView_Select(header->hwndFrom, item_handle, TVGN_CARET);
							}

							// コンテキストメニューを表示します。
							show_context_menu(tree_view, item_handle);

							break;
						}
					case TVN_ITEMEXPANDED:
						{
							auto nm = (NMTREEVIEW*)lParam;

							// ツリーノードを取得できた場合は
							if (auto node = get_tree_node(nm->itemNew.hItem))
							{
								// 展開状態を取得しておきます。
								node->expanded = !!(nm->itemNew.state & TVIS_EXPANDED);
							}

							break;
						}
					case TVN_BEGINDRAG:
						{
							auto nm = (NMTREEVIEW*)lParam;

							// ドラッグを開始します。
							begin_drag(tree_view, nm->itemNew.hItem);

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} tree_dialog;
}
