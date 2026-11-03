#pragma once

namespace apn::item_list
{
	//
	// このクラスは絞り込み用コンボボックスを管理します。
	//
	inline struct combobox_manager_t
	{
		//
		// ダイアログです。
		//
		HWND dialog = {};

		//
		// 先頭のコンボボックスのIDです。
		//
		UINT first_id = {};

		//
		// このクラスは絞り込み用コンボボックス内のエディットボックスです。
		//
		struct edit_t : my::Window
		{
			//
			// 絞り込み用コンボボックス内のエディットボックスのウィンドウプロシージャです。
			//
			virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
			{
				//
				// この関数は、リストを絞り込みます。
				//
				constexpr auto apply_finder = [](HWND hwnd)
				{
					// コンボボックスを取得します。
					auto combobox = ::GetParent(hwnd);

					// コンボボックスのコントロールIDを取得します。
					auto control_id = ::GetDlgCtrlID(combobox);
					auto column_index = control_id - combobox_manager.first_id;
					if (column_index >= list.k_column.k_max_size) return;

					// エディットボックスからテキストを取得します。
					auto text = my::get_window_text(hwnd);

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
					finder.texts[column_index] = text;

					// 絞り込みを適用します。
					app->apply_finder();
				};

				switch (message)
				{
				case WM_KILLFOCUS:
					{
						MY_TRACE_FUNC("WM_KILLFOCUS, {/hex}, {/hex}", w_param, l_param);

						// リストを絞り込みます。
						apply_finder(hwnd);

						break;
					}
				case WM_CHAR:
					{
						MY_TRACE_FUNC("WM_CHAR, {/hex}, {/hex}", w_param, l_param);

						switch (w_param)
						{
						case VK_RETURN:
							{
								// リストを絞り込みます。
								apply_finder(hwnd);

								return 0;
							}
						case VK_TAB:
							{
								// コンボボックスを取得します。
								auto combobox = ::GetParent(hwnd);

								// コンボボックスのコントロールIDを取得します。
								auto control_id = ::GetDlgCtrlID(combobox);

								auto first_id = combobox_manager.first_id;
								auto last_id = combobox_manager.first_id + list.k_column.k_max_size;

								// シフトキーが押されていない場合は
								if (::GetKeyState(VK_SHIFT) >= 0)
								{
									// 次のコンボボックスにフォーカスを移します。
									::SetFocus(::GetDlgItem(combobox_manager.dialog, (control_id == last_id) ? last_id : (control_id + 1)));
								}
								// シフトキーが押されている場合は
								else
								{
									// 前のコンボボックスにフォーカスを移します。
									::SetFocus(::GetDlgItem(combobox_manager.dialog, (control_id == last_id) ? last_id : (control_id - 1)));
								}

								return 0;
							}
						}

						break;
					}
				}

				return __super::on_wnd_proc(hwnd, message, w_param, l_param);
			}
		} edits[list.k_column.k_max_size];

		//
		// 絞り込み用コンボボックスを初期化します。
		//
		BOOL init(HWND dialog, UINT first_id)
		{
			MY_TRACE_FUNC("");

			this->dialog = dialog;
			this->first_id = first_id;

			// すべての絞り込み用コンボボックスを走査します。
			for (auto i = 0; i < list.k_column.k_max_size; i++)
			{
				// 絞り込み用コンボボックスを取得します。
				auto combobox = ::GetDlgItem(dialog, first_id + i);

				// コンボボックス情報を取得します。
				COMBOBOXINFO cbi = { sizeof(cbi) };
				::GetComboBoxInfo(combobox, &cbi);

				// 絞り込み用コンボボックス内のエディットボックスをサブクラス化します。
				edits[i].subclass(cbi.hwndItem);
			}

			return TRUE;
		}

		//
		// 絞り込み用コンボボックスをリフレッシュします。
		//
		BOOL refresh()
		{
			MY_TRACE_FUNC("");

			// 行を走査します。
			for (auto i = 0; i < list.k_column.k_max_size; i++)
			{
				// 絞り込み用テキストを格納するコンテナです。
				// std::setなので、重複テキストは除外されます。
				std::set<std::wstring> texts;

				// コンテナに空文字列を追加します。
				texts.emplace(L"");

				// コンテナに絞り込み用テキストを追加します。
				for (const auto& node : list.nodes)
				{
					// 文字列ストリームを作成します。
					std::wstringstream stream(node->get_text(i));

					// 空白で区切ってコンテナに追加します。
					std::wstring item;
					while (std::getline(stream, item, L','))
						texts.emplace(item);
				}

				// 絞り込み用コンボボックスを取得します。
				auto combobox = ::GetDlgItem(dialog, first_id + i);

				// 現在の絞り込み用テキストを取得しておきます。
				auto cur_text = my::get_window_text(combobox);

				// 絞り込み用コンボボックスをリセットします。
				my::combobox::clear(combobox);

				// コンテナに格納されたテキストをコンボボックスに追加します。
				for (const auto& text : texts)
					my::combobox::add_text(combobox, text);

				// 絞り込み用テキストを復元します。
				::SetWindowText(combobox, cur_text.c_str());
			}

			return TRUE;
		}

		//
		// 絞り込み用コンボボックスをリセットします。
		//
		BOOL reset()
		{
			MY_TRACE_FUNC("");

			// 行を走査します。
			for (auto i = 0; i < list.k_column.k_max_size; i++)
			{
				// 絞り込み用コンボボックスを取得します。
				auto combobox = ::GetDlgItem(dialog, first_id + i);

				// 絞り込み用コンボボックスをリセットします。
				::SetWindowText(combobox, L"");

				// 絞り込み用テキストをリセットします。
				finder.texts[i].clear();
			}

			return TRUE;
		}
	} combobox_manager;
}
