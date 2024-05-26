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
			get_int(IDC_ELLIPSE, hive.ellipse);
			get_int(IDC_BORDER_WIDTH, hive.border_width);
			get_combobox_index(IDC_SHADOW_MODE, hive.shadow_mode);
			get_combobox_index(IDC_CORNER_MODE, hive.round_mode);
			get_combobox_index(IDC_STATIC_EDGE_MODE, hive.static_edge_mode);
			get_combobox_index(IDC_FILE_DIALOG_MODE, hive.file_dialog_mode);
			get_combobox_index(IDC_DPI_SCALING_MODE, hive.dpi_scaling_mode);
			get_check(IDC_USE_LAYER_COLOR, hive.use_layer_color);
			get_check(IDC_USE_LAYER_COLOR_MULTI, hive.use_layer_color_multi);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_combobox_string(IDC_SKIN, hive.skin_name);
			set_combobox_string(IDC_SCHEME, hive.scheme_name);
			set_int(IDC_ELLIPSE, hive.ellipse);
			set_int(IDC_BORDER_WIDTH, hive.border_width);
			set_combobox_index(IDC_SHADOW_MODE, hive.shadow_mode);
			set_combobox_index(IDC_CORNER_MODE, hive.round_mode);
			set_combobox_index(IDC_STATIC_EDGE_MODE, hive.static_edge_mode);
			set_combobox_index(IDC_FILE_DIALOG_MODE, hive.file_dialog_mode);
			set_combobox_index(IDC_DPI_SCALING_MODE, hive.dpi_scaling_mode);
			set_check(IDC_USE_LAYER_COLOR, hive.use_layer_color);
			set_check(IDC_USE_LAYER_COLOR_MULTI, hive.use_layer_color_multi);
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

			init_combobox(IDC_SHADOW_MODE, _T("影を付ける"), _T("影を省略する"));
			init_combobox(IDC_CORNER_MODE, _T("角を丸くする"), _T("丸角を省略する"));
			init_combobox(IDC_STATIC_EDGE_MODE, _T("通常"), _T("エッジを省略する"));
			init_combobox(IDC_FILE_DIALOG_MODE, _T("通常"), _T("ファイル選択ダイアログを除外する"));
			init_combobox(IDC_DPI_SCALING_MODE, _T("通常"), _T("補正する"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto row = std::make_shared<RelativePos>(get_base_size() + margin_value * 2);
			auto half = std::make_shared<AbsolutePos>(1, 2);
			auto full = std::make_shared<AbsolutePos>(2, 2);
			std::shared_ptr<AbsolutePos> q[9];
			for (auto i = 0; i < std::size(q); i++)
				q[i] = std::make_shared<AbsolutePos>(i, std::size(q) - 1);
			auto rc = my::get_client_rect(ctrl(IDC_ELLIPSE_SPIN));
			auto spin_w = my::get_width(rc);
			auto spin = std::make_shared<RelativePos>(spin_w);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[1], margin, ctrl(IDC_SKIN_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(IDC_SKIN));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[5], margin, ctrl(IDC_SCHEME_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[8], margin, ctrl(IDC_SCHEME));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);

				node->add_pane(c_axis.c_horz, c_align.c_left, q[1], margin, ctrl(IDC_ELLIPSE_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, q[2], margin, nullptr);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_ELLIPSE_SPIN));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, full, {}, ctrl(IDC_ELLIPSE));
				}

				node->add_pane(c_axis.c_horz, c_align.c_left, q[3], margin, ctrl(IDC_BORDER_WIDTH_STAT));
				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, nullptr);
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, spin, {}, ctrl(IDC_BORDER_WIDTH_SPIN));
					sub_node->add_pane(c_axis.c_horz, c_align.c_right, full, {}, ctrl(IDC_BORDER_WIDTH));
				}
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[1], margin, ctrl(IDC_SHADOW_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(IDC_SHADOW_MODE));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[5], margin, ctrl(IDC_CORNER_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[8], margin, ctrl(IDC_CORNER_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[1], margin, ctrl(IDC_STATIC_EDGE_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(IDC_STATIC_EDGE_MODE));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[5], margin, ctrl(IDC_FILE_DIALOG_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[8], margin, ctrl(IDC_FILE_DIALOG_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[1], margin, ctrl(IDC_DPI_SCALING_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(IDC_DPI_SCALING_MODE));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_USE_LAYER_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[8], margin, ctrl(IDC_USE_LAYER_COLOR_MULTI));
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
			case IDC_SHADOW_MODE:
			case IDC_CORNER_MODE:
			case IDC_STATIC_EDGE_MODE:
			case IDC_FILE_DIALOG_MODE:
			case IDC_DPI_SCALING_MODE:
				{
					if (code == CBN_SELCHANGE)
						hive.app->on_change_controls();

					break;
				}
			// エディットボックスが変更されたときの処理です。
			case IDC_ELLIPSE:
			case IDC_BORDER_WIDTH:
				{
					if (code == EN_UPDATE)
						hive.app->on_change_controls();

					break;
				}
			// その他のコントロールが変更されたときの処理です。
			case IDC_USE_LAYER_COLOR:
			case IDC_USE_LAYER_COLOR_MULTI:
				{
					hive.app->on_change_controls();

					break;
				}
			// カラーピッカーボタンが押されたときの処理です。
			case IDC_MAIN_BACKGROUND_DARK:
			case IDC_MAIN_BACKGROUND_LIGHT:
			case IDC_MAIN_TEXT_DARK:
			case IDC_MAIN_TEXT_LIGHT:
			case IDC_SUB_BACKGROUND_DARK:
			case IDC_SUB_BACKGROUND_LIGHT:
			case IDC_SUB_TEXT_DARK:
			case IDC_SUB_TEXT_LIGHT:
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
					::InvalidateRect(control, 0, FALSE);

					hive.app->update_skin();

					break;
				}
			// 「入れ替える」ボタンが押されたときの処理です。
			case IDC_MAIN_BACKGROUND_SWAP:
			case IDC_MAIN_TEXT_SWAP:
			case IDC_SUB_BACKGROUND_SWAP:
			case IDC_SUB_TEXT_SWAP:
				{
					auto dark_id = id - 2;
					auto light_id = id - 1;

					auto dark = get_uint(dark_id);
					auto light = get_uint(light_id);

					set_uint(dark_id, light);
					set_uint(light_id, dark);

					::InvalidateRect(::GetDlgItem(*this, dark_id), nullptr, FALSE);
					::InvalidateRect(::GetDlgItem(*this, light_id), nullptr, FALSE);

					hive.app->update_skin();

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
					case IDC_MAIN_BACKGROUND_DARK:
					case IDC_MAIN_BACKGROUND_LIGHT:
					case IDC_MAIN_TEXT_DARK:
					case IDC_MAIN_TEXT_LIGHT:
					case IDC_SUB_BACKGROUND_DARK:
					case IDC_SUB_BACKGROUND_LIGHT:
					case IDC_SUB_TEXT_DARK:
					case IDC_SUB_TEXT_LIGHT:
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
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
