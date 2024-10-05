#pragma once

namespace apn::dark
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

			get_combobox_string(IDC_SKIN, hive.skin_name);
			get_combobox_string(IDC_SCHEME, hive.scheme_name);
			get_check(IDC_DARK_MODE, hive.dark_mode);
			get_int(IDC_ELLIPSE, hive.ellipse);
			get_int(IDC_BORDER_WIDTH, hive.border_width);
			get_check(IDC_DRAW_SHADOW, hive.draw_shadow);
			get_check(IDC_AS_ROUND, hive.as_round);
			get_check(IDC_DRAW_BUTTON_EDGE, hive.draw_button_edge);
			get_check(IDC_DRAW_BORDER, hive.draw_border);
			get_check(IDC_AS_ZEBRA, hive.as_zebra);
			get_check(IDC_APPLY_FILE_DIALOG, hive.apply_file_dialog);
			get_check(IDC_FIX_DPI_SCALING, hive.fix_dpi_scaling);
			get_check(IDC_USE_LAYER_COLOR, hive.use_layer_color);
			get_check(IDC_USE_LAYER_COLOR_MULTI, hive.use_layer_color_multi);
			get_check(IDC_DONT_WRITE_BYTECODE, hive.dont_write_bytecode);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_combobox_string(IDC_SKIN, hive.skin_name);
			set_combobox_string(IDC_SCHEME, hive.scheme_name);
			set_check(IDC_DARK_MODE, hive.dark_mode);
			set_int(IDC_ELLIPSE, hive.ellipse);
			set_int(IDC_BORDER_WIDTH, hive.border_width);
			set_check(IDC_DRAW_SHADOW, hive.draw_shadow);
			set_check(IDC_AS_ROUND, hive.as_round);
			set_check(IDC_DRAW_BUTTON_EDGE, hive.draw_button_edge);
			set_check(IDC_DRAW_BORDER, hive.draw_border);
			set_check(IDC_AS_ZEBRA, hive.as_zebra);
			set_check(IDC_APPLY_FILE_DIALOG, hive.apply_file_dialog);
			set_check(IDC_FIX_DPI_SCALING, hive.fix_dpi_scaling);
			set_check(IDC_USE_LAYER_COLOR, hive.use_layer_color);
			set_check(IDC_USE_LAYER_COLOR_MULTI, hive.use_layer_color_multi);
			set_check(IDC_DONT_WRITE_BYTECODE, hive.dont_write_bytecode);
		}

		//
		// スキンリストコントロールを更新します。
		//
		void update_skin_list_control()
		{
			MY_TRACE_FUNC("");

			::SendDlgItemMessageW(*this, IDC_SKIN, CB_RESETCONTENT, 0, 0);
			for (auto& skin : hive.skin_list)
				::SendDlgItemMessageW(*this, IDC_SKIN, CB_ADDSTRING, 0, (LPARAM)skin->display_name.c_str());

			::SendDlgItemMessageW(*this, IDC_SCHEME, CB_RESETCONTENT, 0, 0);
			for (auto& scheme : hive.scheme_list)
				::SendDlgItemMessageW(*this, IDC_SCHEME, CB_ADDSTRING, 0, (LPARAM)scheme->display_name.c_str());
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto stat = std::make_shared<RelativePos>(base_size * 2 + margin_value * 2);
			auto editbox = std::make_shared<RelativePos>(base_size * 2);
			auto checkbox = std::make_shared<RelativePos>(base_size * 7);
			auto combobox = std::make_shared<RelativePos>(base_size * 7);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);
			auto rc = my::get_client_rect(ctrl(IDC_ELLIPSE_SPIN));
			auto spin_w = my::get_width(rc);
			auto spin = std::make_shared<RelativePos>(spin_w);
#if 0
			auto drop_width = base_size * 10;
			::SendDlgItemMessage(*this, IDC_SKIN, CB_SETDROPPEDWIDTH, drop_width, 0);
			::SendDlgItemMessage(*this, IDC_SCHEME, CB_SETDROPPEDWIDTH, drop_width, 0);
