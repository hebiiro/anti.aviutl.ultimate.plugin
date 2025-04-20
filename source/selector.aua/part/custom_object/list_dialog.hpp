#pragma once

namespace apn::selector::custom_object
{
	//
	// このクラスはカスタムオブジェクトのリストダイアログです。
	//
	inline struct ListDialog : StdAddinDialog<IDD_CUSTOM_OBJECT_LIST>
	{
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

			// インプレース編集用のエディットボックスをサブクラス化して
			// ウィンドウメッセージをハンドルできるようにします。
			::SetWindowSubclass(inplace_edit.editbox = ::GetDlgItem(*this, IDC_INPLACE_EDITBOX),
				[](HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data) -> LRESULT {
					return ((ListDialog*)ref_data)->on_inplace_editbox(
						hwnd, message, wParam, lParam, subclass_id, ref_data);
				}, (UINT_PTR)&inplace_edit.editbox, (DWORD_PTR)this);

			auto list_view = ctrl(IDC_LIST);

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
				node->add_pane(c_axis.c_horz, c_align.c_left, full, {}, ctrl(IDC_LIST));
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
			case WM_NOTIFY:
				{
//					MY_TRACE_FUNC("WM_NOTIFY, {/hex}, {/hex}", wParam, lParam);

					auto header = (NMHDR*)lParam;
					if (header->idFrom != IDC_LIST) break;

					switch (header->code)
					{
					case LVN_ITEMCHANGED:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_ITEMCHANGED");

							// 項目が変更されたときの処理です。

							break;
						}
					case LVN_COLUMNCLICK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_COLUMNCLICK");

							// カラムがクリックされたときの処理です。
							// クリックされたカラムのテキストを更新します。
							// クリックされたカラムを基準にリストビューをソートします。

							auto list_view = header->hwndFrom;
							auto nm = (NM_LISTVIEW*)lParam;

							sort_items(list_view, nm->iSubItem);

							break;
						}
					case NM_RCLICK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, NM_RCLICK");

							// リストビューが右クリックされたときの処理です。
							// クリックされた行用のコンテキストメニューを表示します。

							auto list_view = header->hwndFrom;
							auto nm = (NMITEMACTIVATE*)lParam;

							show_context_menu(list_view, nm->iItem, nm->iSubItem);

							break;
						}
					case NM_DBLCLK:
						{
							MY_TRACE_FUNC("WM_NOTIFY, NM_DBLCLK");

							// リストビューがダブルクリックされたときの処理です。
							// (サブ)項目のインプレース編集を開始します。

							auto list_view = header->hwndFrom;
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

							// リストビューでキー操作がされたときの処理です。
							// (サブ)項目のインプレース編集を開始します。

							auto list_view = header->hwndFrom;

							auto item = ListView_GetSelectionMark(list_view);

							begin_inplace_edit(list_view, item, agit.c_column.c_note);

							break;
						}
					case LVN_BEGINDRAG:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_BEGINDRAG");

							// リストビューでドラッグが開始されたときの処理です。

							auto list_view = header->hwndFrom;
							auto nm = (NM_LISTVIEW*)lParam;

							begin_drag(list_view, nm->iItem, nm->iSubItem);

							break;
						}
					case LVN_GETDISPINFO:
						{
							MY_TRACE_FUNC("WM_NOTIFY, LVN_GETDISPINFO");

							auto list_view = header->hwndFrom;
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
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// インプレース編集用のエディットボックスのメッセージをハンドルします。
		//
		LRESULT on_inplace_editbox(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR subclass_id, DWORD_PTR ref_data)
		{
			switch (message)
			{
			case WM_KILLFOCUS:
				{
					MY_TRACE_FUNC("WM_KILLFOCUS, {/hex}, {/hex}", wParam, lParam);

					auto list_view = ctrl(IDC_LIST);

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
							end_inplace_edit(list_view);

							return 0;
						}
					case VK_ESCAPE:
						{
							cancel_inplace_edit(list_view);

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
