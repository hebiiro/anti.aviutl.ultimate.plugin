#pragma once

namespace apn::font_preview
{
	//
	// このクラスはメインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_text(IDC_SAMPLE_TEXT, hive.sample);
			set_int(IDC_ITEM_HEIGHT, hive.item_height);
			set_int(IDC_FONT_HEIGHT, hive.font_height);
			set_int(IDC_WINDOW_SIZE_W, hive.window_size.cx);
			set_int(IDC_WINDOW_SIZE_H, hive.window_size.cy);
			set_check(IDC_SINGLELINE, hive.singleline);
			set_combobox_index(IDC_PAINT_MODE, paint.mode);
			set_uint(IDC_NORMAL_BACKGROUND_COLOR, paint.palette[paint.c_style.c_normal].color.background);
			set_uint(IDC_NORMAL_PREVIEW_COLOR, paint.palette[paint.c_style.c_normal].color.sample);
			set_uint(IDC_NORMAL_FONT_NAME_COLOR, paint.palette[paint.c_style.c_normal].color.font_name);
			set_uint(IDC_SELECT_BACKGROUND_COLOR, paint.palette[paint.c_style.c_select].color.background);
			set_uint(IDC_SELECT_PREVIEW_COLOR, paint.palette[paint.c_style.c_select].color.sample);
			set_uint(IDC_SELECT_FONT_NAME_COLOR, paint.palette[paint.c_style.c_select].color.font_name);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_text(IDC_SAMPLE_TEXT, hive.sample);
			get_int(IDC_ITEM_HEIGHT, hive.item_height);
			get_int(IDC_FONT_HEIGHT, hive.font_height);
			get_int(IDC_WINDOW_SIZE_W, hive.window_size.cx);
			get_int(IDC_WINDOW_SIZE_H, hive.window_size.cy);
			get_check(IDC_SINGLELINE, hive.singleline);
			get_combobox_index(IDC_PAINT_MODE, paint.mode);
			get_uint(IDC_NORMAL_BACKGROUND_COLOR, paint.palette[paint.c_style.c_normal].color.background);
			get_uint(IDC_NORMAL_PREVIEW_COLOR, paint.palette[paint.c_style.c_normal].color.sample);
			get_uint(IDC_NORMAL_FONT_NAME_COLOR, paint.palette[paint.c_style.c_normal].color.font_name);
			get_uint(IDC_SELECT_BACKGROUND_COLOR, paint.palette[paint.c_style.c_select].color.background);
			get_uint(IDC_SELECT_PREVIEW_COLOR, paint.palette[paint.c_style.c_select].color.sample);
			get_uint(IDC_SELECT_FONT_NAME_COLOR, paint.palette[paint.c_style.c_select].color.font_name);

