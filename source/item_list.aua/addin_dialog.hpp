#pragma once

namespace apn::item_list
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct addin_dialog_t : StdAddinDialog<idd_item_list>
	{
		//
		// コントロールを初期化します。
		//
		BOOL init_controls()
		{
			MY_TRACE_FUNC("");

			// リストビューを初期化します。
			if (!listview.init(ctrl(idc_list))) return FALSE;

			// インプレースエディットボックスを初期化します。
			if (!inplace_edit.init(listview, ctrl(idc_inplace_editbox))) return FALSE;

			// 絞り込み用コンボボックスの管理者を初期化します。
			if (!combobox_manager.init(*this, idc_layer)) return FALSE;

			// 列データを走査します。
			for (auto i = 0; i < list.k_column.k_max_size; i++)
			{
				// 列データを取得します。
				const auto& data = list.k_column.k_datas[i];

				// リストビューに列を追加します。
				lv::insert_column(listview, i, data.text, data.width);
			}

			return TRUE;
		}

		//
		// リストビューの列名を変更します。
		//
		BOOL set_column_text(int32_t col_index)
		{
			MY_TRACE_FUNC("{/}", col_index);

			// 列データから列名を取得します。
			auto text = (std::wstring)list.k_column.k_datas[col_index].text;

			// ソートノードの番号です。
			auto index = (int32_t)1;

			// ソートノードを走査します。
			for (const auto& node : sort.nodes)
			{
				// 指定された列のソートノードの場合は
				if (node->col_index == col_index)
				{
					// ソート情報を列名に加えます。
					text += node->ascendant ? L"△" : L"▽";
					text += my::format(L"{/}", index);

					// ループを終了します。
					break;
				}

				// ソートノードの番号を増やします。
				index++;
			}

			// リストビューの列名を変更します。
			LVCOLUMN col = {
				.mask = LVCF_TEXT,
				.pszText = text.data(),
			};
			return ListView_SetColumn(listview, col_index, &col);
		}

		//
		// リストビューの項目をソートします。
		//
		BOOL sort_items(int32_t col_index)
		{
			MY_TRACE_FUNC("{/}", col_index);

			// ソートノードを追加します。
			sort.add(col_index);

			// リストをソートします。
			sort.apply();

			// リストを絞り込みます。
			finder.apply();

			// ヘッダーのテキストを更新します。
			for (int32_t i = 0; i < list.k_column.k_max_size; i++)
				set_column_text(i);

			// リストビューをリフレッシュします。
			listview.refresh();

			return TRUE;
		}

		//
		// カラムをリセットします。
		//
		BOOL reset_columns()
		{
			MY_TRACE_FUNC("");

			// 列データを走査します。
			for (auto i = 0; i < list.k_column.k_max_size; i++)
			{
				// 列データを取得します。
				const auto& data = list.k_column.k_datas[i];

				// リストビューの列名を変更します。
				LVCOLUMN col = {
					.mask = LVCF_TEXT,
					.pszText = (LPWSTR)data.text,
				};
				ListView_SetColumn(listview, i, &col);
			}

			return TRUE;
		}

		//
		// リストビューのコンテキストメニューを表示します。
		//
		BOOL show_context_menu(int32_t index, int32_t sub_item)
		{
			MY_TRACE_FUNC("{/}, {/}", index, sub_item);

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t k_flag_as_time = 1000;
			constexpr uint32_t k_refresh = 1100;
			constexpr uint32_t k_reset_sort = 1101;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr menu(::CreatePopupMenu());

			// ポップアップメニューに項目を追加します。
			::AppendMenu(menu.get(), MF_STRING, k_flag_as_time, _T("フレームを時間で表示"));
			::AppendMenu(menu.get(), MF_SEPARATOR, 0, nullptr);
			::AppendMenu(menu.get(), MF_STRING, k_refresh, _T("リフレッシュ"));
			::AppendMenu(menu.get(), MF_STRING, k_reset_sort, _T("並び順をリセット"));

			// ポップアップメニューを表示します。
			auto point = my::get_cursor_pos();
			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, listview, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case k_flag_as_time:
				{
					// 時間で表示するフラグを切り替えます。
					hive.flag_as_time = !hive.flag_as_time;

					// リストをリフレッシュします。
					app->refresh_list();

					break;
				}
			case k_refresh:
				{
					// リストをリフレッシュします。
					app->refresh_list();

					break;
				}
			case k_reset_sort:
				{
					// ソートをリセットします。
					app->reset_sort();

					break;
				}
			}

			return TRUE;
		}

		//
		// ダイアログのレイアウトを更新します。
		//
		BOOL recalc_layout(BOOL flag_listview, BOOL flag_finders)
		{
			MY_TRACE_FUNC("");

			// ダイアログのクライアント矩形を取得します。
			auto client_rc = my::get_client_rect(*this);

			// レイアウトの基準となるサイズを取得します。
			auto base_rc = my::get_window_rect(ctrl(combobox_manager.first_id));
			auto base_w = my::get_width(base_rc);
			auto base_h = my::get_height(base_rc);

			my::DeferWindowPos dwp(1 + list.k_column.k_max_size);

			// 絞り込み用コンボボックスのウィンドウ位置を変更します。
			if (flag_finders)
			{
				auto rc = client_rc;
				rc.bottom = rc.top + base_h;
				auto x = client_rc.left - listview.horz_scroll;

				int order_array[list.k_column.k_max_size] = {};
				ListView_GetColumnOrderArray(listview, std::size(order_array), order_array);

				for (size_t i = 0; i < std::size(order_array); i++)
				{
					auto order = order_array[i];

					LVCOLUMN column = {};
					column.mask = LVCF_WIDTH | LVCF_ORDER | LVCF_SUBITEM;
					ListView_GetColumn(listview, order, &column);

					auto combobox = ctrl(combobox_manager.first_id + order);

					rc.left = x; x += column.cx;
					rc.right = x;

					dwp.set_window_pos(combobox, nullptr, &rc, SWP_NOZORDER | SWP_NOACTIVATE);

					::SendMessage(combobox, CB_SETDROPPEDWIDTH, std::max(my::get_width(rc), 200), 0);
				}
			}

			// リストビューのウィンドウ位置を変更します。
			if (flag_listview)
			{
				auto rc = client_rc;
				rc.top += base_h;

				dwp.set_window_pos(listview, nullptr, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
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

			init_controls();
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			// 列のインデックスを算出します。
			auto col_index = control_id - combobox_manager.first_id;

			// 列のインデックスが有効の場合は
			if((size_t)col_index < std::size(finder.texts))
			{
				if (code == CBN_SELCHANGE)
				{
					// 絞り込み用テキストをセットします。
					finder.texts[col_index] = my::combobox::get_text(control, -1);

					// フィルタが変更された可能性があるのでリストを更新します。
					app->apply_finder();
				}
			}
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_SIZE:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// ダイアログのレイアウトを更新します。
					recalc_layout(TRUE, TRUE);

					break;
				}
			case WM_HSCROLL:
				{
					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// リストビューが水平スクロールされたので、
					// 絞り込み用コンボボックスのレイアウトを更新します。
					recalc_layout(FALSE, TRUE);

					// リストビューのスクロールに合わせて、ダイアログを即時描画します。
					::UpdateWindow(*this);

					break;
				}
			case WM_NOTIFY:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					auto nmh = (NMHDR*)l_param;

					switch (nmh->code)
					{
					case LVN_COLUMNCLICK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_COLUMNCLICK");

							// リストビューではない場合は何もしません。
							if (nmh->hwndFrom != listview) break;

							// カラムがクリックされたときの処理です。

							auto nm = (NM_LISTVIEW*)l_param;

							// クリックされたカラムを基準にリストビューをソートします。
							// 同時に、クリックされたカラムのテキストを更新します。
							sort_items(nm->iSubItem);

							break;
						}
					case NM_CLICK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, NM_CLICK");

							// リストビューではない場合は何もしません。
							if (nmh->hwndFrom != listview) break;

							// リストビューがクリックされたときの処理です。

							auto nm = (NMITEMACTIVATE*)l_param;

							// 項目のインデックスが範囲外の場合は何もしません。
							if ((size_t)nm->iItem >= finder.nodes.size()) break;
							if ((size_t)nm->iSubItem >= (size_t)list.k_column.k_max_size) break;

							// 選択したアイテムをフォーカスオブジェクトにします。
							magi.exin.set_current_object(finder.nodes[nm->iItem]->object_index);

							break;
						}
					case NM_DBLCLK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, NM_DBLCLK");

							// リストビューではない場合は何もしません。
							if (nmh->hwndFrom != listview) break;

							// リストビューがダブルクリックされたときの処理です。
							// ダブルクリックしたアイテムが
							// 中央に表示されるようにタイムラインをスクロールします。

							auto nm = (NM_LISTVIEW*)l_param;

							// 項目のインデックスが範囲外の場合は何もしません。
							if ((size_t)nm->iItem >= finder.nodes.size()) break;
							if ((size_t)nm->iSubItem >= (size_t)list.k_column.k_max_size) break;

							// ノードを取得します。
							const auto& node = finder.nodes[nm->iItem];

							// タイムラインを水平方向にスクロールします。
							{
								auto left_frame = magi.exin.get_top_visible_frame();
								auto right_frame = (int32_t)magi.exin.x_to_frame(magi.exin.get_layer_width());

								auto frame = node->frame_begin;
								frame -= (right_frame - left_frame) / 2;
								magi.exin.set_top_visible_frame(frame);
							}

							// タイムラインを垂直方向にスクロールします。
							{
								auto layer = node->layer;
								layer -= (magi.exin.get_layer_visible_count() / 2);
								magi.exin.set_top_visible_layer(layer);
							}

							break;
						}
					case NM_RCLICK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, NM_RCLICK");

							// リストビューではない場合は何もしません。
							if (nmh->hwndFrom != listview) break;

							// リストビューが右クリックされたときの処理です。

							auto nm = (NMITEMACTIVATE*)l_param;

							// クリックされた行用のコンテキストメニューを表示します。
							show_context_menu(nm->iItem, nm->iSubItem);

							break;
						}
					case LVN_GETDISPINFO:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_GETDISPINFO");

							// リストビューではない場合は何もしません。
							if (nmh->hwndFrom != listview) break;

							// リストビュー項目の文字列をセットするときの処理です。

							auto nm = (NMLVDISPINFO*)l_param;

							// 項目テキストを設定する必要がある場合は
							if (nm->item.mask & LVIF_TEXT)
							{
								// 項目のインデックスが範囲外の場合は何もしません。
								if ((size_t)nm->item.iItem >= finder.nodes.size()) break;
								if ((size_t)nm->item.iSubItem >= (size_t)list.k_column.k_max_size) break;

								// 項目に対応するアイテム一覧を取得します。
								const auto& node = finder.nodes[nm->item.iItem];

								// 列に対応する文字列をセットします。
								nm->item.pszText = (LPWSTR)node->get_text(nm->item.iSubItem).c_str();
							}

							break;
						}
					case HDN_ENDDRAG:
					case HDN_ENDTRACK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, HDN_ENDDRAG or HDN_ENDTRACK");

							// ヘッダーコントロールをドラッグして
							// 列幅を変更したときの処理です。

							auto header = nmh->hwndFrom;
							auto nm = (NMHEADER*)l_param;

							// リストビューのヘッダーコントロールではない場合は何もしません。
							if (header != ListView_GetHeader(listview)) break;

							// 列幅が変更されたので、絞り込み用コンボボックスのレイアウトを更新します。
							recalc_layout(FALSE, TRUE);

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} addin_dialog;
}
