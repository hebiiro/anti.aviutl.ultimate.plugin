#pragma once

namespace apn::dark
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct addin_dialog_t : StdAddinDialog<idd_config>
	{
		//
		// コンフィグでコントロールを更新します。
		//
		BOOL to_ui() { return update_controls(); }

		//
		// コントロールでコンフィグを更新します。
		//
		BOOL from_ui() { return update_config(); }

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_text(idc_dark_style_file_name, hive.dark.style_file_name);
			set_check(idc_d2d_flag_figure, hive.d2d.flag_figure);
			set_check(idc_d2d_flag_text, hive.d2d.flag_text);
			set_check(idc_dark_flag_exclude_comdlg32, hive.dark.flag_exclude_comdlg32);

			set_check(idc_round_flag_use, hive.round.flag_use);
			set_int(idc_round_size, hive.round.size);

			set_check(idc_border_flag_3d_edge, hive.border.flag_3d_edge);
			set_int(idc_border_width, hive.border.width);

			set_check(idc_gradient_flag_use, hive.gradient.flag_use);
			set_check(idc_gradient_flag_end_alpha, hive.gradient.flag_end_alpha);
			set_int(idc_gradient_end_alpha, hive.gradient.end_alpha);

			set_check(idc_shadow_flag_use, hive.shadow.flag_use);
			set_check(idc_shadow_flag_blur, hive.shadow.flag_blur);
			set_int(idc_shadow_offset_x, hive.shadow.offset.x);
			set_int(idc_shadow_offset_y, hive.shadow.offset.y);
			set_int(idc_shadow_size, hive.shadow.size);

			set_check(idc_scrollbar_flag_arrow_as_button, hive.scrollbar.flag_arrow_as_button);
			set_check(idc_scrollbar_flag_has_gripper, hive.scrollbar.flag_has_gripper);
			set_int(idc_scrollbar_reduction, hive.scrollbar.reduction);

			set_check(idc_checkbox_flag_reversed, hive.checkbox.flag_reversed);
			set_check(idc_exedit_flag_draw_button_edge, hive.exedit.flag_draw_button_edge);
			set_combobox_index(idc_exedit_layer_border, hive.exedit.layer_border);
			set_check(idc_exedit_flag_as_zebra, hive.exedit.flag_as_zebra);
			set_check(idc_exedit_flag_draw_selected_layer, hive.exedit.flag_draw_selected_layer);
			set_check(idc_exedit_flag_use_layer_color, hive.exedit.flag_use_layer_color);
			set_check(idc_exedit_flag_use_layer_color_multi, hive.exedit.flag_use_layer_color_multi);
			set_combobox_index(idc_exedit_cursor_style, hive.exedit.cursor.style);
			set_int(idc_exedit_cursor_width, hive.exedit.cursor.width);
			set_int(idc_exedit_group_alpha, hive.exedit.group.alpha);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

//			get_text(idc_dark_style_file_name, hive.dark.style_file_name);
			get_check(idc_d2d_flag_figure, hive.d2d.flag_figure);
			get_check(idc_d2d_flag_text, hive.d2d.flag_text);
			get_check(idc_dark_flag_exclude_comdlg32, hive.dark.flag_exclude_comdlg32);

			get_check(idc_round_flag_use, hive.round.flag_use);
			get_int(idc_round_size, hive.round.size);

			get_check(idc_border_flag_3d_edge, hive.border.flag_3d_edge);
			get_int(idc_border_width, hive.border.width);

			get_check(idc_gradient_flag_use, hive.gradient.flag_use);
			get_check(idc_gradient_flag_end_alpha, hive.gradient.flag_end_alpha);
			get_int(idc_gradient_end_alpha, hive.gradient.end_alpha);

			get_check(idc_shadow_flag_use, hive.shadow.flag_use);
			get_check(idc_shadow_flag_blur, hive.shadow.flag_blur);
			get_int(idc_shadow_offset_x, hive.shadow.offset.x);
			get_int(idc_shadow_offset_y, hive.shadow.offset.y);
			get_int(idc_shadow_size, hive.shadow.size);

			get_check(idc_scrollbar_flag_arrow_as_button, hive.scrollbar.flag_arrow_as_button);
			get_check(idc_scrollbar_flag_has_gripper, hive.scrollbar.flag_has_gripper);
			get_int(idc_scrollbar_reduction, hive.scrollbar.reduction);

			get_check(idc_checkbox_flag_reversed, hive.checkbox.flag_reversed);
			get_check(idc_exedit_flag_draw_button_edge, hive.exedit.flag_draw_button_edge);
			get_combobox_index(idc_exedit_layer_border, hive.exedit.layer_border);
			get_check(idc_exedit_flag_as_zebra, hive.exedit.flag_as_zebra);
			get_check(idc_exedit_flag_draw_selected_layer, hive.exedit.flag_draw_selected_layer);
			get_check(idc_exedit_flag_use_layer_color, hive.exedit.flag_use_layer_color);
			get_check(idc_exedit_flag_use_layer_color_multi, hive.exedit.flag_use_layer_color_multi);
			get_combobox_index(idc_exedit_cursor_style, hive.exedit.cursor.style);
			get_int(idc_exedit_cursor_width, hive.exedit.cursor.width);
			get_int(idc_exedit_group_alpha, hive.exedit.group.alpha);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(idc_exedit_layer_border,
				_T("通常"),
				_T("上だけ"),
				_T("なし"));

			init_combobox(idc_exedit_cursor_style,
				_T("ソリッド"),
				_T("ダッシュ"),
				_T("ドット"),
				_T("ダッシュドット"),
				_T("ダッシュドットドット"));
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			switch (control_id)
			{
			case idc_dark_style_file_name_ref:
				{
					// ユーザーが指定したスタイルファイルのパスを取得します。
					auto file_name = get_open_file_name(
						hwnd,
						L"スタイルファイルを選択",
						L"スタイルファイル (*.conf)\0*.conf\0"
						L"すべてのファイル (*.*)\0*.*\0",
						hive.dark.style_file_name.c_str());
					if (file_name.empty()) break;

					app->on_set_style_file_name(file_name);

					break;
				}
			case idc_dark_flag_exclude_comdlg32:
				{
					from_ui();

					break;
				}
			case idc_d2d_flag_figure:
			case idc_d2d_flag_text:
			case idc_round_flag_use:
			case idc_border_flag_3d_edge:
			case idc_gradient_flag_use:
			case idc_gradient_flag_end_alpha:
			case idc_shadow_flag_use:
			case idc_shadow_flag_blur:
			case idc_scrollbar_flag_arrow_as_button:
			case idc_scrollbar_flag_has_gripper:
			case idc_checkbox_flag_reversed:
			case idc_exedit_flag_draw_button_edge:
			case idc_exedit_flag_as_zebra:
			case idc_exedit_flag_draw_selected_layer:
			case idc_exedit_flag_use_layer_color:
			case idc_exedit_flag_use_layer_color_multi:
				{
					from_ui(), app->on_update();

					break;
				}
			case idc_round_size:
			case idc_border_width:
			case idc_gradient_end_alpha:
			case idc_shadow_offset_x:
			case idc_shadow_offset_y:
			case idc_shadow_size:
			case idc_scrollbar_reduction:
			case idc_exedit_cursor_width:
			case idc_exedit_group_alpha:
				{
					if (code == EN_UPDATE)
						from_ui(), app->on_update();

					break;
				}
			case idc_exedit_layer_border:
			case idc_exedit_cursor_style:
				{
					if (code == CBN_SELCHANGE)
						from_ui(), app->on_update();

					break;
				}
			}
		}

		//
		// WM_NOTIFYを処理します。
		//
		virtual void on_notify(NMHDR* nmhdr) override
		{
			switch(nmhdr->code)
			{
			case UDN_DELTAPOS:
				{
					auto nm = (NMUPDOWN*)nmhdr;
					auto edit_id = (UINT)nmhdr->idFrom - 1;

					switch (edit_id)
					{
					case idc_border_width:
					case idc_shadow_size:
					case idc_exedit_cursor_width:
						{
							auto value = get_int(edit_id);
							value += (nm->iDelta > 0) ? -1 : +1;
							value = std::clamp(value, 0, +100);
							set_int(edit_id, value);
							break;
						}
					case idc_round_size:
					case idc_gradient_end_alpha:
					case idc_scrollbar_reduction:
					case idc_exedit_group_alpha:
						{
							auto value = get_int(edit_id);
							value += (nm->iDelta > 0) ? -10 : +10;
							value = std::clamp(value, 0, +100);
							set_int(edit_id, value);
							break;
						}
					case idc_shadow_offset_x:
					case idc_shadow_offset_y:
						{
							auto value = get_int(edit_id);
							value += (nm->iDelta > 0) ? -10 : +10;
							value = std::clamp(value, -100, +100);
							set_int(edit_id, value);
							break;
						}
					}

					break;
				}
			}
		}
	} addin_dialog;
}
