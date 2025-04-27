#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはカスタムオブジェクトのリストダイアログです。
	//
	inline struct ListDialog : StdAddinDialog<IDD_CUSTOM_OBJECT_LIST>
	{
		inline static constexpr struct Message {
			inline static constexpr auto c_recalc_layout = WM_APP + 2025;
		} c_message;

		inline static constexpr struct Filter {
			inline static constexpr auto c_control_id_front = IDC_SCRIPT_NAME;
			inline static constexpr auto c_control_id_back = c_control_id_front + agit.c_column.c_max_size - 1;
		} c_filter;

		//
		// このクラスはインプレース編集用の変数を保持します。
		//
		struct InplaceEdit {
			HWND editbox = nullptr;
			struct State {
				int item = -1;
				int sub_item = -1;
				std::shared_ptr<Agit::List::Node> node;
			} state;
		} inplace_edit;

		//
		// リストビューを初期化します。
		//
		BOOL init_list_view()
		{
			MY_TRACE_FUNC("");

			auto list_view = ctrl(IDC_LIST);
#if 0
			// リストビューをサブクラス化して
			// ウィンドウメッセージをハンドルできるようにします。
			::SetWindowSubclass(list_view,
				[](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data) -> LRESULT {
					return ((ListDialog*)ref_data)->on_list_view(hwnd, message, wParam, lParam, subclass_id, ref_data);
				}, (UINT_PTR)this, (DWORD_PTR)this);
#endif
			// インプレース編集用のエディットボックスをサブクラス化して
			// ウィンドウメッセージをハンドルできるようにします。
			::SetWindowSubclass(inplace_edit.editbox = ::GetDlgItem(*this, IDC_INPLACE_EDITBOX),
				[](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data) -> LRESULT {
					return ((ListDialog*)ref_data)->on_editbox_for_inplace_edit(hwnd, message, wParam, lParam, subclass_id, ref_data);
				}, (UINT_PTR)this, (DWORD_PTR)this);

			// すべての絞り込み用コンボボックスを走査します。
			for (auto i = 0; i < agit.c_column.c_max_size; i++)
			{
				// コンボボックスを取得します。
				auto combobox = ctrl(c_filter.c_control_id_front + i);

				// コンボボックス情報を取得します。
				COMBOBOXINFO cbi = { sizeof(cbi) };
				::GetComboBoxInfo(combobox, &cbi);

				// コンボボックス内のエディットボックスをサブクラス化して
				// ウィンドウメッセージをハンドルできるようにします。
				::SetWindowSubclass(cbi.hwndItem,
					[](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data) -> LRESULT {
						return ((ListDialog*)ref_data)->on_editbox_for_filtering(hwnd, message, wParam, lParam, subclass_id, ref_data);
					}, (UINT_PTR)this, (DWORD_PTR)this);
			}

			my::modify_style(list_view, 0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
			my::modify_style(inplace_edit.editbox, 0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

			ListView_SetExtendedListViewStyle(
				list_view,
				LVS_EX_DOUBLEBUFFER |
				LVS_EX_FULLROWSELECT |
				LVS_EX_GRIDLINES |
				LVS_EX_HEADERDRAGDROP |
				LVS_EX_HEADERINALLVIEWS |
				LVS_EX_INFOTIP |
				LVS_EX_LABELTIP |
//				LVS_EX_ONECLICKACTIVATE |
//				LVS_EX_UNDERLINEHOT |
				0);

			for (auto i = 0; i < agit.c_column.c_max_size; i++)
			{
				const auto& data = agit.c_column.c_datas[i];

				lv::insert_column(list_view, i, data.text, data.width);
			}

			return TRUE;
		}

		//
		// リストビューを更新します。
		//
		BOOL update_list_view()
		{
			MY_TRACE_FUNC("");

			auto list_view = ctrl(IDC_LIST);

			return update_item_count(list_view);
		}

		//
		// リストビューの項目数を更新します。
		//
		BOOL update_item_count(HWND list_view)
		{
			MY_TRACE_FUNC("");

			// カスタムオブジェクトのフィルタ済みコレクションを取得します。
			const auto& vec = agit.list.collection.filtered.vec;

			// カスタムオブジェクトの数をリストビューに適用します。
			ListView_SetItemCount(list_view, vec.size());

			return TRUE;
		}

		//
		// 絞り込み用コンボボックスを更新します。
		//
		BOOL update_filter_comboboxs()
		{
			MY_TRACE_FUNC("");

			// 絞り込み用テキストを格納するコンテナです。
			std::set<std::wstring> texts_array[agit.c_column.c_max_size];

			// コンテナに空文字列を追加します。
			for (auto i = 0; i < agit.c_column.c_max_size; i++)
				texts_array[i].emplace(L"");


			// リストノードが保持する文字列を
			// コンテナに絞り込み用テキストを格納していきます。
			for (const auto& node : agit.list.collection.raw.vec)
			{
				for (auto i = 0; i < agit.c_column.c_max_size; i++)
				{
					switch (i)
					{
					case agit.c_column.c_script_name:
					case agit.c_column.c_folder_name:
					case agit.c_column.c_file_name:
//					case agit.c_column.c_file_time:
					case agit.c_column.c_author:
//					case agit.c_column.c_desc:
					case agit.c_column.c_note:
						{
							texts_array[i].emplace(node->get_text(i));

							break;
						}
					case agit.c_column.c_tag:
						{
							auto& texts = texts_array[i];

							std::wstringstream stream(node->get_text(i));
							std::wstring text;

							while (std::getline(stream, text, L' '))
								texts.emplace(text);

							break;
						}
					}
				}
			}

			// コンテナに格納されたテキストをコンボボックスに追加します。
			for (auto i = 0; i < agit.c_column.c_max_size; i++)
			{
				auto combobox = ctrl(c_filter.c_control_id_front + i);

				my::combobox::clear(combobox);

				for (const auto& text : texts_array[i])
					my::combobox::add_text(combobox, text);
			}

			return TRUE;
		}

		//
		// 列のテキストを更新します。
		//
		BOOL update_column_text(HWND list_view, int32_t col_index)
		{
			MY_TRACE_FUNC("{/}", col_index);

			auto text = (std::wstring)agit.c_column.c_datas[col_index].text;

			int32_t index = 1;
			for (const auto& node : agit.sort.nodes)
			{
				if (node->col_index == col_index)
				{
					text += node->ascendant ? L"▽" : L"△";
					text += my::format(L"{/}", index);

					break;
				}

				index++;
			}

			LVCOLUMN col = {
				.mask = LVCF_TEXT,
				.pszText = text.data(),
			};

			return ListView_SetColumn(list_view, col_index, &col);
		}

		//
		// 列のフォーマットを設定します。
		//
		void set_column_format(HWND list_view, int32_t index, int32_t remove, int32_t add)
		{
			auto header = ListView_GetHeader(list_view);

			HDITEM item = {
				.mask = HDI_FORMAT,
			};
			Header_GetItem(header, index, &item);

			item.fmt &= ~remove;
			item.fmt |= add;

			Header_SetItem(header, index, &item);
		}

		//
		// リストビューの項目をソートします。
		//
		BOOL sort_items(HWND list_view, int32_t col_index)
		{
			MY_TRACE_FUNC("{/}", col_index);

			// ソート用ノードを追加します。
			agit.sort.add(col_index);

			// リストをソートします。
			agit.sort_list();

			// リストをフィルタします。
			agit.filter_list();

			// ヘッダーのテキストを更新します。
			for (int32_t i = 0; i < agit.c_column.c_max_size; i++)
				update_column_text(list_view, i);

			// リストビューを更新します。
			update_item_count(list_view);

			return TRUE;
		}

		//
		// リストビューのコンテキストメニューを表示します。
		//
		BOOL show_context_menu(HWND list_view, int32_t index, int32_t sub_item)
		{
			MY_TRACE_FUNC("{/}, {/}", index, sub_item);

			// カスタムオブジェクトのフィルタ済みコレクションを取得します。
			const auto& vec = agit.list.collection.filtered.vec;

			// インデックスが無効の場合は何もしません。
			if ((size_t)index >= vec.size()) return FALSE;

			// カスタムオブジェクトを取得します。
			const auto& custom_object = vec[index];

			// カスタムオブジェクトが導入済みかのフラグを取得します。
			auto is_installed = custom_object->is_installed();

			// ポップアップメニューのアイテムIDです。
			constexpr uint32_t c_install = 1000;
			constexpr uint32_t c_uninstall = 1001;
			constexpr uint32_t c_open_guide = 1010;
			constexpr uint32_t c_open_script_file = 1020;
			constexpr uint32_t c_open_script_folder = 1021;
			constexpr uint32_t c_show_option_dialog = 1030;
			constexpr uint32_t c_refresh = 1040;

			// ポップアップメニューを作成します。
			my::menu::unique_ptr<> menu(::CreatePopupMenu());

			// c_installを追加します。
			::AppendMenu(menu.get(), MF_STRING, c_install, _T("導入する"));
			if (is_installed || custom_object->distributor.empty()) ::EnableMenuItem(menu.get(), c_install, MF_GRAYED);
#if 0
			// c_uninstallを追加します。
			::AppendMenu(menu.get(), MF_STRING, c_uninstall, _T("削除する"));
			if (!is_installed) ::EnableMenuItem(menu.get(), c_uninstall, MF_GRAYED);
#endif
			// c_open_guideを追加します。
			::AppendMenu(menu.get(), MF_STRING, c_open_guide, _T("ググる"));

			// c_open_script_fileを追加します。
			::AppendMenu(menu.get(), MF_STRING, c_open_script_file, _T("スクリプトファイルを開く"));
			if (!is_installed) ::EnableMenuItem(menu.get(), c_open_script_file, MF_GRAYED);

			// c_open_script_folderを追加します。
			::AppendMenu(menu.get(), MF_STRING, c_open_script_folder, _T("スクリプトフォルダを開く"));
			if (!is_installed) ::EnableMenuItem(menu.get(), c_open_script_folder, MF_GRAYED);
#if 0
			// セパレータを追加します。
			::AppendMenu(menu.get(), MF_SEPARATOR, 0, nullptr);

			// オプションダイアログを表示を追加します。
			::AppendMenu(menu.get(), MF_STRING, c_show_option_dialog, _T("スクリプトのオプション"));
#endif
			// セパレータを追加します。
			::AppendMenu(menu.get(), MF_SEPARATOR, 0, nullptr);

			// カスタムオブジェクトをリフレッシュします。
			::AppendMenu(menu.get(), MF_STRING, c_refresh, _T("リフレッシュ"));

			// ポップアップメニューを表示します。
			auto point = my::get_cursor_pos();
			auto id = ::TrackPopupMenuEx(menu.get(),
				TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, list_view, nullptr);
			if (!id) return FALSE;

			switch (id)
			{
			case c_install: action->install(custom_object); break;
			case c_uninstall: action->uninstall(custom_object); break;
			case c_open_script_file: action->open_script_file(custom_object); break;
			case c_open_script_folder: action->open_script_folder(custom_object); break;
			case c_open_guide: action->open_guide(custom_object); break;
			case c_show_option_dialog: action->show_option_dialog(custom_object); break;
			case c_refresh: action->refresh(); break;
			}

			return TRUE;
		}

		//
		// インプレース編集中の場合はTRUEを返します。
		//
		BOOL is_inplace_editing(HWND list_view)
		{
			return ::IsWindowVisible(inplace_edit.editbox);
		}

		BOOL reset_inplace_edit(HWND list_view)
		{
			// インプレース編集用エディットボックスを非表示にします。
			::ShowWindow(inplace_edit.editbox, SW_HIDE);

			// インプレース編集の状態をリセットします。
			inplace_edit.state = {};

			return TRUE;
		}

		//
		// インプレース編集を開始します。
		//
		BOOL begin_inplace_edit(HWND list_view, int32_t index, int32_t sub_item)
		{
			MY_TRACE_FUNC("{/}, {/}", index, sub_item);

			// インプレース編集中の場合は
			if (is_inplace_editing(list_view))
			{
				// 既存のインプレース編集を終了します。
				end_inplace_edit(list_view);
			}

			// カスタムオブジェクトのフィルタ済みコレクションを取得します。
			const auto& vec = agit.list.collection.filtered.vec;

			// インデックスが無効の場合は何もしません。
			if ((size_t)index >= vec.size()) return FALSE;

			// インプレース編集の状態を設定します。
			inplace_edit.state = { index, sub_item, vec[index] };

			// サブ項目の矩形を取得します。
			auto rc = RECT {}; rc.top = sub_item;
			SendMessage(list_view, LVM_GETSUBITEMRECT, (WPARAM)index, (LPARAM)&rc);
			my::map_window_points(list_view, *this, &rc);

			// エディットボックスの位置を変更します。
			::SetWindowPos(inplace_edit.editbox, HWND_TOP,
				rc.left, rc.top, my::get_width(rc), my::get_height(rc), 0);

			// エディットボックスを表示します。
			::ShowWindow(inplace_edit.editbox, SW_SHOW);

			// エディットボックスにフォーカスを与えます。
			::SetFocus(inplace_edit.editbox);

			// エディットボックスのテキストを変更します。
			::SetWindowText(inplace_edit.editbox, inplace_edit.state.node->note.c_str());

			// エディットボックスのテキストを選択状態にします。
			::SendMessage(inplace_edit.editbox, EM_SETSEL, 0, -1);

			return TRUE;
		}

		//
		// インプレース編集を終了します。
		//
		BOOL end_inplace_edit(HWND list_view)
		{
			MY_TRACE_FUNC("");

			// インプレース編集中ではない場合はFALSEを返します。
			if (!is_inplace_editing(list_view)) return FALSE;

			// エディットボックスに入力されているテキストを取得します。
			auto text = my::get_window_text(inplace_edit.editbox);

			// カスタムオブジェクトのノートを変更します。
			inplace_edit.state.node->note = text;

			// サブ項目のテキストを更新します。
			ListView_SetItemText(list_view, inplace_edit.state.item, inplace_edit.state.sub_item, text.data());

			// インプレース編集の状態をリセットします。
			return reset_inplace_edit(list_view);
		}

		//
		// インプレース編集を中止します。
		//
		BOOL cancel_inplace_edit(HWND list_view)
		{
			MY_TRACE_FUNC("");

			// インプレース編集中ではない場合はFALSEを返します。
			if (!is_inplace_editing(list_view)) return FALSE;

			// インプレース編集の状態をリセットします。
			return reset_inplace_edit(list_view);
		}

		//
		// ドラッグを開始します。
		//
		BOOL begin_drag(HWND list_view, int32_t index, int32_t sub_item)
		{
			MY_TRACE_FUNC("{/}, {/}", index, sub_item);

			// カスタムオブジェクトのフィルタ済みコレクションを取得します。
			const auto& vec = agit.list.collection.filtered.vec;

			// インデックスが無効の場合は何もしません。
			if ((size_t)index >= vec.size()) return FALSE;

			// カスタムオブジェクトを取得します。
			const auto& custom_object = vec[index];

			// 導入済みではない場合は何もしません。
			if (!custom_object->is_installed()) return FALSE;

			// ドラッグを開始します。
			return action->begin_drag(custom_object);
		}

		//
		// ダイアログレイアウトを更新します。
		//
		BOOL recalc_layout()
		{
			MY_TRACE_FUNC("");

			// リストビューを取得します
			auto list_view = ctrl(IDC_LIST);

			// ダイアログのクライアント矩形を取得します。
			auto client_rc = my::get_client_rect(*this);

			// ダイアログレイアウトの基準となるサイズを取得します。
			auto base_rc = my::get_window_rect(ctrl(c_filter.c_control_id_front));
			auto base_w = my::get_width(base_rc);
			auto base_h = my::get_height(base_rc);

			my::DeferWindowPos dwp(1 + agit.c_column.c_max_size);

			// 絞り込み用コンボボックスのウィンドウ位置を変更します。
			{
				auto rc = client_rc;
				rc.bottom = rc.top + base_h;
				auto x = client_rc.left;

				int order_array[agit.c_column.c_max_size] = {};
				ListView_GetColumnOrderArray(list_view, std::size(order_array), order_array);

				for (size_t i = 0; i < std::size(order_array); i++)
				{
					auto order = order_array[i];

					LVCOLUMN column = {};
					column.mask = LVCF_WIDTH | LVCF_ORDER | LVCF_SUBITEM;
					ListView_GetColumn(list_view, order, &column);

					auto combobox = ctrl(c_filter.c_control_id_front + order);

					rc.left = x; x += column.cx;
					rc.right = x;

					dwp.set_window_pos(combobox, nullptr, &rc, SWP_NOZORDER | SWP_NOACTIVATE);

					::SendMessage(combobox, CB_SETDROPPEDWIDTH, std::max(my::get_width(rc), 200), 0);
				}
			}

			// リストビューのウィンドウ位置を変更します。
			{
				auto rc = client_rc;
				rc.top += base_h;

				dwp.set_window_pos(list_view, nullptr, &rc, SWP_NOZORDER | SWP_NOACTIVATE);
			}

			return TRUE;
		}

		//
		// フィルタ文字列をコンボボックスに追加します。
		//
		BOOL insert_filter_text(HWND combobox, const std::wstring& text)
		{
			MY_TRACE_FUNC("{/hex}, {/}", combobox, text);

			if (text.empty()) return FALSE;

			// コンボボックス内の文字列を検索します。
			auto index = my::combobox::find_string(combobox, text);
			if (index != CB_ERR)
			{
				// コンボボックス内の文字列を削除します。
				my::combobox::erase_text(combobox, index);
			}

			// コンボボックスの先頭に文字列を追加します。
			my::combobox::insert_text(combobox, text, 0);

			return TRUE;
		}

		//
		// フィルタ文字列をコンボボックスから削除します。
		//
		BOOL erase_filter_text(HWND combobox, const std::wstring& text)
		{
			MY_TRACE_FUNC("{/hex}, {/}", combobox, text);

			if (text.empty()) return FALSE;

			// コンボボックスの文字列を消去します。
			::SetWindowText(combobox, L"");

			// コンボボックス内の文字列を検索します。
			auto index = my::combobox::find_string(combobox, text);
			if (index == CB_ERR) return FALSE;

			// コンボボックス内の文字列を削除します。
			my::combobox::erase_text(combobox, index);

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

			init_list_view();
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			if (control_id >= c_filter.c_control_id_front &&
				control_id <= c_filter.c_control_id_back)
			{
				if (code == CBN_SELCHANGE)
				{
					// 絞り込み用テキストをセットします。
					agit.filter.texts[control_id - c_filter.c_control_id_front] = my::combobox::get_text(control, -1);

					// フィルタが変更された可能性があるのでリストを更新します。
					action->filter_list();
				}
			}
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
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {/hex}, {/hex}", wParam, lParam);

					// ダイアログレイアウトを更新します。
					recalc_layout();

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_KEYDOWN:
				{
					MY_TRACE_FUNC("WM_KEYDOWN, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_CHAR:
				{
					MY_TRACE_FUNC("WM_CHAR, {/hex}, {/hex}", wParam, lParam);

					break;
				}
			case WM_NOTIFY:
				{
//					MY_TRACE_FUNC("WM_NOTIFY, {/hex}, {/hex}", wParam, lParam);

					auto nmh = (NMHDR*)lParam;

					switch (nmh->code)
					{
					case LVN_ITEMCHANGED:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_ITEMCHANGED");

							if (nmh->idFrom != IDC_LIST) break;

							// 項目が変更されたときの処理です。

							break;
						}
					case LVN_COLUMNCLICK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_COLUMNCLICK");

							if (nmh->idFrom != IDC_LIST) break;

							// カラムがクリックされたときの処理です。
							// クリックされたカラムのテキストを更新します。
							// クリックされたカラムを基準にリストビューをソートします。

							auto list_view = nmh->hwndFrom;
							auto nm = (NM_LISTVIEW*)lParam;

							sort_items(list_view, nm->iSubItem);

							break;
						}
					case NM_RCLICK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, NM_RCLICK");

							if (nmh->idFrom != IDC_LIST) break;

							// リストビューが右クリックされたときの処理です。
							// クリックされた行用のコンテキストメニューを表示します。

							auto list_view = nmh->hwndFrom;
							auto nm = (NMITEMACTIVATE*)lParam;

							show_context_menu(list_view, nm->iItem, nm->iSubItem);

							break;
						}
					case NM_DBLCLK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, NM_DBLCLK");

							if (nmh->idFrom != IDC_LIST) break;

							// リストビューがダブルクリックされたときの処理です。
							// (サブ)項目のインプレース編集を開始します。

							auto list_view = nmh->hwndFrom;
							auto nm = (NM_LISTVIEW*)lParam;

							if (nm->iSubItem == agit.c_column.c_note)
							{
								begin_inplace_edit(list_view, nm->iItem, nm->iSubItem);
							}

							break;
						}
					case NM_RETURN:
						{
							MY_TRACE_FUNC("WM_NOTIFY, NM_RETURN");

							if (nmh->idFrom != IDC_LIST) break;

							// リストビューでキー操作がされたときの処理です。
							// (サブ)項目のインプレース編集を開始します。

							auto list_view = nmh->hwndFrom;

							auto item = ListView_GetSelectionMark(list_view);

							begin_inplace_edit(list_view, item, agit.c_column.c_note);

							break;
						}
					case LVN_BEGINDRAG:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_BEGINDRAG");

							if (nmh->idFrom != IDC_LIST) break;

							// リストビューでドラッグが開始されたときの処理です。

							auto list_view = nmh->hwndFrom;
							auto nm = (NM_LISTVIEW*)lParam;

							begin_drag(list_view, nm->iItem, nm->iSubItem);

							break;
						}
					case LVN_GETDISPINFO:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_GETDISPINFO");

							if (nmh->idFrom != IDC_LIST) break;

							// リストビュー項目の文字列をセットするときの処理です。

							auto list_view = nmh->hwndFrom;
							auto nm = (NMLVDISPINFO*)lParam;

							// カスタムオブジェクトのフィルタ済みコレクションを取得します。
							const auto& vec = agit.list.collection.filtered.vec;

							// 項目のインデックスが範囲外の場合は何もしません。
							if ((size_t)nm->item.iItem >= vec.size()) break;

							// 項目に対応するカスタムオブジェクトを取得します。
							const auto& custom_object = vec[nm->item.iItem];

							// 列に対応する文字列を取得し、セットします。
							nm->item.pszText = custom_object->get_text(nm->item.iSubItem).data();

							break;
						}
					case HDN_ENDDRAG:
					case HDN_ENDTRACK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, HDN_ENDDRAG or HDN_ENDTRACK");

							auto list_view = ctrl(IDC_LIST);
							auto header = nmh->hwndFrom;
							auto nm = (NMHEADER*)lParam;

							// リストビューのヘッダーコントロールではない場合は何もしません。
							if (header != ListView_GetHeader(list_view))
								break;

							::PostMessage(*this, c_message.c_recalc_layout, 0, 0);

							break;
						}
					}

					break;
				}
			case c_message.c_recalc_layout:
				{
					MY_TRACE_FUNC("c_message.c_recalc_layout");

					// ダイアログレイアウトを更新します。
					recalc_layout();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// リストビューのメッセージをハンドルします。
		//
		LRESULT on_list_view(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data)
		{
			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		//
		// インプレース編集用のエディットボックスのメッセージをハンドルします。
		//
		LRESULT on_editbox_for_inplace_edit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data)
		{
			switch (message)
			{
			case WM_KILLFOCUS:
				{
					MY_TRACE_FUNC("WM_KILLFOCUS, {/hex}, {/hex}", wParam, lParam);

					auto list_view = ctrl(IDC_LIST);

					// エディットボックスがフォーカスを失ったので
					// インプレース編集を終了します。
					end_inplace_edit(list_view);

					break;
				}
			case WM_CHAR:
				{
					MY_TRACE_FUNC("WM_CHAR, {/hex}, {/hex}", wParam, lParam);

					auto list_view = ctrl(IDC_LIST);

					switch (wParam)
					{
					case VK_RETURN:
						{
							// リターンキーが押されたので
							// インプレース編集を終了します。
							end_inplace_edit(list_view);

							return 0;
						}
					case VK_ESCAPE:
						{
							// エスケープキーが押されたので
							// インプレース編集をキャンセルします。
							cancel_inplace_edit(list_view);

							return 0;
						}
					}

					break;
				}
			}

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}

		//
		// 絞り込み用コンボボックスのエディットボックスのメッセージをハンドルします。
		//
		LRESULT on_editbox_for_filtering(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data)
		{
			switch (message)
			{
			case WM_CHAR:
				{
					MY_TRACE_FUNC("WM_CHAR, {/hex}, {/hex}", wParam, lParam);

					// コンボボックスを取得します。
					auto combobox = ::GetParent(hwnd);

					// コンボボックスのコントロールIDを取得します。
					auto control_id = ::GetDlgCtrlID(combobox);
					auto column_index = control_id - c_filter.c_control_id_front;
					if (column_index >= agit.c_column.c_max_size) break;

					switch (wParam)
					{
					case VK_RETURN:
						{
							// エディットボックスからテキストを取得します。
							auto text = my::get_window_text(hwnd);
#if 1
							// テキストが有効の場合は
							if (text.length())
							{
								// コンボボックス内にテキストが存在しない場合は
								if (auto index = my::combobox::find_string(combobox, text); index == CB_ERR)
								{
									// コンボボックスの先頭に文字列を追加します。
									my::combobox::insert_text(combobox, text, 0);
								}
							}

							// 絞り込み用テキストをセットします。
							agit.filter.texts[column_index] = text;
#else
							// シフトキーが押されていない場合は
							if (::GetKeyState(VK_SHIFT) >= 0)
							{
								// 絞り込み用テキストを追加します。
								insert_filter_text(combobox, text);

								// 絞り込み用テキストをセットします。
								agit.filter.texts[column_index] = text;
							}
							// シフトキーが押されている場合は
							else
							{
								// 絞り込み用テキストを削除します。
								erase_filter_text(combobox, text);

								// 絞り込み用テキストをリセットします。
								agit.filter.texts[column_index] = L"";
							}
#endif
							// フィルタが変更された可能性があるのでリストを更新します。
							action->filter_list();

							return 0;
						}
					case VK_TAB:
						{
							// シフトキーが押されていない場合は
							if (::GetKeyState(VK_SHIFT) >= 0)
							{
								// 次のコンボボックスにフォーカスを移します。
								::SetFocus(ctrl((control_id == c_filter.c_control_id_back) ? c_filter.c_control_id_front : (control_id + 1)));
							}
							// シフトキーが押されている場合は
							else
							{
								// 前のコンボボックスにフォーカスを移します。
								::SetFocus(ctrl((control_id == c_filter.c_control_id_front) ? c_filter.c_control_id_back : (control_id - 1)));
							}

							return 0;
						}
					}

					break;
				}
			}

			return ::DefSubclassProc(hwnd, message, wParam, lParam);
		}
	} list_dialog;
}
