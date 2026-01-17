#pragma once

namespace apn::volume_meter::main_thread
{
	//
	// このクラスは描画設定ダイアログです。
	//
	inline struct paint_option_dialog_t : StdAddinDialog<idd_paint_option>
	{
		//
		// 描画設定への参照です。
		//
		inline static constexpr auto& po = model.paint_option;

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_uint(idc_background_color, po.background_color);
			get_uint(idc_warning_color, po.warning_color);
			get_uint(idc_level_color, po.level_color);
			get_int(idc_level_width, po.level_width);
			get_int(idc_min_db, po.min_db);
			get_int(idc_max_db, po.max_db);
			get_int(idc_gamma, po.gamma);
			get_int(idc_nb_separators, po.nb_separators);
			get_int(idc_separator_width, po.separator_width);
			get_int(idc_text_area_height, po.text_area_height);

			for (size_t i = 0; i < std::size(po.zones); i++)
			{
				get_int(idc_red_zone_db + i * 4, po.zones[i].db);
				get_uint(idc_red_zone_color + i * 4, po.zones[i].color);
			}

			for (size_t i = 0; i < std::size(po.fonts); i++)
			{
				get_text(idc_font_total_name + i * 100, po.fonts[i].name);
				get_int(idc_font_total_width + i * 100, po.fonts[i].width);
				get_int(idc_font_total_height + i * 100, po.fonts[i].height);
				get_uint(idc_font_total_text_color + i * 100, po.fonts[i].text_color);
				get_uint(idc_font_total_shadow_color + i * 100, po.fonts[i].shadow_color);
				get_int(idc_font_total_text_offset_x + i * 100, po.fonts[i].text_offset.x);
				get_int(idc_font_total_text_offset_y + i * 100, po.fonts[i].text_offset.y);
				get_int(idc_font_total_shadow_offset_x + i * 100, po.fonts[i].shadow_offset.x);
				get_int(idc_font_total_shadow_offset_y + i * 100, po.fonts[i].shadow_offset.y);
			}

			// コンフィグをサブスレッドにも適用します。
			sub_thread::controller.set_paint_option(po);
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_uint(idc_background_color, po.background_color);
			set_uint(idc_warning_color, po.warning_color);
			set_uint(idc_level_color, po.level_color);
			set_int(idc_level_width, po.level_width);
			set_int(idc_min_db, po.min_db);
			set_int(idc_max_db, po.max_db);
			set_int(idc_gamma, po.gamma);
			set_int(idc_nb_separators, po.nb_separators);
			set_int(idc_separator_width, po.separator_width);
			set_int(idc_text_area_height, po.text_area_height);

			for (size_t i = 0; i < std::size(po.zones); i++)
			{
				set_int(idc_red_zone_db + i * 4, po.zones[i].db);
				set_uint(idc_red_zone_color + i * 4, po.zones[i].color);
			}

			for (size_t i = 0; i < std::size(po.fonts); i++)
			{
				set_text(idc_font_total_name + i * 100, po.fonts[i].name);
				set_int(idc_font_total_width + i * 100, po.fonts[i].width);
				set_int(idc_font_total_height + i * 100, po.fonts[i].height);
				set_uint(idc_font_total_text_color + i * 100, po.fonts[i].text_color);
				set_uint(idc_font_total_shadow_color + i * 100, po.fonts[i].shadow_color);
				set_int(idc_font_total_text_offset_x + i * 100, po.fonts[i].text_offset.x);
				set_int(idc_font_total_text_offset_y + i * 100, po.fonts[i].text_offset.y);
				set_int(idc_font_total_shadow_offset_x + i * 100, po.fonts[i].shadow_offset.x);
				set_int(idc_font_total_shadow_offset_y + i * 100, po.fonts[i].shadow_offset.y);
			}
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");
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
			case idc_level_width:
			case idc_min_db:
			case idc_max_db:
			case idc_gamma:
			case idc_nb_separators:
			case idc_separator_width:
			case idc_text_area_height:
			case idc_red_zone_db:
			case idc_yellow_zone_db:
			case idc_green_zone_db:
			case idc_font_total_width:
			case idc_font_total_height:
			case idc_font_total_text_offset_x:
			case idc_font_total_text_offset_y:
			case idc_font_total_shadow_offset_x:
			case idc_font_total_shadow_offset_y:
			case idc_font_zone_width:
			case idc_font_zone_height:
			case idc_font_zone_text_offset_x:
			case idc_font_zone_text_offset_y:
			case idc_font_zone_shadow_offset_x:
			case idc_font_zone_shadow_offset_y:
			case idc_font_peak_width:
			case idc_font_peak_height:
			case idc_font_peak_text_offset_x:
			case idc_font_peak_text_offset_y:
			case idc_font_peak_shadow_offset_x:
			case idc_font_peak_shadow_offset_y:
			case idc_font_level_width:
			case idc_font_level_height:
			case idc_font_level_text_offset_x:
			case idc_font_level_text_offset_y:
			case idc_font_level_shadow_offset_x:
			case idc_font_level_shadow_offset_y:
				{
					// コントロールの値でコンフィグを更新します。
					if (code == EN_CHANGE) update_config(); break;
				}

			// 色選択ボタンの場合の処理です。
			case idc_background_color:
			case idc_warning_color:
			case idc_level_color:
			case idc_red_zone_color:
			case idc_yellow_zone_color:
			case idc_green_zone_color:
			case idc_font_total_text_color:
			case idc_font_total_shadow_color:
			case idc_font_zone_text_color:
			case idc_font_zone_shadow_color:
			case idc_font_peak_text_color:
			case idc_font_peak_shadow_color:
			case idc_font_level_text_color:
			case idc_font_level_shadow_color:
				{
					try
					{
						// カラー選択ダイアログを表示してカラーを取得します。
						auto color = magi.choose_color(hwnd, get_uint(control_id));

						// 取得した色をコントロールに適用します。
						set_uint(control_id, color);
						my::invalidate(control);

						// コントロールの値でコンフィグを更新します。
						update_config();
					}
					catch (...)
					{
					}

					break;
				}

			// フォント選択ボタンの場合の処理です。
			case idc_font_total_name:
			case idc_font_zone_name:
			case idc_font_peak_name:
			case idc_font_level_name:
				{
					font_dialog_with_handler_t font_dialog(
						[&](font_dialog_t& font_dialog)
					{
						// 適用ボタンが押されたときの処理です。

						// 取得したフォント名をコントロールに適用します。
						set_text(control_id, font_dialog.font_name);

						// コントロールの値でコンフィグを更新します。
						update_config();
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
					case idc_background_color:
					case idc_warning_color:
					case idc_level_color:
					case idc_red_zone_color:
					case idc_yellow_zone_color:
					case idc_green_zone_color:
					case idc_font_total_text_color:
					case idc_font_total_shadow_color:
					case idc_font_zone_text_color:
					case idc_font_zone_shadow_color:
					case idc_font_peak_text_color:
					case idc_font_peak_shadow_color:
					case idc_font_level_text_color:
					case idc_font_level_shadow_color:
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
						// 指数の場合の処理です。
						case idc_gamma:
							spin(-10, +10, 1, +1000); break;

						// dBの場合の処理です。
						case idc_min_db:
						case idc_max_db:
						case idc_red_zone_db:
						case idc_yellow_zone_db:
						case idc_green_zone_db:
							spin(-1, +1, -100, +100); break;

						// 大きさの場合の処理です。
						case idc_nb_separators:
						case idc_level_width:
						case idc_separator_width:
						case idc_text_area_height:
						case idc_font_total_width:
						case idc_font_total_height:
						case idc_font_zone_width:
						case idc_font_zone_height:
						case idc_font_peak_width:
						case idc_font_peak_height:
						case idc_font_level_width:
						case idc_font_level_height:
							spin(-1, +1, 0, +1000); break;

						// 座標の場合の処理です。
						case idc_font_total_text_offset_x:
						case idc_font_total_text_offset_y:
						case idc_font_total_shadow_offset_x:
						case idc_font_total_shadow_offset_y:
						case idc_font_zone_text_offset_x:
						case idc_font_zone_text_offset_y:
						case idc_font_zone_shadow_offset_x:
						case idc_font_zone_shadow_offset_y:
						case idc_font_peak_text_offset_x:
						case idc_font_peak_text_offset_y:
						case idc_font_peak_shadow_offset_x:
						case idc_font_peak_shadow_offset_y:
						case idc_font_level_text_offset_x:
						case idc_font_level_text_offset_y:
						case idc_font_level_shadow_offset_x:
						case idc_font_level_shadow_offset_y:
							spin(-1, +1, -1000, +1000); break;
						}
					}

					return FALSE;
				}
			}

			return __super::on_dlg_proc(hwnd, message, w_param, l_param);
		}

	} paint_option_dialog;
}
