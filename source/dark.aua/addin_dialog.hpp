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
			get_int(IDC_SHADOW_DENSITY, hive.shadow_density);
			get_int(IDC_SCROLLBAR_REDUCTION, hive.scrollbar_reduction);
			get_check(IDC_DRAW_SHADOW, hive.draw_shadow);
			get_check(IDC_AS_ROUND, hive.as_round);
			get_check(IDC_DRAW_GRADATION, hive.draw_gradation);
			get_check(IDC_DRAW_BUTTON_EDGE, hive.draw_button_edge);
			get_check(IDC_DRAW_BORDER, hive.draw_border);
			get_check(IDC_AS_ZEBRA, hive.as_zebra);
			get_check(IDC_APPLY_FILE_DIALOG, hive.apply_file_dialog);
			get_check(IDC_FIX_DPI_SCALING, hive.fix_dpi_scaling);
			get_check(IDC_USE_LAYER_COLOR, hive.use_layer_color);
			get_check(IDC_USE_LAYER_COLOR_MULTI, hive.use_layer_color_multi);
			get_check(IDC_DONT_WRITE_BYTECODE, hive.dont_write_bytecode);

			for (size_t i = 0; i < 3; i++)
			{
				get_int(IDC_DARK_1_FILL_COLOR + i, hive.dark_color[i].fill);
				get_int(IDC_DARK_1_BORDER_COLOR + i, hive.dark_color[i].border);
				get_int(IDC_DARK_1_TEXT_COLOR + i, hive.dark_color[i].text);

				get_int(IDC_LIGHT_1_FILL_COLOR + i, hive.light_color[i].fill);
				get_int(IDC_LIGHT_1_BORDER_COLOR + i, hive.light_color[i].border);
				get_int(IDC_LIGHT_1_TEXT_COLOR + i, hive.light_color[i].text);
			}
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
			set_int(IDC_SHADOW_DENSITY, hive.shadow_density);
			set_int(IDC_SCROLLBAR_REDUCTION, hive.scrollbar_reduction);
			set_check(IDC_DRAW_SHADOW, hive.draw_shadow);
			set_check(IDC_AS_ROUND, hive.as_round);
			set_check(IDC_DRAW_GRADATION, hive.draw_gradation);
			set_check(IDC_DRAW_BUTTON_EDGE, hive.draw_button_edge);
			set_check(IDC_DRAW_BORDER, hive.draw_border);
			set_check(IDC_AS_ZEBRA, hive.as_zebra);
			set_check(IDC_APPLY_FILE_DIALOG, hive.apply_file_dialog);
			set_check(IDC_FIX_DPI_SCALING, hive.fix_dpi_scaling);
			set_check(IDC_USE_LAYER_COLOR, hive.use_layer_color);
			set_check(IDC_USE_LAYER_COLOR_MULTI, hive.use_layer_color_multi);
			set_check(IDC_DONT_WRITE_BYTECODE, hive.dont_write_bytecode);

			for (size_t i = 0; i < std::size(hive.dark_color); i++)
			{
				set_int(IDC_DARK_1_FILL_COLOR + i, hive.dark_color[i].fill);
				set_int(IDC_DARK_1_BORDER_COLOR + i, hive.dark_color[i].border);
				set_int(IDC_DARK_1_TEXT_COLOR + i, hive.dark_color[i].text);

				set_int(IDC_LIGHT_1_FILL_COLOR + i, hive.light_color[i].fill);
				set_int(IDC_LIGHT_1_BORDER_COLOR + i, hive.light_color[i].border);
				set_int(IDC_LIGHT_1_TEXT_COLOR + i, hive.light_color[i].text);
			}
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
		// pythonから配色を取得します。
		//
		BOOL pick_color(uint32_t fill_id, uint32_t border_id, uint32_t text_id)
		{
			try
			{
				// pythonにアクセスするための準備をします。
				auto utils = py::module::import("utils");
				auto pick_color = [utils](LPCSTR name, size_t index) {
					return utils.attr("get_tone_color")(name, index).cast<COLORREF>();
				};

				// pythonから配色を取得します。
				share::ColorSet color[std::size(hive.dark_color)] = {};
				for (size_t i = 0; i < std::size(color); i++)
				{
					color[i].fill = pick_color("fill", i);
					color[i].border = pick_color("border", i);
					color[i].text = pick_color("text", i);
				}

				// 取得した配色をコントロールに適用します。
				for (size_t i = 0; i < std::size(color); i++)
				{
					set_uint(fill_id + i, color[i].fill);
					set_uint(border_id + i, color[i].border);
					set_uint(text_id + i, color[i].text);

					::InvalidateRect(ctrl(fill_id + i), nullptr, FALSE);
					::InvalidateRect(ctrl(border_id + i), nullptr, FALSE);
					::InvalidateRect(ctrl(text_id + i), nullptr, FALSE);
				}

				// コントロールが変更されたことをアプリに通知します。
				return app->on_change_controls();
			}
			catch (const std::exception& error)
			{
				auto what = python.convert_what(error);
				MY_TRACE_STR(what);

				// pythonでエラーが発生したので
				// メッセージボックスでエラーメッセージを表示します。
				hive.message_box(std::format(L"配色の取得に失敗しました\n\n{}", what));
			}

			return FALSE;
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
			auto button = std::make_shared<RelativePos>(base_size * 4);
			auto editbox = std::make_shared<RelativePos>(base_size * 2 + margin_value * 2);
			auto checkbox = std::make_shared<RelativePos>(base_size * 7);
			auto combobox = std::make_shared<RelativePos>(base_size * 7);
			auto combobox_short = std::make_shared<RelativePos>(base_size * 6);
			auto combobox_long = std::make_shared<RelativePos>(base_size * 11);
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
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox_short, margin, ctrl(IDC_SKIN));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SCHEME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox_long, margin, ctrl(IDC_SCHEME));
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

				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SHADOW_DENSITY_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, nullptr);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_SHADOW_DENSITY_SPIN));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, full, {}, ctrl(IDC_SHADOW_DENSITY));
				}

				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SCROLLBAR_REDUCTION_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, editbox, margin, nullptr);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_SCROLLBAR_REDUCTION_SPIN));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, full, {}, ctrl(IDC_SCROLLBAR_REDUCTION));
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
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_DRAW_GRADATION));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_USE_LAYER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_USE_LAYER_COLOR_MULTI));
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_DONT_WRITE_BYTECODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, nullptr);
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_FILL_COLOR_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_BORDER_COLOR_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_TEXT_COLOR_STAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_DARK_1_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_1_FILL_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_1_BORDER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_1_TEXT_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_PICK_DARK_COLOR_FROM_SCHEME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_DARK_2_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_2_FILL_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_2_BORDER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_2_TEXT_COLOR));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_DARK_3_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_3_FILL_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_3_BORDER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_DARK_3_TEXT_COLOR));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_LIGHT_1_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_1_FILL_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_1_BORDER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_1_TEXT_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_PICK_LIGHT_COLOR_FROM_SCHEME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_LIGHT_2_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_2_FILL_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_2_BORDER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_2_TEXT_COLOR));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_LIGHT_3_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_3_FILL_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_3_BORDER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, button, margin, ctrl(IDC_LIGHT_3_TEXT_COLOR));
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
			case IDC_SHADOW_DENSITY:
			case IDC_SCROLLBAR_REDUCTION:
				{
					if (code == EN_UPDATE)
						app->on_change_controls();

					break;
				}
			// その他のコントロールが変更されたときの処理です。
			case IDC_DARK_MODE:
			case IDC_DRAW_SHADOW:
			case IDC_AS_ROUND:
			case IDC_DRAW_GRADATION:
			case IDC_DRAW_BUTTON_EDGE:
			case IDC_DRAW_BORDER:
			case IDC_AS_ZEBRA:
			case IDC_APPLY_FILE_DIALOG:
			case IDC_FIX_DPI_SCALING:
			case IDC_USE_LAYER_COLOR:
			case IDC_USE_LAYER_COLOR_MULTI:
			case IDC_DONT_WRITE_BYTECODE:
				{
					app->on_change_controls();

					break;
				}
			// カラーピッカーボタンが押されたときの処理です。
			case IDC_DARK_1_FILL_COLOR:
			case IDC_DARK_1_BORDER_COLOR:
			case IDC_DARK_1_TEXT_COLOR:
			case IDC_DARK_2_FILL_COLOR:
			case IDC_DARK_2_BORDER_COLOR:
			case IDC_DARK_2_TEXT_COLOR:
			case IDC_DARK_3_FILL_COLOR:
			case IDC_DARK_3_BORDER_COLOR:
			case IDC_DARK_3_TEXT_COLOR:
			case IDC_LIGHT_1_FILL_COLOR:
			case IDC_LIGHT_1_BORDER_COLOR:
			case IDC_LIGHT_1_TEXT_COLOR:
			case IDC_LIGHT_2_FILL_COLOR:
			case IDC_LIGHT_2_BORDER_COLOR:
			case IDC_LIGHT_2_TEXT_COLOR:
			case IDC_LIGHT_3_FILL_COLOR:
			case IDC_LIGHT_3_BORDER_COLOR:
			case IDC_LIGHT_3_TEXT_COLOR:
				{
					// 色選択ダイアログを表示します。
					CHOOSECOLOR cc { sizeof(cc) };
					cc.hwndOwner = hwnd;
					cc.lpCustColors = hive.custom_colors;
					cc.rgbResult = get_uint(id);
					cc.Flags = CC_RGBINIT | CC_FULLOPEN;
					if (!::ChooseColor(&cc)) break;

					// 取得した色をコントロールに適用します。
					set_uint(id, cc.rgbResult);
					::InvalidateRect(control, nullptr, FALSE);

					// コントロールが変更されたことをアプリに通知します。
					app->on_change_controls();
//					app->update_skin();

					break;
				}
			case IDC_PICK_DARK_COLOR_FROM_SCHEME:
				{
					pick_color(
						IDC_DARK_1_FILL_COLOR,
						IDC_DARK_1_BORDER_COLOR,
						IDC_DARK_1_TEXT_COLOR);

					break;
				}
			case IDC_PICK_LIGHT_COLOR_FROM_SCHEME:
				{
					pick_color(
						IDC_LIGHT_1_FILL_COLOR,
						IDC_LIGHT_1_BORDER_COLOR,
						IDC_LIGHT_1_TEXT_COLOR);

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
					case IDC_DARK_1_FILL_COLOR:
					case IDC_DARK_1_BORDER_COLOR:
					case IDC_DARK_1_TEXT_COLOR:
					case IDC_DARK_2_FILL_COLOR:
					case IDC_DARK_2_BORDER_COLOR:
					case IDC_DARK_2_TEXT_COLOR:
					case IDC_DARK_3_FILL_COLOR:
					case IDC_DARK_3_BORDER_COLOR:
					case IDC_DARK_3_TEXT_COLOR:
					case IDC_LIGHT_1_FILL_COLOR:
					case IDC_LIGHT_1_BORDER_COLOR:
					case IDC_LIGHT_1_TEXT_COLOR:
					case IDC_LIGHT_2_FILL_COLOR:
					case IDC_LIGHT_2_BORDER_COLOR:
					case IDC_LIGHT_2_TEXT_COLOR:
					case IDC_LIGHT_3_FILL_COLOR:
					case IDC_LIGHT_3_BORDER_COLOR:
					case IDC_LIGHT_3_TEXT_COLOR:
						{
							auto dis = (DRAWITEMSTRUCT*)lParam;
							auto dc = dis->hDC;
							auto rc = dis->rcItem;
							auto color = get_uint(id);

							// 背景を描画します。
							::FillRect(dc, &rc, my::gdi::unique_ptr<HBRUSH>(::CreateSolidBrush(color)).get());

							// 縁を描画します。
							::DrawEdge(dc, &rc, EDGE_RAISED, BF_RECT);

							// 文字の色を算出します。
							auto r = GetRValue(color);
							auto g = GetGValue(color);
							auto b = GetBValue(color);
							auto sqr = [](auto value) { return value * value; };
							auto toning_color = sqrt(sqr(r) + sqr(g * 2) + sqr(b)) < 300 ? RGB(255, 255, 255) : RGB(0, 0, 0);

							// 文字を描画します。
							auto old_text_color = ::SetTextColor(dc, toning_color);
							auto old_bk_mode = ::SetBkMode(dc, TRANSPARENT);
							auto s = std::format(_T("{}, {}, {}"), r, g, b);
							::DrawText(dc, s.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
							::SetTextColor(dc, old_text_color);
							::SetBkMode(dc, old_bk_mode);

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
						case IDC_SHADOW_DENSITY:
						case IDC_SCROLLBAR_REDUCTION:
							{
								auto value = get_int(edit_id);
								value += (nm->iDelta > 0) ? -10 : +10;
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
