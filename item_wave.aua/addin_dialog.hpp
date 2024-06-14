#pragma once

namespace apn::item_wave
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_int(IDC_SCALE, hive.scale);
			set_combobox_index(IDC_WAVE_TYPE, hive.wave_type);
			set_combobox_index(IDC_UPDATE_MODE, hive.update_mode);
			set_combobox_index(IDC_XOR_MODE, hive.xor_mode);
			set_check(IDC_SHOW_WAVE, hive.show_wave);
			set_check(IDC_SHOW_TEXT, hive.show_text);
			set_check(IDC_NAMECAGE, hive.namecage);
			set_check(IDC_BEHIND, hive.behind);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_int(IDC_SCALE, hive.scale);
			get_combobox_index(IDC_WAVE_TYPE, hive.wave_type);
			get_combobox_index(IDC_UPDATE_MODE, hive.update_mode);
			get_combobox_index(IDC_XOR_MODE, hive.xor_mode);
			get_check(IDC_SHOW_WAVE, hive.show_wave);
			get_check(IDC_SHOW_TEXT, hive.show_text);
			get_check(IDC_NAMECAGE, hive.namecage);
			get_check(IDC_BEHIND, hive.behind);

			magi.exin.invalidate();
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(IDC_WAVE_TYPE, _T("中央"), _T("下"), _T("上"));
			init_combobox(IDC_UPDATE_MODE, _T("更新しない"), _T("更新する"), _T("再生中は更新しない"));
			init_combobox(IDC_XOR_MODE, _T("通常"), _T("XOR"), _T("NotXOR"), _T("Not"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto rest = std::make_shared<AbsolutePos>(1, 1, 1);
			std::shared_ptr<AbsolutePos> q[24 + 1];
			for (auto i = 0; i < std::size(q); i++)
				q[i] = std::make_shared<AbsolutePos>(i, std::size(q) - 1);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_UPDATE_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[24], margin, ctrl(IDC_UPDATE_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_right, row, margin, ctrl(IDC_SCALE_SPIN));
				node->add_pane(c_axis.c_horz, c_align.c_right, q[12], margin, ctrl(IDC_SCALE));
				node->add_pane(c_axis.c_horz, c_align.c_right, q[24], margin, ctrl(IDC_SCALE_STAT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_WAVE_TYPE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[24], margin, ctrl(IDC_WAVE_TYPE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_XOR_MODE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[24], margin, ctrl(IDC_XOR_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_SELECT_PEN_COLOR));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[24], margin, ctrl(IDC_SELECT_BRUSH_COLOR));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_CLEAR_CACHES));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_SHOW_WAVE));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[24], margin, ctrl(IDC_SHOW_TEXT));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, q[12], margin, ctrl(IDC_NAMECAGE));
				node->add_pane(c_axis.c_horz, c_align.c_left, q[24], margin, ctrl(IDC_BEHIND));
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
			// エディットボックス
			case IDC_SCALE: if (code == EN_UPDATE) update_config(); break;

			// コンボボックス
			case IDC_WAVE_TYPE:
			case IDC_UPDATE_MODE:
			case IDC_XOR_MODE: if (code == CBN_SELCHANGE) update_config(); break;

			// ボタン
			case IDC_SELECT_PEN_COLOR:
				{
					if (IDOK == magi.exin.show_color_dialog(0, &hive.pen_color, 2))
						magi.exin.invalidate();
					break;
				}
			case IDC_SELECT_BRUSH_COLOR:
				{
					if (IDOK == magi.exin.show_color_dialog(0, &hive.brush_color, 2))
						magi.exin.invalidate();
					break;
				}
			case IDC_CLEAR_CACHES: app->clear_caches(); break;

			// チェックボックス
			case IDC_SHOW_WAVE:
			case IDC_SHOW_TEXT:
			case IDC_NAMECAGE:
			case IDC_BEHIND: update_config(); break;
			}
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_NOTIFY:
				{
					MY_TRACE_FUNC("WM_NOTIFY, {:#010x}, {:#010x}", wParam, lParam);

					switch (wParam)
					{
					case IDC_SCALE_SPIN:
						{
							auto header = (NMHDR*)lParam;
							if (header->code == UDN_DELTAPOS)
							{
								auto scale = get_int(IDC_SCALE);

								auto nm = (NM_UPDOWN*)header;
								if (nm->iDelta < 0)
								{
									scale += 10;
								}
								else
								{
									scale -= 10;
								}

								scale = std::max<int>(scale, 0);
								scale = std::min<int>(scale, 2000);

								set_int(IDC_SCALE, scale);
							}

							break;
						}
					}

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	} addin_dialog;
}