#endif
			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SKIN_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox, margin, ctrl(IDC_SKIN));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SCHEME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox, margin, ctrl(IDC_SCHEME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);

				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_ELLIPSE_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, nullptr);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_ELLIPSE_SPIN));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, full, {}, ctrl(IDC_ELLIPSE));
				}

				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_BORDER_WIDTH_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, nullptr);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_BORDER_WIDTH_SPIN));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, full, {}, ctrl(IDC_BORDER_WIDTH));
				}
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_DARK_MODE));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_DRAW_SHADOW));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_AS_ROUND));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_DRAW_BUTTON_EDGE));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_DRAW_BORDER));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_AS_ZEBRA));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_APPLY_FILE_DIALOG));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_FIX_DPI_SCALING));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_USE_LAYER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_USE_LAYER_COLOR_MULTI));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_DONT_WRITE_BYTECODE));
			}
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT id, HWND control) override
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", code, id, control);

			switch (id)
			{
			// コンボボックスが変更されたときの処理です。
			case IDC_SKIN:
			case IDC_SCHEME:
				{
					if (code == CBN_SELCHANGE)
						app->on_change_controls();

					break;
				}
			// エディットボックスが変更されたときの処理です。
			case IDC_ELLIPSE:
			case IDC_BORDER_WIDTH:
				{
					if (code == EN_UPDATE)
						app->on_change_controls();

					break;
				}
			// その他のコントロールが変更されたときの処理です。
			case IDC_DARK_MODE:
			case IDC_DRAW_SHADOW:
			case IDC_AS_ROUND:
			case IDC_DRAW_BUTTON_EDGE:
			case IDC_DRAW_BORDER:
			case IDC_AS_ZEBRA:
			case IDC_APPLY_FILE_DIALOG:
			case IDC_FIX_DPI_SCALING:
			case IDC_USE_LAYER_COLOR:
			case IDC_USE_LAYER_COLOR_MULTI:
				{
					app->on_change_controls();

					break;
				}
			// カラーピッカーボタンが押されたときの処理です。
			case IDC_DARK_PRIMARY_FILL:
			case IDC_DARK_PRIMARY_BORDER:
			case IDC_DARK_PRIMARY_TEXT:
			case IDC_DARK_SECONDARY_FILL:
			case IDC_DARK_SECONDARY_BORDER:
			case IDC_DARK_SECONDARY_TEXT:
			case IDC_LIGHT_PRIMARY_FILL:
			case IDC_LIGHT_PRIMARY_BORDER:
			case IDC_LIGHT_PRIMARY_TEXT:
			case IDC_LIGHT_SECONDARY_FILL:
			case IDC_LIGHT_SECONDARY_BORDER:
			case IDC_LIGHT_SECONDARY_TEXT:
				{
					auto color = get_uint(id);

					CHOOSECOLOR cc { sizeof(cc) };
					cc.hwndOwner = hwnd;
					cc.lpCustColors = hive.custom_colors;
					cc.rgbResult = color;
					cc.Flags = CC_RGBINIT | CC_FULLOPEN;
					if (!::ChooseColor(&cc)) break;

					color = cc.rgbResult;

					set_uint(id, color);
					::InvalidateRect(control, nullptr, FALSE);

					app->update_skin();

					break;
				}
			}
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_DRAWITEM:
				{
					auto id = (UINT)wParam;

					switch (id)
					{
					// カラーピッカーボタンを描画します。
					case IDC_DARK_PRIMARY_FILL:
					case IDC_DARK_PRIMARY_BORDER:
					case IDC_DARK_PRIMARY_TEXT:
					case IDC_DARK_SECONDARY_FILL:
					case IDC_DARK_SECONDARY_BORDER:
					case IDC_DARK_SECONDARY_TEXT:
					case IDC_LIGHT_PRIMARY_FILL:
					case IDC_LIGHT_PRIMARY_BORDER:
					case IDC_LIGHT_PRIMARY_TEXT:
					case IDC_LIGHT_SECONDARY_FILL:
					case IDC_LIGHT_SECONDARY_BORDER:
					case IDC_LIGHT_SECONDARY_TEXT:
						{
							auto dis = (DRAWITEMSTRUCT*)lParam;
							auto color = get_uint(id);

							::FillRect(dis->hDC, &dis->rcItem,
								my::gdi::unique_ptr<HBRUSH>(::CreateSolidBrush(color)).get());

							return TRUE;
						}
					}

					break;
				}
			case WM_NOTIFY:
				{
					auto header = (NMHDR*)lParam;
					if (header->code == UDN_DELTAPOS)
					{
						auto nm = (NMUPDOWN*)header;
						auto edit_id = header->idFrom - 1;

						switch (edit_id)
						{
						case IDC_ELLIPSE:
						case IDC_BORDER_WIDTH:
							{
								auto value = get_int(edit_id);
								value += (nm->iDelta > 0) ? -1 : +1;
								value = std::clamp(value, 0, +100);
								set_int(edit_id, value);
								break;
							}
						}
					}

					return FALSE;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
