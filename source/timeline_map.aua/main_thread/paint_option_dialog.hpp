#pragma once

namespace apn::timeline_map::main_thread
{
	//
	// このクラスは描画設定ダイアログです。
	//
	inline struct paint_option_dialog_t : StdAddinDialog<idd_paint_option>
	{
		//
		// このクラスはリスナーです。
		//
		struct listner_t {
			virtual void on_from_ui(BOOL recreate_resources) = 0;
		} *listener = {};

		//
		// コントロールからRGBAを取得します。
		//
		void get_rgba(uint32_t control_id, rgba_t& rgba)
		{
			auto rgb = get_uint(control_id);
			auto a = get_uint(control_id + 1);

			rgba.r = GetRValue(rgb);
			rgba.g = GetGValue(rgb);
			rgba.b = GetBValue(rgb);
			rgba.a = (uint8_t)a;
		}

		//
		// コントロールにRGBAをセットします。
		//
		void set_rgba(uint32_t control_id, const rgba_t& rgba)
		{
			set_uint(control_id, RGB(rgba.r, rgba.g, rgba.b));
			set_uint(control_id + 1, rgba.a);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_text(idc_text_font_name, property.text.font_name);
			get_int(idc_text_font_size, property.text.font_size);
			get_rgba(idc_text_color, property.text.color);
			get_rgba(idc_text_shadow_color, property.text.shadow_color);
			get_int(idc_text_padding_x, property.text.padding.x);
			get_int(idc_text_padding_y, property.text.padding.y);
			get_int(idc_text_shadow_offset_x, property.text.shadow_offset.x);
			get_int(idc_text_shadow_offset_y, property.text.shadow_offset.y);
			get_combobox_index(idc_text_horz_align, property.text.horz_align);
			get_combobox_index(idc_text_vert_align, property.text.vert_align);

			get_int(idc_layer_top_space, property.layer.top_space);
			get_rgba(idc_layer_odd_color, property.layer.odd_color);
			get_rgba(idc_layer_even_color, property.layer.even_color);

			get_int(idc_item_round_size, property.item.round_size);
			get_combobox_index(idc_item_round_mode, property.item.round_mode);
			get_rgba(idc_item_stroke_color, property.item.stroke_color);
			get_int(idc_item_stroke_width, property.item.stroke_width);

			for (size_t i = 0; i < property.item.c_item.c_max_size; i++)
			{
				get_rgba(idc_item_image_media_start_color + i * 4, property.item.nodes[i].start_color);
				get_rgba(idc_item_image_media_end_color + i * 4, property.item.nodes[i].end_color);
			}

			get_rgba(idc_midpt_color, property.midpt.color);
			get_rgba(idc_midpt_line_color, property.midpt.line_color);
			get_int(idc_midpt_line_width, property.midpt.line_width);

			get_rgba(idc_current_frame_line_color, property.current_frame.line_color);
			get_int(idc_current_frame_line_width, property.current_frame.line_width);

			get_int(idc_visible_area_round_size, property.visible_area.round_size);
			get_combobox_index(idc_visible_area_round_mode, property.visible_area.round_mode);
			get_rgba(idc_visible_area_color, property.visible_area.color);
			get_rgba(idc_visible_area_stroke_color, property.visible_area.stroke_color);
			get_int(idc_visible_area_stroke_width, property.visible_area.stroke_width);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_text(idc_text_font_name, property.text.font_name);
			set_int(idc_text_font_size, property.text.font_size);
			set_rgba(idc_text_color, property.text.color);
			set_rgba(idc_text_shadow_color, property.text.shadow_color);
			set_int(idc_text_padding_x, property.text.padding.x);
			set_int(idc_text_padding_y, property.text.padding.y);
			set_int(idc_text_shadow_offset_x, property.text.shadow_offset.x);
			set_int(idc_text_shadow_offset_y, property.text.shadow_offset.y);
			set_combobox_index(idc_text_horz_align, property.text.horz_align);
			set_combobox_index(idc_text_vert_align, property.text.vert_align);

			set_int(idc_layer_top_space, property.layer.top_space);
			set_rgba(idc_layer_odd_color, property.layer.odd_color);
			set_rgba(idc_layer_even_color, property.layer.even_color);

			set_int(idc_item_round_size, property.item.round_size);
			set_combobox_index(idc_item_round_mode, property.item.round_mode);
			set_rgba(idc_item_stroke_color, property.item.stroke_color);
			set_int(idc_item_stroke_width, property.item.stroke_width);

			for (size_t i = 0; i < property.item.c_item.c_max_size; i++)
			{
				set_rgba(idc_item_image_media_start_color + i * 4, property.item.nodes[i].start_color);
				set_rgba(idc_item_image_media_end_color + i * 4, property.item.nodes[i].end_color);
			}

			set_rgba(idc_midpt_color, property.midpt.color);
			set_rgba(idc_midpt_line_color, property.midpt.line_color);
			set_int(idc_midpt_line_width, property.midpt.line_width);

			set_rgba(idc_current_frame_line_color, property.current_frame.line_color);
			set_int(idc_current_frame_line_width, property.current_frame.line_width);

			set_int(idc_visible_area_round_size, property.visible_area.round_size);
			set_combobox_index(idc_visible_area_round_mode, property.visible_area.round_mode);
			set_rgba(idc_visible_area_color, property.visible_area.color);
			set_rgba(idc_visible_area_stroke_color, property.visible_area.stroke_color);
			set_int(idc_visible_area_stroke_width, property.visible_area.stroke_width);
		}

		//
		// コントロールの値でコンフィグを更新します。
		//
		void from_ui(BOOL recreate_resources)
		{
			MY_TRACE_FUNC("");

			// コンフィグを更新します。
			update_config();

			// リスナーに通知します。
			if (listener) listener->on_from_ui(recreate_resources);
		}

		//
		// コントロールの値でコンフィグを更新します。
		//
		void choose_color(HWND control, uint32_t control_id, BOOL recreate_resources)
		{
			try
			{
				// カラー選択ダイアログを表示してカラーを取得します。
				auto color = magi.choose_color(hwnd, get_uint(control_id));

				// 取得した色をコントロールに適用します。
				set_uint(control_id, color);
				my::invalidate(control);

				// コントロールの値でコンフィグを更新します。
				from_ui(recreate_resources);
			}
			catch (...)
			{
			}
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(idc_text_horz_align, L"左", L"右", L"中央");
			init_combobox(idc_text_vert_align, L"上", L"下", L"中央");
			init_combobox(idc_item_round_mode, L"なし", L"絶対", L"相対", L"個別");
			init_combobox(idc_visible_area_round_mode, L"なし", L"絶対", L"相対", L"個別");
		}

		//
		// ダイアログのコマンド処理です。
		//
		virtual void on_command(UINT code, UINT control_id, HWND control) override
		{
			MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", code, control_id, control);

			switch (control_id)
			{
			// ボタンの場合の処理です。
			case IDCANCEL:
				{
					// 描画設定ダイアログを非表示にします。
					::ShowWindow(*this, SW_HIDE);

					break;
				}

			// エディットボックスの場合の処理です。
			case idc_text_font_size:
			case idc_text_padding_x:
			case idc_text_padding_y:
			case idc_text_shadow_offset_x:
			case idc_text_shadow_offset_y:
			case idc_layer_top_space:
			case idc_midpt_line_width:
			case idc_current_frame_line_width:
			case idc_visible_area_round_size:
			case idc_visible_area_stroke_width:
			case idc_item_round_size:
			case idc_item_stroke_width:
				{
					// コントロールの値でコンフィグを更新します。
					if (code == EN_CHANGE) from_ui(FALSE); break;
				}

			// リソースに関連するエディットボックスの場合の処理です。
			case idc_text_color_a:
			case idc_text_shadow_color_a:
			case idc_layer_odd_color_a:
			case idc_layer_even_color_a:
			case idc_midpt_color_a:
			case idc_midpt_line_color_a:
			case idc_current_frame_line_color_a:
			case idc_visible_area_color_a:
			case idc_visible_area_stroke_color_a:
			case idc_item_stroke_color_a:
			case idc_item_image_media_start_color_a:
			case idc_item_image_media_end_color_a:
			case idc_item_image_filter_effect_start_color_a:
			case idc_item_image_filter_effect_end_color_a:
			case idc_item_image_filter_start_color_a:
			case idc_item_image_filter_end_color_a:
			case idc_item_sound_media_start_color_a:
			case idc_item_sound_media_end_color_a:
			case idc_item_sound_filter_start_color_a:
			case idc_item_sound_filter_end_color_a:
			case idc_item_control_start_color_a:
			case idc_item_control_end_color_a:
				{
					// コントロールの値でコンフィグを更新します。
					if (code == EN_CHANGE) from_ui(TRUE); break;
				}

			// コンボボックスの場合の処理です。
			case idc_text_horz_align:
			case idc_text_vert_align:
			case idc_visible_area_round_mode:
			case idc_item_round_mode:
				{
					// コントロールの値でコンフィグを更新します。
					if (code == CBN_SELCHANGE) from_ui(FALSE); break;
				}

			// リソースに関連する色選択ボタンの場合の処理です。
			case idc_text_color:
			case idc_text_shadow_color:
			case idc_layer_odd_color:
			case idc_layer_even_color:
			case idc_midpt_color:
			case idc_midpt_line_color:
			case idc_current_frame_line_color:
			case idc_visible_area_color:
			case idc_visible_area_stroke_color:
			case idc_item_stroke_color:
			case idc_item_image_media_start_color:
			case idc_item_image_media_end_color:
			case idc_item_image_filter_effect_start_color:
			case idc_item_image_filter_effect_end_color:
			case idc_item_image_filter_start_color:
			case idc_item_image_filter_end_color:
			case idc_item_sound_media_start_color:
			case idc_item_sound_media_end_color:
			case idc_item_sound_filter_start_color:
			case idc_item_sound_filter_end_color:
			case idc_item_control_start_color:
			case idc_item_control_end_color:
				{
					// 色選択ダイアログを表示します。
					choose_color(control, control_id, TRUE);

					break;
				}

			// フォント選択ボタンの場合の処理です。
			case idc_text_font_name:
				{
					font_dialog_with_handler_t font_dialog(
						[&](font_dialog_t& font_dialog)
					{
						// 適用ボタンが押されたときの処理です。

						// 取得したフォント名をコントロールに適用します。
						set_text(control_id, font_dialog.font_name);

						// コントロールの値でコンフィグを更新します。
						from_ui(FALSE);
					});

					// フォント選択ダイアログを表示します。
					if (font_dialog.do_modal(hwnd, get_text(control_id)))
					{
						// 適用ボタンが押されたときと同じ処理を実行します。
						font_dialog.apply_handler(font_dialog);
					}

					break;
				}
			}
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_DRAWITEM:
				{
					auto control_id = (UINT)w_param;

					switch (control_id)
					{
					// 色選択ボタンの場合の処理です。
					case idc_text_color:
					case idc_text_shadow_color:
					case idc_layer_odd_color:
					case idc_layer_even_color:
					case idc_midpt_color:
					case idc_midpt_line_color:
					case idc_current_frame_line_color:
					case idc_visible_area_color:
					case idc_visible_area_stroke_color:
					case idc_item_stroke_color:
					case idc_item_image_media_start_color:
					case idc_item_image_media_end_color:
					case idc_item_image_filter_effect_start_color:
					case idc_item_image_filter_effect_end_color:
					case idc_item_image_filter_start_color:
					case idc_item_image_filter_end_color:
					case idc_item_sound_media_start_color:
					case idc_item_sound_media_end_color:
					case idc_item_sound_filter_start_color:
					case idc_item_sound_filter_end_color:
					case idc_item_control_start_color:
					case idc_item_control_end_color:
						{
							auto dis = (DRAWITEMSTRUCT*)l_param;

							// 選択されている色で塗りつぶします。
							my::gdi::unique_ptr<HBRUSH> brush(
								::CreateSolidBrush(get_uint(control_id)));
							::FillRect(dis->hDC, &dis->rcItem, brush.get());

							// ボタンの枠を描画します。
							::DrawEdge(dis->hDC, &dis->rcItem, EDGE_RAISED, BF_RECT);

							return TRUE;
						}
					}

					break;
				}
			case WM_NOTIFY:
				{
					auto header = (NMHDR*)l_param;
					if (header->code == UDN_DELTAPOS)
					{
						// スピンボタンの場合の処理です。
						// 対応するエディットボックスの値を増減させます。

						auto nm = (NMUPDOWN*)header;
						auto edit_id = (UINT)header->idFrom - 1;

						//
						// この関数はエディットボックスの値を増減させます。
						//
						const auto spin = [&](int32_t dec, int32_t inc, int32_t min, int32_t max)
						{
							auto value = get_int(edit_id);
							value += (nm->iDelta > 0) ? dec : inc;
							value = std::clamp(value, min, max);
							set_int(edit_id, value);
						};

						switch (edit_id)
						{
						// アルファの場合の処理です。
						case idc_text_color_a:
						case idc_text_shadow_color_a:
						case idc_layer_odd_color_a:
						case idc_layer_even_color_a:
						case idc_midpt_color_a:
						case idc_midpt_line_color_a:
						case idc_current_frame_line_color_a:
						case idc_visible_area_color_a:
						case idc_visible_area_stroke_color_a:
						case idc_item_stroke_color_a:
							spin(-1, +1, 0, 255); break;

						// 大きさの場合の処理です。
						case idc_text_font_size:
						case idc_text_padding_x:
						case idc_text_padding_y:
						case idc_layer_top_space:
						case idc_midpt_line_width:
						case idc_visible_area_round_size:
						case idc_visible_area_stroke_width:
						case idc_item_round_size:
						case idc_item_stroke_width:
							spin(-1, +1, 0, +10000); break;

						// 座標の場合の処理です。
						case idc_text_shadow_offset_x:
						case idc_text_shadow_offset_y:
							spin(-1, +1, -10000, +10000); break;
						}
					}

					return FALSE;
				}
			}

			return __super::on_dlg_proc(hwnd, message, w_param, l_param);
		}

	} paint_option_dialog;
}
