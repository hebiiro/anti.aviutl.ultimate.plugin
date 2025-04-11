#pragma once

namespace apn::font_tree
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
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

			// フォントを取得します。
			AviUtl::SysInfo si = {};
			magi.fp->exfunc->get_sys_info(nullptr, &si);

			// お気入りコントロールを初期化します。
			favorite_manager.subclass(ctrl(IDC_FAVORITE));
#if 0
			// これを実行しても何も変わりません。
			TreeView_SetExtendedStyle(favorite_manager,
				TVS_EX_DOUBLEBUFFER | TVS_EX_FADEINOUTEXPANDOS, 0);
#endif
			SetWindowFont(favorite_manager, si.hfont, TRUE);
			::SetWindowTheme(favorite_manager, L"Explorer", nullptr);

			// 履歴コントロールを初期化します。
			recent_manager.subclass(ctrl(IDC_RECENT));
			SetWindowFont(recent_manager, si.hfont, TRUE);
			::SetWindowTheme(recent_manager, L"Explorer", nullptr);

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto row = std::make_shared<RelativePos>(get_base_size() + margin_value * 2);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_RECENT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, full);
				node->add_pane(c_axis.c_horz, c_align.c_left, full, margin, ctrl(IDC_FAVORITE));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, id, control);

			switch (id)
			{
			case IDC_RECENT:
				{
					// 履歴コンボボックスの選択項目が変更された場合は
					if (code == CBN_SELCHANGE)
					{
						// 履歴コンボボックスで選択されているフォント名を取得します。
						auto font_name = cb::get_text(recent_manager, -1);

						// フォントコンボボックスを取得します。
						auto font_combobox = magi.exin.get_font_combobox();
						if (!font_combobox) break;

						// フォントコンボボックスの選択項目を切り替えます。
						cb::select_text(font_combobox, font_name.c_str());
					}

					break;
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
			case WM_NOTIFY:
				{
					switch (wParam)
					{
					case IDC_FAVORITE:
						{
							// このスコープのみで使用されるフラグです。
							// TRUEの場合はTVN_SELCHANGEDを処理しません。
							static auto ignore_sel_changed = FALSE;

							auto nm = (NMHDR*)lParam;

							switch (nm->code)
							{
							case NM_RCLICK:
								{
									MY_TRACE_FUNC("IDC_FAVORITE, NM_RCLICK");

									// ドロップハイライト要素が有効の場合は
									if (auto item_handle = TreeView_GetDropHilight(favorite_manager))
									{
										// ドロップハイライト要素を選択します。
										// ただし、フォントコンボボックスの選択項目は変更しないので
										// TVN_SELCHANGEDが処理されないようにフラグを立てておきます。
										ignore_sel_changed = TRUE;
										TreeView_SelectItem(favorite_manager, item_handle);
										ignore_sel_changed = FALSE;
									}

									favorite_manager.show_context_menu(my::get_cursor_pos());

									break;
								}
							case TVN_SELCHANGED:
								{
									MY_TRACE_FUNC("IDC_FAVORITE, TVN_SELCHANGED");

									if (ignore_sel_changed) // フラグが立っている場合は何もしません。
									{
										MY_TRACE("TVN_SELCHANGEDを無視します\n");

										break;
									}

									auto nm = (NMTREEVIEW*)lParam;

									// フォントコンボボックスを取得します。
									auto font_combobox = magi.exin.get_font_combobox();
									if (!font_combobox) break;

									// 選択要素に該当するノードを取得します。
									auto node = favorite_manager.get_node(nm->itemNew.hItem);
									if (!node || node->name.empty()) break;

									// フォントコンボボックスの選択項目を変更します。
									cb::select_text(font_combobox, node->name);

									break;
								}
							}

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