			auto hwnd = magi.exin.get_setting_dialog();
			auto combobox = ::GetDlgItem(hwnd, hive.c_control_id.c_font_combobox);
			::InvalidateRect(combobox, nullptr, TRUE);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(IDC_PAINT_MODE, _T("自動"), _T("システムカラー"), _T("テーマ"), _T("カスタムカラー"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto row = std::make_shared<RelativePos>(get_base_size() + margin_value * 2);
			std::shared_ptr<AbsolutePos> q[12 + 1];
			for (auto i = 0; i < std::size(q); i++)
				q[i] = std::make_shared<AbsolutePos>(i, std::size(q) - 1);
			auto multiline_row = std::make_shared<RelativePos>((get_base_size() + margin_value * 2) * 3);
			auto groupbox_margin = RECT { margin_value * 4, get_base_size() / 2 + margin_value, margin_value * 4, margin_value * 4 };
			auto groupbox_row = std::make_shared<RelativePos>(get_base_size() * 2 + margin_value * 2);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, multiline_row);

				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, q[5], margin, ctrl(IDC_SAMPLE_TEXT));
				}

				{
					auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, nullptr);

					{
						auto node = sub_node->add_pane(c_axis.c_vert, c_align.c_top, row);
						node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(IDC_ITEM_HEIGHT_STAT));
						node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_ITEM_HEIGHT));
						node->add_pane(c_axis.c_horz, c_align.c_left, q[10], margin, ctrl(IDC_FONT_HEIGHT_STAT));
						node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_FONT_HEIGHT));
					}

					{
						auto node = sub_node->add_pane(c_axis.c_vert, c_align.c_top, row);
						node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(IDC_WINDOW_SIZE_STAT));
						node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_WINDOW_SIZE_W));
						node->add_pane(c_axis.c_horz, c_align.c_left, q[8], margin, ctrl(IDC_WINDOW_SIZE_H));
					}

					{
						auto node = sub_node->add_pane(c_axis.c_vert, c_align.c_top, row);
						node->add_pane(c_axis.c_horz, c_align.c_left, q[5], margin, ctrl(IDC_SINGLELINE));
						node->add_pane(c_axis.c_horz, c_align.c_left, q[8], margin, ctrl(IDC_PAINT_MODE_STAT));
						node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_PAINT_MODE));
					}
				}
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, groupbox_row, margin, ctrl(IDC_NORMAL_STAT));
				auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, q[12], groupbox_margin);
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[2], margin, ctrl(IDC_NORMAL_BACKGROUND_COLOR_STAT));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(IDC_NORMAL_BACKGROUND_COLOR));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_NORMAL_PREVIEW_COLOR_STAT));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[8], margin, ctrl(IDC_NORMAL_PREVIEW_COLOR));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[10], margin, ctrl(IDC_NORMAL_FONT_NAME_COLOR_STAT));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_NORMAL_FONT_NAME_COLOR));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, groupbox_row, margin, ctrl(IDC_SELECT_STAT));
				auto sub_node = node->add_pane(c_axis.c_horz, c_align.c_left, q[12], groupbox_margin);
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[2], margin, ctrl(IDC_SELECT_BACKGROUND_COLOR_STAT));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[4], margin, ctrl(IDC_SELECT_BACKGROUND_COLOR));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[6], margin, ctrl(IDC_SELECT_PREVIEW_COLOR_STAT));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[8], margin, ctrl(IDC_SELECT_PREVIEW_COLOR));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[10], margin, ctrl(IDC_SELECT_FONT_NAME_COLOR_STAT));
				sub_node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_SELECT_FONT_NAME_COLOR));
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
			case IDC_SAMPLE_TEXT:
			case IDC_ITEM_HEIGHT:
			case IDC_FONT_HEIGHT:
			case IDC_WINDOW_SIZE_W:
			case IDC_WINDOW_SIZE_H:
				{
					if (code == EN_UPDATE) update_config();

					break;
				}
			case IDC_SINGLELINE:
				{
					update_config();

					break;
				}
			case IDC_PAINT_MODE:
				{
					if (code == CBN_SELCHANGE) update_config();

					break;
				}
			case IDC_NORMAL_BACKGROUND_COLOR:
			case IDC_NORMAL_PREVIEW_COLOR:
			case IDC_NORMAL_FONT_NAME_COLOR:
			case IDC_SELECT_BACKGROUND_COLOR:
			case IDC_SELECT_PREVIEW_COLOR:
			case IDC_SELECT_FONT_NAME_COLOR:
				{
					auto color = get_uint(id);

					CHOOSECOLOR cc { sizeof(cc) };
					cc.hwndOwner = hwnd;
					cc.lpCustColors = hive.custom_colors;
					cc.rgbResult = color;
					cc.Flags = CC_RGBINIT | CC_FULLOPEN;
					if (!::ChooseColor(&cc)) return;

					set_uint(id, cc.rgbResult);

					::InvalidateRect(control, 0, FALSE);

					update_config();

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
					case IDC_NORMAL_BACKGROUND_COLOR:
					case IDC_NORMAL_PREVIEW_COLOR:
					case IDC_NORMAL_FONT_NAME_COLOR:
					case IDC_SELECT_BACKGROUND_COLOR:
					case IDC_SELECT_PREVIEW_COLOR:
					case IDC_SELECT_FONT_NAME_COLOR:
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
