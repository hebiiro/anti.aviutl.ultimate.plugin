#pragma once

namespace apn::item_align
{
	//
	// このクラスはアドインダイアログです。
	//
	inline struct AddinDialog : StdAddinDialog<IDD_MAIN_DIALOG>
	{
		void apply_sub_time(uint32_t id, double time)
		{
			auto text = my::format(_T("{/0.4f}"), time);
			set_text(id - 2, text);
		}

		void apply_sub_time(uint32_t id)
		{
			switch (hive.sub_time_mode)
			{
			case hive.c_sub_time_mode.c_frame:
				{
					Utils utils;

					auto frame = get_int(id);
					auto time = utils.frame_to_time(frame);
					apply_sub_time(id, time);

					break;
				}
			case hive.c_sub_time_mode.c_bpm:
				{
					Utils utils;
					if (utils.bpm == 0) break;

					auto bpm = get_int(id);
					auto time = utils.bpm_to_time(bpm);
					apply_sub_time(id, time);

					break;
				}
			}
		}

		//
		// コントロールを更新します。
		//
		virtual void on_update_controls() override
		{
			MY_TRACE_FUNC("");

			set_check(IDC_USE_CURRENT_FRAME, hive.use_current_frame);
			set_text(IDC_SHIFT_TIME, hive.shift_time);
			set_text(IDC_REL_SPACE_TIME, hive.relative_space_time);
			set_text(IDC_ABS_SPACE_TIME, hive.absolute_space_time);
			set_text(IDC_STRETCH_TIME, hive.stretch_time);
			set_text(IDC_ALIGN_TIME, hive.align_time);
			set_int(IDC_MOVE_VERT_LAYER, hive.move_vert_layer);
			set_combobox_index(IDC_SUB_TIME_MODE, hive.sub_time_mode);
			set_int(IDC_SHIFT_TIME2, hive.shift_sub_time);
			set_int(IDC_REL_SPACE_TIME2, hive.relative_space_sub_time);
			set_int(IDC_ABS_SPACE_TIME2, hive.absolute_space_sub_time);
			set_int(IDC_STRETCH_TIME2, hive.stretch_sub_time);
			set_int(IDC_ALIGN_TIME2, hive.align_sub_time);
		}

		//
		// コンフィグを更新します。
		//
		virtual void on_update_config() override
		{
			MY_TRACE_FUNC("");

			get_check(IDC_USE_CURRENT_FRAME, hive.use_current_frame);
			get_text(IDC_SHIFT_TIME, hive.shift_time);
			get_text(IDC_REL_SPACE_TIME, hive.relative_space_time);
			get_text(IDC_ABS_SPACE_TIME, hive.absolute_space_time);
			get_text(IDC_STRETCH_TIME, hive.stretch_time);
			get_text(IDC_ALIGN_TIME, hive.align_time);
			get_int(IDC_MOVE_VERT_LAYER, hive.move_vert_layer);
			get_combobox_index(IDC_SUB_TIME_MODE, hive.sub_time_mode);
			get_int(IDC_SHIFT_TIME2, hive.shift_sub_time);
			get_int(IDC_REL_SPACE_TIME2, hive.relative_space_sub_time);
			get_int(IDC_ABS_SPACE_TIME2, hive.absolute_space_sub_time);
			get_int(IDC_STRETCH_TIME2, hive.stretch_sub_time);
			get_int(IDC_ALIGN_TIME2, hive.align_sub_time);
		}

		//
		// ダイアログの初期化処理です。
		//
		virtual void on_init_dialog() override
		{
			MY_TRACE_FUNC("");

			init_combobox(IDC_SUB_TIME_MODE, _T("なし"), _T("フレーム"), _T("BPM"));

			using namespace my::layout;

			auto margin_value = 2;
			auto margin = RECT { margin_value, margin_value, margin_value, margin_value };
			auto base_size = get_base_size();
			auto row = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto stat = std::make_shared<RelativePos>(base_size * 4);
			auto arrow = std::make_shared<RelativePos>(base_size + margin_value * 2);
			auto stat2 = std::make_shared<RelativePos>(base_size * 2);
			auto time = std::make_shared<RelativePos>(base_size * 3 + margin_value * 2);
			auto time2 = std::make_shared<RelativePos>(base_size * 3 + margin_value * 2);
			auto checkbox = std::make_shared<RelativePos>(base_size * 6);
			auto combobox = std::make_shared<RelativePos>(base_size * 4);
			auto rest = std::make_shared<AbsolutePos>(1, 1, 1);

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, checkbox, margin, ctrl(IDC_USE_CURRENT_FRAME));
				node->add_pane(c_axis.c_horz, c_align.c_left, combobox, margin, ctrl(IDC_SUB_TIME_MODE));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_ALIGN_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_ALIGN_LEFT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_ALIGN_RIGHT));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_ALIGN_STAT2));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(IDC_ALIGN_TIME));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(IDC_ALIGN_TIME2));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_STRETCH_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_STRETCH_LEFT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_STRETCH_RIGHT));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_STRETCH_STAT2));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(IDC_STRETCH_TIME));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(IDC_STRETCH_TIME2));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_REL_SPACE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_REL_SPACE_LEFT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_REL_SPACE_RIGHT));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_REL_SPACE_STAT2));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(IDC_REL_SPACE_TIME));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(IDC_REL_SPACE_TIME2));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_ABS_SPACE_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_ABS_SPACE_LEFT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_ABS_SPACE_RIGHT));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_ABS_SPACE_STAT2));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(IDC_ABS_SPACE_TIME));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(IDC_ABS_SPACE_TIME2));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_SHIFT_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_SHIFT_DOWN));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_SHIFT_UP));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_SHIFT_STAT2));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(IDC_SHIFT_TIME));
				node->add_pane(c_axis.c_horz, c_align.c_left, time2, margin, ctrl(IDC_SHIFT_TIME2));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, stat, margin, ctrl(IDC_MOVE_VERT_STAT));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_MOVE_VERT_DOWN));
				node->add_pane(c_axis.c_horz, c_align.c_left, arrow, margin, ctrl(IDC_MOVE_VERT_UP));
				node->add_pane(c_axis.c_horz, c_align.c_left, stat2, margin, ctrl(IDC_MOVE_VERT_STAT2));
				node->add_pane(c_axis.c_horz, c_align.c_left, time, margin, ctrl(IDC_MOVE_VERT_LAYER));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(IDC_FIX_BPM));
			}

			{
				auto node = root->add_pane(c_axis.c_vert, c_align.c_top, row);
				node->add_pane(c_axis.c_horz, c_align.c_left, rest, margin, ctrl(IDC_ERASE_MIDPT));
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
			// ボタン
			case IDC_ALIGN_LEFT: hive.app->align_left(); break;
			case IDC_ALIGN_RIGHT: hive.app->align_right(); break;
			case IDC_STRETCH_LEFT: hive.app->stretch_left(); break;
			case IDC_STRETCH_RIGHT: hive.app->stretch_right(); break;
			case IDC_REL_SPACE_LEFT: hive.app->relative_space_left(); break;
			case IDC_REL_SPACE_RIGHT: hive.app->relative_space_right(); break;
			case IDC_ABS_SPACE_LEFT: hive.app->absolute_space_left(); break;
			case IDC_ABS_SPACE_RIGHT: hive.app->absolute_space_right(); break;
			case IDC_SHIFT_UP: hive.app->shift_up(); break;
			case IDC_SHIFT_DOWN: hive.app->shift_down(); break;
			case IDC_MOVE_VERT_DOWN: hive.app->move_vert_down(); break;
			case IDC_MOVE_VERT_UP: hive.app->move_vert_up(); break;
			case IDC_FIX_BPM: hive.app->fix_bpm(); break;
			case IDC_ERASE_MIDPT: hive.app->erase_midpt(); break;

			// エディットボックス
			case IDC_ALIGN_TIME:
			case IDC_STRETCH_TIME:
			case IDC_REL_SPACE_TIME:
			case IDC_ABS_SPACE_TIME:
			case IDC_SHIFT_TIME:
			case IDC_MOVE_VERT_LAYER: if (code == EN_CHANGE) update_config(); break;

			// サブエディットボックス
			case IDC_ALIGN_TIME2:
			case IDC_STRETCH_TIME2:
			case IDC_REL_SPACE_TIME2:
			case IDC_ABS_SPACE_TIME2:
			case IDC_SHIFT_TIME2: if (code == EN_CHANGE) apply_sub_time(id); break;

			// コンボボックス
			case IDC_SUB_TIME_MODE: if (code == CBN_SELCHANGE) update_config(); break;

			// その他
			case IDC_USE_CURRENT_FRAME: update_config(); break;
			}
		}
	} addin_dialog;
}
